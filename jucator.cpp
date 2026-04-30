#include "jucator.h"
#include "exceptii.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

int Jucator::total_jucatori_ = 0;

// ==================== Jucator (baza) ====================

Jucator::Jucator(std::string nume) : nume_(std::move(nume)) {
    ++total_jucatori_;
}

Jucator::Jucator(const Jucator& other) : nume_(other.nume_) {
    for (const auto& c : other.mana_)
        mana_.push_back(c->clone());
    for (const auto& c : other.castigate_)
        castigate_.push_back(c->clone());
}

void swap(Jucator& a, Jucator& b) {
    std::swap(a.nume_,     b.nume_);
    std::swap(a.mana_,     b.mana_);
    std::swap(a.castigate_, b.castigate_);
}

// (op= nu se defineste in baza abstracta — este implementat in subclasele concrete)

void Jucator::adauga_carte(std::unique_ptr<Carte> c) {
    mana_.push_back(std::move(c));
}

void Jucator::adauga_castigate(std::unique_ptr<Carte> c) {
    castigate_.push_back(std::move(c));
}

int Jucator::puncte_castigate() const {
    int total = 0;
    for (const auto& c : castigate_)
        total += c->puncte();
    return total;
}

int Jucator::cauta_taietor(const Carte& referinta) const {
    // Prioritate: intai carti cu aceeasi valoare (non-septar), apoi septari
    // — exceptie: daca referinta e septar, doar alt septar taie
    if (referinta.esteSeptar()) {
        for (int i = 0; i < (int)mana_.size(); ++i)
            if (mana_[i]->esteSeptar()) return i;
        return -1;
    }
    // referinta normala/punct: cauta aceeasi valoare
    for (int i = 0; i < (int)mana_.size(); ++i)
        if (!mana_[i]->esteSeptar() && mana_[i]->valoare() == referinta.valoare())
            return i;
    // fallback: septar
    for (int i = 0; i < (int)mana_.size(); ++i)
        if (mana_[i]->esteSeptar()) return i;
    return -1;
}

int Jucator::cauta_cea_mai_slaba() const {
    // Prefera carte fara puncte si fara septar
    for (int i = 0; i < (int)mana_.size(); ++i)
        if (!mana_[i]->esteSeptar() && mana_[i]->puncte() == 0) return i;
    // Daca nu are, returneaza prima non-septar
    for (int i = 0; i < (int)mana_.size(); ++i)
        if (!mana_[i]->esteSeptar()) return i;
    // Ultima resursa: septar
    return 0;
}

std::ostream& operator<<(std::ostream& os, const Jucator& j) {
    os << "[" << j.nume_ << "] mana (" << j.nr_carti() << " carti): ";
    for (const auto& c : j.mana_)
        os << *c << "  |  ";
    return os;
}

// ==================== JucatorUman ====================

JucatorUman::JucatorUman(const std::string& nume) : Jucator(nume) {}

std::unique_ptr<Jucator> JucatorUman::clone() const {
    return std::make_unique<JucatorUman>(*this);
}

void JucatorUman::afiseaza_mana() const {
    std::cout << "\nMana ta:\n";
    for (int i = 0; i < (int)mana_.size(); ++i)
        std::cout << "  [" << i+1 << "] " << *mana_[i] << "\n";
}

int JucatorUman::citeste_index(int min, int max) const {
    int ales = -1;
    while (ales < min || ales > max) {
        std::cout << "Alege (" << min << "-" << max << "): ";
        if (!(std::cin >> ales)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            ales = -1;
        }
    }
    return ales;
}

std::unique_ptr<Carte> JucatorUman::alege_carte_lider() {
    if (!are_carti()) throw ExceptieManaGoala{};
    afiseaza_mana();
    std::cout << "Alege cartea cu care incepi runda:\n";
    int idx = citeste_index(1, nr_carti()) - 1;
    auto carte = std::move(mana_[idx]);
    mana_.erase(mana_.begin() + idx);
    return carte;
}

std::unique_ptr<Carte> JucatorUman::alege_carte_taiere(const Carte& referinta) {
    if (!are_carti()) throw ExceptieManaGoala{};
    afiseaza_mana();
    std::cout << "Referinta pe masa: " << referinta << "\n";
    std::cout << "Alege carte pentru a taia [0 = nu tai]: ";
    int idx = citeste_index(0, nr_carti());
    if (idx == 0) return nullptr;
    idx--;
    // Validare: cartea aleasa poate taia?
    if (!mana_[idx]->poateTaia(referinta) && !mana_[idx]->esteSeptar()) {
        // Daca referinta e septar si cartea nu e septar, invalida
        if (referinta.esteSeptar()) {
            std::cout << "Aceasta carte nu poate taia un 7! Incearca din nou.\n";
            return alege_carte_taiere(referinta);
        }
        // Daca referinta nu e septar, cartea trebuie sa aiba aceeasi valoare sau sa fie septar
        std::cout << "Aceasta carte nu poate taia! Trebuie aceeasi valoare sau un 7.\n";
        return alege_carte_taiere(referinta);
    }
    auto carte = std::move(mana_[idx]);
    mana_.erase(mana_.begin() + idx);
    return carte;
}

// ==================== JucatorAI ====================

JucatorAI::JucatorAI(const std::string& nume) : Jucator(nume) {}

std::unique_ptr<Jucator> JucatorAI::clone() const {
    return std::make_unique<JucatorAI>(*this);
}

bool JucatorAI::mana_are_punct() const {
    for (const auto& c : mana_)
        if (c->puncte() > 0) return true;
    return false;
}

int JucatorAI::cauta_momeala() const {
    // Cea mai mica carte normala neutru (8 sau 9 preferat)
    int idx = -1;
    int val_min = 999;
    for (int i = 0; i < (int)mana_.size(); ++i) {
        if (!mana_[i]->esteSeptar() && mana_[i]->puncte() == 0) {
            if (mana_[i]->valoare() < val_min) {
                val_min = mana_[i]->valoare();
                idx = i;
            }
        }
    }
    return idx;
}

std::unique_ptr<Carte> JucatorAI::alege_carte_lider() {
    if (!are_carti()) throw ExceptieManaGoala{};

    // Strategie: joaca momeala (carte slaba) ca sa atraga puncte
    int idx = cauta_momeala();
    if (idx == -1) idx = cauta_cea_mai_slaba();

    std::cout << "[AI " << nume_ << "] joaca: " << *mana_[idx] << "\n";
    auto carte = std::move(mana_[idx]);
    mana_.erase(mana_.begin() + idx);
    return carte;
}

std::unique_ptr<Carte> JucatorAI::alege_carte_taiere(const Carte& referinta) {
    if (!are_carti()) throw ExceptieManaGoala{};

    // Daca referinta e punct (10 sau A), incearca sa taie cu orice
    if (referinta.puncte() > 0) {
        int idx = cauta_taietor(referinta);
        if (idx != -1) {
            std::cout << "[AI " << nume_ << "] taie cu: " << *mana_[idx] << "\n";
            auto carte = std::move(mana_[idx]);
            mana_.erase(mana_.begin() + idx);
            return carte;
        }
        std::cout << "[AI " << nume_ << "] nu poate taia, arunca o carte.\n";
        return nullptr;
    }

    // Daca referinta e neutra, AI-ul NU foloseste septari, arunca neutru
    // — excepta ultima mana a jocului (gestionata de Joc)
    std::cout << "[AI " << nume_ << "] nu taie (carte neutra).\n";
    return nullptr;
}
