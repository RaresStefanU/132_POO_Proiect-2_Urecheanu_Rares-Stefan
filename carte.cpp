#include "carte.h"
#include "exceptii.h"
#include <sstream>

int Carte::total_instante_ = 0;

// ----------- Carte (baza) -----------

Carte::Carte(int val, std::string col)
    : valoare_(val), culoare_(std::move(col))
{
    ++total_instante_;
}

std::ostream& operator<<(std::ostream& os, const Carte& c) {
    os << c.descriere();
    return os;
}

// ----------- CarteNormala -----------

static std::string numeValoare(int v) {
    switch(v) {
        case  7: return "7";
        case  8: return "8";
        case  9: return "9";
        case 10: return "10";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        case 14: return "A";
        default: return "?";
    }
}

CarteNormala::CarteNormala(int val, const std::string& col)
    : Carte(val, col)
{
    // 8,9,J,Q,K sunt normale; 10 si A sunt CartePunct; 7 este CarteSeptar
    if (val == 7 || val == 10 || val == 14)
        throw ExceptieCarteInvalida("valoare " + std::to_string(val) + " nu este carte normala");
}

std::unique_ptr<Carte> CarteNormala::clone() const {
    return std::make_unique<CarteNormala>(*this);
}

bool CarteNormala::poateTaia(const Carte& alta) const {
    // taie daca are aceeasi valoare ca referinta
    return valoare_ == alta.valoare();
}

std::string CarteNormala::descriere() const {
    return numeValoare(valoare_) + " de " + culoare_;
}

// ----------- CartePunct -----------

CartePunct::CartePunct(int val, const std::string& col)
    : Carte(val, col)
{
    if (val != 10 && val != 14)
        throw ExceptieCarteInvalida("valoare " + std::to_string(val) + " nu este carte punct");
}

std::unique_ptr<Carte> CartePunct::clone() const {
    return std::make_unique<CartePunct>(*this);
}

bool CartePunct::poateTaia(const Carte& alta) const {
    return valoare_ == alta.valoare();
}

std::string CartePunct::descriere() const {
    return numeValoare(valoare_) + " de " + culoare_ + " [PUNCT]";
}

// ----------- CarteSeptar -----------

CarteSeptar::CarteSeptar(const std::string& col)
    : Carte(7, col) {}

std::unique_ptr<Carte> CarteSeptar::clone() const {
    return std::make_unique<CarteSeptar>(*this);
}

bool CarteSeptar::poateTaia(const Carte& /*alta*/) const {
    // 7 taie orice — exceptie: daca prima carte e tot 7,
    // atunci referinta devine 7 si tot 7 trebuie sa tai
    // (aceasta logica e gestionata in Joc, nu aici)
    return true;
}

std::string CarteSeptar::descriere() const {
    return "7 de " + culoare_ + " [SEPTAR]";
}
