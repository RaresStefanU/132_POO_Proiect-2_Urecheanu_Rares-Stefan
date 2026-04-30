#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ============================================================
// 1. LISTA DUBLU INLANTUITA - mana jucatorului
// ============================================================
struct Card {
    string suit;  // Cupa, Trefla, Romb, Pica
    int value;    // 1-10 (1=As, 7=Septica, 10=Zece)
    string name;  // "7 de Cupa" etc
};

struct ListNode {
    Card card;
    ListNode* prev;
    ListNode* next;
    ListNode(Card c) : card(c), prev(nullptr), next(nullptr) {}
};

class Hand {
    ListNode* head;
    ListNode* tail;
    int size;
public:
    Hand() : head(nullptr), tail(nullptr), size(0) {}

    void push_back(Card c) {
        ListNode* n = new ListNode(c);
        if (!tail) { head = tail = n; }
        else { tail->next = n; n->prev = tail; tail = n; }
        size++;
    }

    void push_front(Card c) {
        ListNode* n = new ListNode(c);
        if (!head) { head = tail = n; }
        else { n->next = head; head->prev = n; head = n; }
        size++;
    }

    // Insert la pozitie k (0-indexed)
    void insert(int k, Card c) {
        if (k <= 0) { push_front(c); return; }
        if (k >= size) { push_back(c); return; }
        ListNode* cur = head;
        for (int i = 0; i < k; i++) cur = cur->next;
        ListNode* n = new ListNode(c);
        n->prev = cur->prev;
        n->next = cur;
        cur->prev->next = n;
        cur->prev = n;
        size++;
    }

    // Cauta o carte dupa valoare si culoare, returneaza indexul sau -1
    int search(int value, string suit) {
        ListNode* cur = head;
        int idx = 0;
        while (cur) {
            if (cur->card.value == value && cur->card.suit == suit) return idx;
            cur = cur->next;
            idx++;
        }
        return -1;
    }

    // Sterge carte dupa index, returneaza cartea stearsa
    Card removeAt(int idx) {
        ListNode* cur = head;
        for (int i = 0; i < idx; i++) cur = cur->next;
        Card c = cur->card;
        if (cur->prev) cur->prev->next = cur->next;
        else head = cur->next;
        if (cur->next) cur->next->prev = cur->prev;
        else tail = cur->prev;
        delete cur;
        size--;
        return c;
    }

    // Sterge prima carte (pentru a da carte)
    Card removeFront() {
        return removeAt(0);
    }

    void print() {
        ListNode* cur = head;
        int i = 0;
        while (cur) {
            cout << "  [" << i << "] " << cur->card.name << "\n";
            cur = cur->next;
            i++;
        }
    }

    int getSize() { return size; }
    bool isEmpty() { return size == 0; }

    ListNode* getHead() { return head; }

    ~Hand() {
        ListNode* cur = head;
        while (cur) { ListNode* tmp = cur->next; delete cur; cur = tmp; }
    }
};

// ============================================================
// 2. MIN-HEAP - scorul jucatorilor (scor mai mic = mai bun)
// ============================================================
struct PlayerScore {
    string name;
    int score;
};

class ScoreHeap {
    PlayerScore heap[10];
    int n;

    int parent(int i) { return (i - 1) / 2; }
    int left(int i)   { return 2 * i + 1; }
    int right(int i)  { return 2 * i + 2; }

    void heapifyUp(int i) {
        while (i > 0 && heap[parent(i)].score > heap[i].score) {
            swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    void heapifyDown(int i) {
        int smallest = i;
        if (left(i) < n && heap[left(i)].score < heap[smallest].score)   smallest = left(i);
        if (right(i) < n && heap[right(i)].score < heap[smallest].score) smallest = right(i);
        if (smallest != i) { swap(heap[i], heap[smallest]); heapifyDown(smallest); }
    }

public:
    ScoreHeap() : n(0) {}

    void insert(PlayerScore ps) {
        heap[n++] = ps;
        heapifyUp(n - 1);
    }

    // Actualizeaza scorul unui jucator
    void updateScore(string name, int newScore) {
        for (int i = 0; i < n; i++) {
            if (heap[i].name == name) {
                heap[i].score = newScore;
                heapifyUp(i);
                heapifyDown(i);
                return;
            }
        }
    }

    // Cauta scorul unui jucator
    int search(string name) {
        for (int i = 0; i < n; i++)
            if (heap[i].name == name) return heap[i].score;
        return -1;
    }

    // Sterge jucatorul cu scorul minim (castigatorul rundei)
    PlayerScore extractMin() {
        PlayerScore min = heap[0];
        heap[0] = heap[--n];
        heapifyDown(0);
        return min;
    }

    void print() {
        cout << "  Scoruri curente:\n";
        for (int i = 0; i < n; i++)
            cout << "    " << heap[i].name << ": " << heap[i].score << " puncte\n";
    }

    int getSize() { return n; }
};

// ============================================================
// 3. TREAP - talonul (cartile jucate)
// ============================================================
struct TreapNode {
    Card card;
    int priority;
    int key; // ordinea in care a fost pusa cartea
    TreapNode *left, *right;
    TreapNode(Card c, int k) : card(c), priority(rand()), key(k), left(nullptr), right(nullptr) {}
};

class Talon {
    TreapNode* root;
    int counter; // cate carti au fost puse

    TreapNode* rotateRight(TreapNode* y) {
        TreapNode* x = y->left;
        y->left = x->right;
        x->right = y;
        return x;
    }

    TreapNode* rotateLeft(TreapNode* x) {
        TreapNode* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    TreapNode* insert(TreapNode* n, Card c, int key) {
        if (!n) return new TreapNode(c, key);
        if (key < n->key) {
            n->left = insert(n->left, c, key);
            if (n->left->priority > n->priority) n = rotateRight(n);
        } else {
            n->right = insert(n->right, c, key);
            if (n->right->priority > n->priority) n = rotateLeft(n);
        }
        return n;
    }

    TreapNode* remove(TreapNode* n, int key) {
        if (!n) return nullptr;
        if (key < n->key) n->left = remove(n->left, key);
        else if (key > n->key) n->right = remove(n->right, key);
        else {
            if (!n->left && !n->right) { delete n; return nullptr; }
            else if (!n->left) { n = rotateLeft(n); n->left = remove(n->left, key); }
            else if (!n->right) { n = rotateRight(n); n->right = remove(n->right, key); }
            else {
                if (n->left->priority > n->right->priority) { n = rotateRight(n); n->right = remove(n->right, key); }
                else { n = rotateLeft(n); n->left = remove(n->left, key); }
            }
        }
        return n;
    }

    // Cauta o carte in talon dupa valoare si culoare
    bool search(TreapNode* n, int value, string suit) {
        if (!n) return false;
        if (n->card.value == value && n->card.suit == suit) return true;
        return search(n->left, value, suit) || search(n->right, value, suit);
    }

    // Afiseaza talonul in ordinea jucarii (inorder pe key)
    void inorder(TreapNode* n) {
        if (!n) return;
        inorder(n->left);
        cout << n->card.name << " ";
        inorder(n->right);
    }

    // Returneaza ultima carte pusa (maximul din treap)
    TreapNode* getMax(TreapNode* n) {
        if (!n) return nullptr;
        while (n->right) n = n->right;
        return n;
    }

    void destroy(TreapNode* n) {
        if (!n) return;
        destroy(n->left); destroy(n->right); delete n;
    }

    // Numara cartile in talon
    int count(TreapNode* n) {
        if (!n) return 0;
        return 1 + count(n->left) + count(n->right);
    }

public:
    Talon() : root(nullptr), counter(0) {}

    void addCard(Card c) {
        root = insert(root, c, counter++);
    }

    Card getTopCard() {
        TreapNode* m = getMax(root);
        return m ? m->card : Card{"", 0, "Gol"};
    }

    bool searchCard(int value, string suit) {
        return search(root, value, suit);
    }

    // Goleste talonul (la inceputul unei runde noi)
    void clear() {
        destroy(root);
        root = nullptr;
        counter = 0;
    }

    int size() { return count(root); }

    void print() {
        cout << "Talon (in ordine): ";
        inorder(root);
        cout << "\n";
    }

    ~Talon() { destroy(root); }
};

// ============================================================
// LOGICA JOCULUI - SEPTICA
// ============================================================

// Creaza un pachet de 32 de carti (7, 8, 9, 10, J, Q, K, A)
void createDeck(Card deck[], int& deckSize) {
    string suits[] = {"Cupa", "Trefla", "Romb", "Pica"};
    // valori: 7,8,9,10,J(11),Q(12),K(13),A(1)
    int values[]   = {7, 8, 9, 10, 11, 12, 13, 1};
    string names[] = {"7", "8", "9", "10", "J", "Q", "K", "A"};
    deckSize = 0;
    for (auto& s : suits) {
        for (int i = 0; i < 8; i++) {
            Card c;
            c.suit  = s;
            c.value = values[i];
            c.name  = names[i] + " de " + s;
            deck[deckSize++] = c;
        }
    }
}

void shuffleDeck(Card deck[], int deckSize) {
    for (int i = deckSize - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(deck[i], deck[j]);
    }
}

// Verifica daca o carte poate fi pusa pe talon
bool canPlay(Card toPlay, Card topCard) {
    // Poti pune daca: aceeasi culoare SAU aceeasi valoare SAU e o Septica (7)
    if (toPlay.value == 7) return true;
    if (toPlay.suit == topCard.suit) return true;
    if (toPlay.value == topCard.value) return true;
    return false;
}

// Valoarea unei carti pentru scor (cartile ramase in mana conteaza ca puncte negative)
int cardPoints(Card c) {
    if (c.value == 1)  return 11; // As
    if (c.value == 10) return 10;
    if (c.value == 13) return 4;  // K
    if (c.value == 12) return 3;  // Q
    if (c.value == 11) return 2;  // J
    return 0; // 7,8,9 nu au puncte
}

void printSeparator() {
    cout << "\n========================================\n";
}

void printHeader(string text) {
    printSeparator();
    cout << "  " << text << "\n";
    printSeparator();
}

int main() {
    srand(time(0));

    printHeader("Bun venit la SEPTICA!");
    cout << "Joc pentru 2 jucatori la acelasi PC.\n";
    cout << "Reguli: Pui o carte de aceeasi culoare, aceeasi valoare, sau o Septica (7).\n";
    cout << "Septica (7) obliga adversarul sa ia 2 carti din pachet!\n";
    cout << "Castiga primul care ramane fara carti.\n\n";

    string name1, name2;
    cout << "Numele Jucatorului 1: ";
    cin >> name1;
    cout << "Numele Jucatorului 2: ";
    cin >> name2;

    // Scoruri initiale in heap
    ScoreHeap scoreHeap;
    scoreHeap.insert({name1, 0});
    scoreHeap.insert({name2, 0});

    int roundNum = 1;
    bool playAgain = true;

    while (playAgain) {
        printHeader("RUNDA " + to_string(roundNum));

        // Creare si amestecare pachet
        Card deck[32];
        int deckSize = 0;
        createDeck(deck, deckSize);
        shuffleDeck(deck, deckSize);

        // Mainile jucatorilor (Liste dublu inlantuite)
        Hand hand1, hand2;
        int deckIdx = 0;

        // Impartim cate 5 carti fiecarui jucator
        for (int i = 0; i < 5; i++) {
            hand1.push_back(deck[deckIdx++]);
            hand2.push_back(deck[deckIdx++]);
        }

        // Talonul (Treap) - prima carte
        Talon talon;
        // Asiguram ca prima carte din talon nu e o Septica
        while (deck[deckIdx].value == 7 && deckIdx < deckSize - 1) deckIdx++;
        talon.addCard(deck[deckIdx++]);

        cout << "Carti impartite! Prima carte din talon: " << talon.getTopCard().name << "\n";

        int currentPlayer = 1; // 1 sau 2
        int septiciPending = 0; // cate septicii consecutive au fost jucate
        bool roundOver = false;

        while (!roundOver) {
            Hand& currentHand = (currentPlayer == 1) ? hand1 : hand2;
            Hand& otherHand   = (currentPlayer == 1) ? hand2 : hand1;
            string currentName = (currentPlayer == 1) ? name1 : name2;
            string otherName   = (currentPlayer == 1) ? name2 : name1;

            printHeader("Randul lui " + currentName);
            cout << "Carte din varf talon: " << talon.getTopCard().name << "\n";
            cout << "Carti ramase in pachet: " << (deckSize - deckIdx) << "\n\n";

            // Daca sunt Septici in asteptare si jucatorul curent nu are Septica
            if (septiciPending > 0) {
                // Verificam daca jucatorul curent are o Septica
                bool hasSeven = false;
                ListNode* cur = currentHand.getHead();
                while (cur) {
                    if (cur->card.value == 7) { hasSeven = true; break; }
                    cur = cur->next;
                }

                if (!hasSeven) {
                    // Trebuie sa ia septiciPending * 2 carti
                    int toTake = septiciPending * 2;
                    cout << currentName << " nu are Septica si trebuie sa ia " << toTake << " carti!\n";
                    for (int i = 0; i < toTake && deckIdx < deckSize; i++) {
                        currentHand.push_back(deck[deckIdx++]);
                        cout << "  + " << currentHand.getHead()->card.name << "\n";
                    }
                    septiciPending = 0;
                    currentPlayer = (currentPlayer == 1) ? 2 : 1;
                    continue;
                }
            }

            cout << "Mana ta (" << currentHand.getSize() << " carti):\n";
            currentHand.print();
            cout << "\n";

            // Verificam daca jucatorul are vreo carte valida
            bool hasValidCard = false;
            ListNode* cur = currentHand.getHead();
            while (cur) {
                if (canPlay(cur->card, talon.getTopCard())) { hasValidCard = true; break; }
                cur = cur->next;
            }

            int choice = -1;

            if (!hasValidCard) {
                cout << currentName << " nu are carte valida. Ia o carte din pachet.\n";
                if (deckIdx < deckSize) {
                    Card drawn = deck[deckIdx++];
                    currentHand.push_back(drawn);
                    cout << "  Ai primit: " << drawn.name << "\n";
                    // Verificam daca cartea trasa e valida
                    if (canPlay(drawn, talon.getTopCard())) {
                        cout << "  Poti juca " << drawn.name << "! (1=Da, 0=Nu): ";
                        int play; cin >> play;
                        if (play == 1) choice = currentHand.getSize() - 1;
                    }
                } else {
                    cout << "  Pachetul e gol! Treci randul.\n";
                }
            } else {
                cout << "Alege indexul cartii de jucat (sau -1 pentru a lua din pachet): ";
                cin >> choice;

                if (choice == -1) {
                    // Ia din pachet
                    if (deckIdx < deckSize) {
                        Card drawn = deck[deckIdx++];
                        currentHand.push_back(drawn);
                        cout << "  Ai primit: " << drawn.name << "\n";
                    } else {
                        cout << "  Pachetul e gol!\n";
                    }
                    choice = -1;
                }
            }

            if (choice >= 0 && choice < currentHand.getSize()) {
                // Verificam daca alegerea e valida
                ListNode* chosen = currentHand.getHead();
                for (int i = 0; i < choice; i++) chosen = chosen->next;

                if (!canPlay(chosen->card, talon.getTopCard())) {
                    cout << "  Carte invalida! Nu poti juca " << chosen->card.name << " peste " << talon.getTopCard().name << ". Randul trecut.\n";
                } else {
                    Card played = currentHand.removeAt(choice);
                    talon.addCard(played);
                    cout << "  " << currentName << " a jucat: " << played.name << "\n";

                    if (played.value == 7) {
                        septiciPending++;
                        cout << "  SEPTICA! " << otherName << " va trebui sa ia " << septiciPending * 2 << " carti!\n";
                    } else {
                        septiciPending = 0;
                    }

                    // Verificam daca jucatorul a castigat
                    if (currentHand.isEmpty()) {
                        printHeader(currentName + " A CASTIGAT RUNDA " + to_string(roundNum) + "!");

                        // Calculam penalitati pentru cel care a pierdut
                        int penalty = 0;
                        ListNode* c = otherHand.getHead();
                        while (c) { penalty += cardPoints(c->card); c = c->next; }

                        int oldScore = scoreHeap.search(otherName);
                        scoreHeap.updateScore(otherName, oldScore + penalty);
                        cout << otherName << " primeste " << penalty << " puncte penalizare.\n\n";
                        scoreHeap.print();
                        roundOver = true;
                        continue;
                    }
                }
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
            } else if (choice == -1) {
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
            }
        }

        roundNum++;
        cout << "\nJucati inca o runda? (1=Da, 0=Nu): ";
        int again; cin >> again;
        playAgain = (again == 1);
    }

    // Rezultat final
    printHeader("REZULTAT FINAL");
    scoreHeap.print();

    int score1 = scoreHeap.search(name1);
    int score2 = scoreHeap.search(name2);

    cout << "\n";
    if (score1 < score2)
        cout << "  Castigatorul overall este: " << name1 << " cu " << score1 << " puncte!\n";
    else if (score2 < score1)
        cout << "  Castigatorul overall este: " << name2 << " cu " << score2 << " puncte!\n";
    else
        cout << "  Egalitate! Ambii jucatori au " << score1 << " puncte.\n";

    cout << "\nMultumim ca ai jucat Septica!\n";
    return 0;
}
