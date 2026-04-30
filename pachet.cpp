#include "pachet.h"
#include "exceptii.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <ostream>

int Pachet::total_pachete_ = 0;

// Valorile interne: 7=7, 8=8, 9=9, 10=10, J=11, Q=12, K=13, A=14
static const std::vector<int> VALORI = {7, 8, 9, 10, 11, 12, 13, 14};
static const std::vector<std::string> CULORI = {"Inima", "Caro", "Trefla", "Pica"};

Pachet::Pachet() {
    ++total_pachete_;
    for (const auto& culoare : CULORI) {
        for (int val : VALORI) {
            if (val == 7)
                carti_.push_back(std::make_unique<CarteSeptar>(culoare));
            else if (val == 10 || val == 14)
                carti_.push_back(std::make_unique<CartePunct>(val, culoare));
            else
                carti_.push_back(std::make_unique<CarteNormala>(val, culoare));
        }
    }
}

void Pachet::amesteca() {
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::shuffle(carti_.begin(), carti_.end(), std::default_random_engine(
        static_cast<std::default_random_engine::result_type>(seed)));
}

std::unique_ptr<Carte> Pachet::trage() {
    if (esteGol()) throw ExceptieStocGol{};
    auto c = std::move(carti_.back());
    carti_.pop_back();
    return c;
}

std::ostream& operator<<(std::ostream& os, const Pachet& p) {
    os << "Pachet (" << p.marime() << " carti):\n";
    for (const auto& c : p.carti_)
        os << "  " << *c << "\n";
    return os;
}
