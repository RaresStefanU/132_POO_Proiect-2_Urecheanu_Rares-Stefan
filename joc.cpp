#include "joc.h"
#include "exceptii.h"
#include <iostream>
#include <vector>

int Joc::total_jocuri_ = 0;

Joc::Joc(std::unique_ptr<Jucator> j1, std::unique_ptr<Jucator> j2) {
    ++total_jocuri_;
    jucatori_[0] = std::move(j1);
    jucatori_[1] = std::move(j2);
    pachet_.amesteca();
}

void Joc::afiseaza_stoc() const {
    std::cout << "  [Stoc: " << pachet_.marime() << " carti ramase]\n";
}

void Joc::afiseaza_scor() const {
    for (int i = 0; i < 2; ++i)
        std::cout << "  " << jucatori_[i]->nume()
                  << ": " << jucatori_[i]->puncte_castigate() << " puncte\n";
}

void Joc::imparte_initial() {
    // Impartim alternativ 4 carti fiecarui jucator
    for (int runda = 0; runda < MARIME_MANA; ++runda) {
        for (int j = 0; j < 2; ++j) {
            if (pachet_.esteGol()) throw ExceptieStocGol{};
            jucatori_[j]->adauga_carte(pachet_.trage());
        }
    }
}

void Joc::completeaza_maini(int idx_castigator) {
    // Castigatorul trage primul, conform regulilor
    for (int i = 0; i < 2; ++i) {
        int idx = (idx_castigator + i) % 2;
        while (!pachet_.esteGol() && jucatori_[idx]->nr_carti() < MARIME_MANA)
            jucatori_[idx]->adauga_carte(pachet_.trage());
    }
}

int Joc::joaca_mana(int idx_lider) {
    // Cartile pe masa (vor fi luate de castigator)
    std::vector<std::unique_ptr<Carte>> masa;

    std::cout << "\n========== MANA NOUA ==========\n";
    std::cout << jucatori_[0]->nume() << " (lider: " << (idx_lider==0?"da":"nu") << ")   ";
    std::cout << jucatori_[1]->nume() << " (lider: " << (idx_lider==1?"da":"nu") << ")\n";
    afiseaza_stoc();

    // ------ Liderul joaca prima carte ------
    int idx_secundar = 1 - idx_lider;
    auto prima_carte = jucatori_[idx_lider]->alege_carte_lider();
    const Carte* referinta = prima_carte.get();
    std::cout << "\n>>> " << jucatori_[idx_lider]->nume()
              << " deschide cu: " << *prima_carte << "\n";
    masa.push_back(std::move(prima_carte));

    // idx_curent indica cel care trebuie sa raspunda
    int idx_curent    = idx_secundar;
    int idx_ultim_taiator = idx_lider;

    // ------ Bucla de taietur ------
    while (true) {
        auto taiere = jucatori_[idx_curent]->alege_carte_taiere(*referinta);

        if (!taiere) {
            // Nu taie -> runda se opreste
            std::cout << jucatori_[idx_curent]->nume() << " nu taie.\n";
            break;
        }

        // Validare suplimentara (mai ales pentru uman)
        if (!taiere->poateTaia(*referinta)) {
            std::cout << "[EROARE] Cartea " << *taiere << " nu poate taia " << *referinta << "!\n";
            // Re-adauga in mana si cere din nou — nu se intampla la AI
            jucatori_[idx_curent]->adauga_carte(std::move(taiere));
            continue;
        }

        std::cout << ">>> " << jucatori_[idx_curent]->nume()
                  << " taie cu: " << *taiere << "\n";

        referinta = taiere.get();
        idx_ultim_taiator = idx_curent;
        masa.push_back(std::move(taiere));

        // Schimba randul
        idx_curent = 1 - idx_curent;
    }

    // ------ Castigatorul preia cartile ------
    std::cout << "\n" << jucatori_[idx_ultim_taiator]->nume()
              << " castiga " << masa.size() << " carti!\n";

    int puncte_in_mana = 0;
    for (auto& c : masa) {
        puncte_in_mana += c->puncte();
        jucatori_[idx_ultim_taiator]->adauga_castigate(std::move(c));
    }
    if (puncte_in_mana > 0)
        std::cout << "  (++" << puncte_in_mana << " punct(e) de valoare!)\n";

    std::cout << "Scor curent:\n";
    afiseaza_scor();

    return idx_ultim_taiator;
}

void Joc::ruleaza() {
    std::cout << "╔══════════════════════════╗\n";
    std::cout << "║      JOC: SEPTICA        ║\n";
    std::cout << "╚══════════════════════════╝\n";
    std::cout << "Jucatori: " << jucatori_[0]->nume()
              << " vs " << jucatori_[1]->nume() << "\n";
    std::cout << "Total carti instante create: " << Carte::totalInstante() << "\n\n";

    imparte_initial();

    int idx_lider = 0; // Jucatorul 0 incepe

    // ------ Bucla principala ------
    while (jucatori_[0]->are_carti() || jucatori_[1]->are_carti()) {
        // Daca un jucator nu mai are carti si stocul e gol, jocul se termina
        if (!jucatori_[idx_lider]->are_carti()) break;

        int castigator = joaca_mana(idx_lider);
        idx_lider = castigator;

        // Completeaza mainile daca mai sunt carti in stoc
        if (!pachet_.esteGol())
            completeaza_maini(castigator);
    }

    // ------ Evaluare finala ------
    stare_ = StareJoc::Terminat;
    std::cout << "\n╔══════════════════════════╗\n";
    std::cout << "║     REZULTAT FINAL       ║\n";
    std::cout << "╚══════════════════════════╝\n";
    afiseaza_scor();

    int p0 = jucatori_[0]->puncte_castigate();
    int p1 = jucatori_[1]->puncte_castigate();

    if (p0 >= PUNCTE_VICTORIE)
        std::cout << "\nCASTIGATOR: " << jucatori_[0]->nume() << "! (" << p0 << " puncte)\n";
    else if (p1 >= PUNCTE_VICTORIE)
        std::cout << "\nCASTIGATOR: " << jucatori_[1]->nume() << "! (" << p1 << " puncte)\n";
    else if (p0 == p1)
        std::cout << "\nREMIZA! (" << p0 << " - " << p1 << ")\n";
    else {
        // 4-4 nu se poate (total 8), dar poate fi 4-4 daca total < 8 ramane in stoc? Nu.
        // Fallback: mai multe puncte castiga
        auto& castigator_final = (p0 > p1) ? jucatori_[0] : jucatori_[1];
        std::cout << "\nCASTIGATOR: " << castigator_final->nume() << "!\n";
    }

    std::cout << "\n[Statistici] Jocuri totale: " << Joc::totalJocuri()
              << " | Jucatori creati: " << Jucator::totalJucatori()
              << " | Pachete create: " << Pachet::totalPachete() << "\n";
}
