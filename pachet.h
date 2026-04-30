#pragma once
#include "carte.h"
#include <vector>
#include <memory>

class Pachet {
public:
    Pachet();   // construieste un pachet complet de 32 de carti
    void amesteca();
    std::unique_ptr<Carte> trage();   // scoate si returneaza prima carte
    bool esteGol() const { return carti_.empty(); }
    int marime()   const { return static_cast<int>(carti_.size()); }

    // Numar total de pachete create vreodata (static)
    static int totalPachete() { return total_pachete_; }

    friend std::ostream& operator<<(std::ostream& os, const Pachet& p);

    // Non-copyable (nu are sens sa copiezi un pachet in joc)
    Pachet(const Pachet&) = delete;
    Pachet& operator=(const Pachet&) = delete;
    Pachet(Pachet&&) = default;
    Pachet& operator=(Pachet&&) = default;

private:
    std::vector<std::unique_ptr<Carte>> carti_;
    static int total_pachete_;
};
