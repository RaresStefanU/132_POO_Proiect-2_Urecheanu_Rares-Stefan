#include "joc.h"
#include "jucator.h"
#include "exceptii.h"
#include <iostream>
#include <memory>
#include <limits>

// dynamic_cast demo: afiseaza tipul real al jucatorului
void afiseaza_tip_jucator(const Jucator& j) {
    if (dynamic_cast<const JucatorAI*>(&j))
        std::cout << j.nume() << " este un JucatorAI.\n";
    else if (dynamic_cast<const JucatorUman*>(&j))
        std::cout << j.nume() << " este un JucatorUman.\n";
}

int main() {
    std::cout << "==============================\n";
    std::cout << "   Bine ai venit la Septica!  \n";
    std::cout << "==============================\n\n";

    std::cout << "Alege modul de joc:\n";
    std::cout << "  [1] Jucator vs AI\n";
    std::cout << "  [2] AI vs AI (demo automat)\n";
    std::cout << "Optiunea: ";

    int optiune = 0;
    while (optiune != 1 && optiune != 2) {
        if (!(std::cin >> optiune)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        std::unique_ptr<Jucator> j1, j2;

        if (optiune == 1) {
            std::string nume;
            std::cout << "Introdu numele tau: ";
            std::getline(std::cin, nume);
            if (nume.empty()) nume = "Jucator";
            j1 = std::make_unique<JucatorUman>(nume);
            j2 = std::make_unique<JucatorAI>("CPU");
        } else {
            j1 = std::make_unique<JucatorAI>("AI-1");
            j2 = std::make_unique<JucatorAI>("AI-2");
        }

        // Demo dynamic_cast (cerinta: apel prin pointer la baza)
        afiseaza_tip_jucator(*j1);
        afiseaza_tip_jucator(*j2);
        std::cout << "\n";

        // Demo clone (constructor virtual)
        {
            auto clone_j1 = j1->clone();
            std::cout << "[Clone test] " << *clone_j1 << "\n\n";
        }

        Joc joc(std::move(j1), std::move(j2));
        joc.ruleaza();

    } catch (const ExceptieStocGol& e) {
        std::cerr << "[EXCEPTIE - StocGol] " << e.what() << "\n";
    } catch (const ExceptieManaGoala& e) {
        std::cerr << "[EXCEPTIE - ManaGoala] " << e.what() << "\n";
    } catch (const ExceptieCarteInvalida& e) {
        std::cerr << "[EXCEPTIE - CarteInvalida] " << e.what() << "\n";
    } catch (const ExceptieJoc& e) {
        std::cerr << "[EXCEPTIE - Joc] " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[EXCEPTIE - std] " << e.what() << "\n";
    }

    return 0;
}
