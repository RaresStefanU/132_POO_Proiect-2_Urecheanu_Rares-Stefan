# Șeptică — Proiect POO (C++17)

Implementare în consolă a jocului de cărți **Șeptică** pentru 2 jucători, realizat ca proiect pentru materia Programare Orientată pe Obiecte (FMI).

---

## Compilare și rulare

```bash
make        # compilează
make run    # compilează și pornește jocul
make clean  # șterge fișierele obiect și executabilul
```

Cerințe: `g++` cu suport C++17.

---

## Moduri de joc

- **Jucător vs AI** — tu joci în consolă, AI-ul decide automat
- **AI vs AI** — demo automat, util pentru testare

---

## Structura proiectului

```
septica/
├── include/
│   ├── exceptii.h      # ierarhie proprie de excepții
│   ├── carte.h         # Carte (abstractă), CarteNormala, CartePunct, CarteSeptar
│   ├── pachet.h        # Pachet (32 cărți, amestecare, tragere)
│   ├── jucator.h       # Jucator (abstractă), JucatorUman, JucatorAI
│   └── joc.h           # Joc — orchestrează o partidă completă
├── src/
│   ├── carte.cpp
│   ├── pachet.cpp
│   ├── jucator.cpp
│   ├── joc.cpp
│   └── main.cpp
└── Makefile
```

---

## Cerințe POO bifate

### Separarea codului în header + sursă
Fiecare clasă are propriul fișier `.h` în `include/` și `.cpp` în `src/`. Niciun header nu conține `using namespace std` la nivel global.

### Moșteniri și funcții virtuale pure

**Ierarhia `Carte`:**
```
Carte (abstractă)
├── CarteNormala   — 8, 9, J, Q, K (0 puncte)
├── CartePunct     — 10, A          (1 punct fiecare)
└── CarteSeptar    — 7              (taie orice)
```

**Ierarhia `Jucator`:**
```
Jucator (abstractă)
├── JucatorUman    — citește decizia de la stdin
└── JucatorAI      — decide automat după strategie
```

Funcții virtuale pure: `clone()`, `alege_carte_lider()`, `alege_carte_taiere()`, `puncte()`, `esteSeptar()`, `poateTaia()`, `descriere()`.

Pointerii la bază (`unique_ptr<Carte>`, `unique_ptr<Jucator>`) sunt **atribute ale altor clase** (`Pachet`, `Joc`), nu doar variabile locale în `main`.

### Constructori virtuali (clone)
Fiecare subclasă concretă implementează `clone()` care returnează `unique_ptr` la tipul corect. Folosit în copy constructor-ul lui `Jucator` pentru a duplica corect cărțile din mână.

### `dynamic_cast`
În `main.cpp`, funcția `afiseaza_tip_jucator()` primește `const Jucator&` și folosește `dynamic_cast` pentru a determina tipul real (`JucatorAI` sau `JucatorUman`).

### Copy constructor + operator= cu copy-and-swap
- `Jucator` — copy constructor manual (clonează fiecare `unique_ptr` din `mana_` și `castigate_`)
- `JucatorUman` și `JucatorAI` — `operator=(T other)` cu `swap` (copy-and-swap complet în subclasele concrete, deoarece `Jucator` este abstractă și nu poate fi instanțiată direct)
- `Pachet` — non-copiabil (`delete`), semantic corect (nu are sens să copiezi un pachet în joc)

### Excepții — ierarhie proprie

```
std::exception
└── ExceptieJoc
    ├── ExceptieStocGol       — tras din pachet gol
    ├── ExceptieManaGoala     — jucator fără cărți
    └── ExceptieCarteInvalida — valoare nepotrivită pentru tipul cartei
```

`throw` în: constructorii `CarteNormala`/`CartePunct` (valoare greșită), `Pachet::trage()`, `Jucator::alege_carte_*()`.  
`try/catch` complet în `main`, cu câte un `catch` separat pentru fiecare tip din ierarhie.

### Funcții și atribute statice

| Clasă      | Atribut static         | Getter                     |
|------------|------------------------|----------------------------|
| `Carte`    | `total_instante_`      | `Carte::totalInstante()`   |
| `Pachet`   | `total_pachete_`       | `Pachet::totalPachete()`   |
| `Jucator`  | `total_jucatori_`      | `Jucator::totalJucatori()` |
| `Joc`      | `total_jocuri_`        | `Joc::totalJocuri()`       |
| `Joc`      | `MARIME_MANA = 4`      | constante de joc           |
| `Joc`      | `PUNCTE_VICTORIE = 5`  | constante de joc           |

---

## Regulile jocului implementate

- Pachet de 32 cărți (7–A, 4 culori), amestecat aleator
- Fiecare jucător primește 4 cărți inițial
- **Referința rundei** = prima carte jucată; poate fi tăiată doar cu aceeași valoare sau cu un 7
- **Excepție:** dacă runda începe cu un 7, poate fi tăiată doar cu alt 7
- Ultimul care taie câștigă toate cărțile de pe masă
- După fiecare mână, jucătorii completează mâna la 4 cărți (câștigătorul trage primul)
- Câștigă cine acumulează ≥ 5 puncte (fiecare 10 și A valorează 1 punct; total 8 puncte)

### Strategia AI
1. Ca **lider**: joacă o carte neutră mică (momeală) pentru a atrage puncte de la adversar
2. Ca **secundar**:
   - Dacă referința este 10 sau A → încearcă să taie cu orice carte posibilă
   - Dacă referința este neutră → nu taie, conservă resursele (septarii în special)