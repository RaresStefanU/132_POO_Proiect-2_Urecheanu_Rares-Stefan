#pragma once
#include <exception>
#include <string>

class ExceptieJoc : public std::exception {
    std::string mesaj;
public:
    explicit ExceptieJoc(std::string msg) : mesaj(std::move(msg)) {}
    const char* what() const noexcept override { return mesaj.c_str(); }
};

class ExceptieStocGol : public ExceptieJoc {
public:
    ExceptieStocGol() : ExceptieJoc("Stocul de carti este gol!") {}
};

class ExceptieManaGoala : public ExceptieJoc {
public:
    ExceptieManaGoala() : ExceptieJoc("Jucatorul nu are carti in mana!") {}
};

class ExceptieCarteInvalida : public ExceptieJoc {
public:
    explicit ExceptieCarteInvalida(const std::string& detaliu)
        : ExceptieJoc("Carte invalida: " + detaliu) {}
};
