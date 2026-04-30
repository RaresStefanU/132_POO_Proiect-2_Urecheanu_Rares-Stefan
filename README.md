# 🃏 Șeptica — Joc de Cărți în C++

Proiect realizat pentru materia **POO** — FMI UniBuc.

Joc de cărți Șeptica pentru **2 jucători la același calculator**, în mod text (consolă), cu trei structuri de date implementate de la zero și integrate în logica jocului.

---

## Structuri de Date

| Structură | Rol în joc |
|---|---|
| **Listă Dublu Înlănțuită** | Mâna fiecărui jucător |
| **Min-Heap** | Scorul jucătorilor |
| **Treap** | Talonul de cărți jucate |

### Listă Dublu Înlănțuită — Mâna Jucătorului
Operații implementate: `push_front`, `push_back`, `insert(k)`, `search`, `removeAt`.

### Min-Heap — Scorul Jucătorilor
Operații implementate: `insert`, `search`, `updateScore`, `extractMin`.

### Treap — Talonul
Un BST balansat probabilistic cu priorități aleatoare. Operații implementate: `addCard`, `searchCard`, `clear`, `getTopCard`.

---

## Regulile Jocului

- Fiecare jucător primește **5 cărți** la începutul rundei
- Poți juca o carte dacă are **aceeași culoare**, **aceeași valoare**, sau este o **Septica (7)**
- Septica obligă adversarul să ia **2 cărți** din pachet (efectul se cumulează dacă și el are Septica)
- Dacă nu ai carte validă, tragi o carte din pachet
- Câștigă runda primul jucător care **rămâne fără cărți**
- Penalizare: As=11, 10=10, K=4, Q=3, J=2, restul=0 puncte
- Câștigătorul overall are **scorul total cel mai mic**

---

## Compilare și Rulare

```bash
g++ -o septica septica.cpp
./septica
```

---

## Structura Codului

```
septica.cpp
├── Listă Dublu Înlănțuită   (struct ListNode, class Hand)
├── Min-Heap                  (struct PlayerScore, class ScoreHeap)
├── Treap                     (struct TreapNode, class Talon)
└── Logica jocului            (funcții joc + main)
```

---

## Complexitate

| Structură | Operație | Complexitate |
|---|---|---|
| Listă | push_front / push_back | O(1) |
| Listă | insert(k) / removeAt(k) | O(k) |
| Listă | search | O(n) |
| Min-Heap | insert / updateScore | O(log n) |
| Min-Heap | extractMin | O(log n) |
| Treap | insert / remove / search | O(log n) mediu |
