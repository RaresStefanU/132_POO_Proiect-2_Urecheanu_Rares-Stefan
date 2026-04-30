#pragma once
#include <string>
#include <memory>
#include <ostream>

// -------------------------------------------------------
// Ierarhie de carti:
//   Carte (abstracta)
//     |- CarteNormala  (8, 9, J, Q, K)
//     |- CartePunct    (10, A)   — valoreaza 1 punct
//     |- CarteSeptar   (7)       — taie orice
// -------------------------------------------------------

class Carte {
public:
    // Constructor virtual (clonare polimorfica)
    virtual std::unique_ptr<Carte> clone() const = 0;
    virtual ~Carte() = default;

    // Interfata virtuala
    virtual int    puncte()       const = 0;   // 0 sau 1
    virtual bool   esteSeptar()   const = 0;   // true doar pentru 7
    virtual bool   poateTaia(const Carte& alta) const = 0;
    virtual std::string descriere() const = 0;

    // Getteri comuni
    int         valoare() const { return valoare_; }
    std::string culoare() const { return culoare_; }

    // Numar total de instante create (atribut static)
    static int totalInstante() { return total_instante_; }

    friend std::ostream& operator<<(std::ostream& os, const Carte& c);

protected:
    Carte(int val, std::string col);
    Carte(const Carte&) = default;
    Carte& operator=(const Carte&) = default;
    Carte(Carte&&) = default;
    Carte& operator=(Carte&&) = default;

    int         valoare_;
    std::string culoare_;

private:
    static int total_instante_;
};

// ------ CarteNormala ------
class CarteNormala : public Carte {
public:
    CarteNormala(int val, const std::string& col);
    std::unique_ptr<Carte> clone() const override;

    int    puncte()     const override { return 0; }
    bool   esteSeptar() const override { return false; }
    bool   poateTaia(const Carte& alta) const override;
    std::string descriere() const override;
};

// ------ CartePunct (10 si A) ------
class CartePunct : public Carte {
public:
    CartePunct(int val, const std::string& col);
    std::unique_ptr<Carte> clone() const override;

    int    puncte()     const override { return 1; }
    bool   esteSeptar() const override { return false; }
    bool   poateTaia(const Carte& alta) const override;
    std::string descriere() const override;
};

// ------ CarteSeptar (7) ------
class CarteSeptar : public Carte {
public:
    CarteSeptar(const std::string& col);
    std::unique_ptr<Carte> clone() const override;

    int    puncte()     const override { return 0; }
    bool   esteSeptar() const override { return true; }
    bool   poateTaia(const Carte& alta) const override;
    std::string descriere() const override;
};
