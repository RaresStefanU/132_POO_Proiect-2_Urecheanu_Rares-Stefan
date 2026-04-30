#pragma once
#include "carte.h"
#include <vector>
#include <memory>
#include <string>
#include <ostream>

// -------------------------------------------------------
// Ierarhie de jucatori:
//   Jucator (abstracta)
//     |- JucatorUman   — citeste decizia de la stdin
//     |- JucatorAI     — decide automat dupa strategie
// -------------------------------------------------------

class Jucator {
public:
    explicit Jucator(std::string nume);
    virtual ~Jucator() = default;

    // Constructor virtual
    virtual std::unique_ptr<Jucator> clone() const = 0;

    // Decide ce carte sa joace ca lider (prima carte a rundei)
    virtual std::unique_ptr<Carte> alege_carte_lider() = 0;

    // Decide daca sa taie si ce carte sa joace (nullptr = nu taie)
    virtual std::unique_ptr<Carte> alege_carte_taiere(const Carte& referinta) = 0;

    // Getteri/operatii comune
    void        adauga_carte(std::unique_ptr<Carte> c);
    void        adauga_castigate(std::unique_ptr<Carte> c);
    bool        are_carti()   const { return !mana_.empty(); }
    int         nr_carti()    const { return static_cast<int>(mana_.size()); }
    int         puncte_castigate() const;
    std::string nume()        const { return nume_; }

    // Numar total de jucatori creati (static)
    static int totalJucatori() { return total_jucatori_; }

    friend std::ostream& operator<<(std::ostream& os, const Jucator& j);

protected:
    // Cauta in mana o carte care poate taia referinta
    // Returneaza indexul sau -1 daca nu exista
    int cauta_taietor(const Carte& referinta) const;

    // Cauta in mana cartea cu cel mai mic punctaj (preferabil neutrale)
    int cauta_cea_mai_slaba() const;

    std::string                         nume_;
    std::vector<std::unique_ptr<Carte>> mana_;
    std::vector<std::unique_ptr<Carte>> castigate_;

    Jucator(const Jucator& other);
    // op= nu se defineste in baza abstracta; subclasele implementeaza copy-and-swap
    friend void swap(Jucator& a, Jucator& b);

private:
    static int total_jucatori_;
};

// ------ JucatorUman ------
class JucatorUman : public Jucator {
public:
    explicit JucatorUman(const std::string& nume);
    std::unique_ptr<Jucator> clone() const override;

    // Copy-and-swap complet in subclasa concreta
    JucatorUman(const JucatorUman& other) : Jucator(other) {}
    JucatorUman& operator=(JucatorUman other) { swap(*this, other); return *this; }

    std::unique_ptr<Carte> alege_carte_lider()               override;
    std::unique_ptr<Carte> alege_carte_taiere(const Carte& r) override;

private:
    void afiseaza_mana() const;
    int  citeste_index(int min, int max) const;
};

// ------ JucatorAI ------
class JucatorAI : public Jucator {
public:
    explicit JucatorAI(const std::string& nume);
    std::unique_ptr<Jucator> clone() const override;

    // Copy-and-swap complet in subclasa concreta
    JucatorAI(const JucatorAI& other) : Jucator(other) {}
    JucatorAI& operator=(JucatorAI other) { swap(*this, other); return *this; }

    std::unique_ptr<Carte> alege_carte_lider()               override;
    std::unique_ptr<Carte> alege_carte_taiere(const Carte& r) override;

private:
    bool mana_are_punct() const;
    int  cauta_momeala()  const;
};
