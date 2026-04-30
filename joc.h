#pragma once
#include "pachet.h"
#include "jucator.h"
#include <vector>
#include <memory>
#include <ostream>

// Starea curenta a jocului
enum class StareJoc { InDesfasurare, Terminat };

class Joc {
public:
    // Preia ownership-ul jucatorilor
    Joc(std::unique_ptr<Jucator> j1, std::unique_ptr<Jucator> j2);

    // Ruleaza jocul complet
    void ruleaza();

    // Accessor la stare
    StareJoc stare() const { return stare_; }

    // Numar total de jocuri initializate (static)
    static int totalJocuri() { return total_jocuri_; }

    // Non-copyable
    Joc(const Joc&) = delete;
    Joc& operator=(const Joc&) = delete;

private:
    void imparte_initial();
    void completeaza_maini(int idx_castigator);
    // Ruleaza o singura mana; returneaza indexul castigatorului (0 sau 1)
    int  joaca_mana(int idx_lider);
    void afiseaza_stoc() const;
    void afiseaza_scor() const;

    Pachet pachet_;
    std::unique_ptr<Jucator> jucatori_[2];
    StareJoc stare_ = StareJoc::InDesfasurare;

    static int total_jocuri_;
    static const int MARIME_MANA = 4;
    static const int PUNCTE_VICTORIE = 5;
};
