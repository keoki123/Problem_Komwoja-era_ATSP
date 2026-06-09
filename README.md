# Projektowanie Efektywnych Algorytmów

Repozytorium zawiera projekty wykonane w ramach przedmiotu **Projektowanie Efektywnych Algorytmów**.  
Wszystkie programy zostały napisane w języku **C++** i dotyczą rozwiązywania problemu komiwojażera, głównie w wersji asymetrycznej **ATSP**.

Celem projektów była implementacja oraz analiza efektywności różnych algorytmów optymalizacyjnych.

---

## Struktura repozytorium

| Folder | Zadanie | Opis |
|---|---:|---|
| `Algorytmem NN, RNN, BF oraz losowym` | Zadanie 1 | Algorytmy Brute Force, Nearest Neighbour, Repetitive Nearest Neighbour oraz algorytm losowy |
| `Algorytmem B&B` | Zadanie 2 | Algorytm Branch and Bound |
| `Algorytmem SW` | Zadanie 3 | Algorytm symulowanego wyżarzania |
| `Algorytmem GA` | Zadanie 4 | Algorytm genetyczny |

---

## Opis projektów

### Zadanie 1 — BF, NN, RNN oraz algorytm losowy

W projekcie zaimplementowano podstawowe metody rozwiązywania problemu komiwojażera:

- **Brute Force** — metoda dokładna sprawdzająca wszystkie możliwe trasy,
- **Nearest Neighbour** — heurystyka najbliższego sąsiada,
- **Repetitive Nearest Neighbour** — wielokrotne uruchomienie NN z różnych miast startowych,
- **Random Search** — losowe generowanie tras.

Projekt umożliwia porównanie czasu działania oraz jakości otrzymanych rozwiązań.

---

### Zadanie 2 — Branch and Bound

Projekt zawiera implementację algorytmu **Branch and Bound**.

Algorytm przeszukuje przestrzeń rozwiązań i odcina gałęzie, które nie mogą prowadzić do poprawy aktualnie najlepszego wyniku.  
Celem było zbadanie działania metody dokładnej dla różnych rozmiarów instancji.

---

### Zadanie 3 — Symulowane wyżarzanie

Projekt przedstawia implementację algorytmu **Simulated Annealing**.

Algorytm rozpoczyna działanie od rozwiązania początkowego, a następnie modyfikuje trasę, akceptując czasami również gorsze rozwiązania. Pozwala to uniknąć utknięcia w minimum lokalnym.

W projekcie uwzględniono między innymi:

- kryterium stopu czasowe,
- temperaturę początkową,
- długość epoki,
- różne schematy chłodzenia,
- różne sposoby tworzenia rozwiązania początkowego.

---

### Zadanie 4 — Algorytm genetyczny

Projekt zawiera implementację **algorytmu genetycznego** dla problemu komiwojażera.

Rozwiązanie reprezentowane jest jako permutacja miast. Algorytm tworzy populację początkową, a następnie generuje kolejne pokolenia za pomocą selekcji, krzyżowania i mutacji.

Zaimplementowano:

- losowe tworzenie populacji początkowej,
- selekcję turniejową,
- selekcję ruletkową,
- krzyżowanie OX,
- krzyżowanie PMX,
- mutację swap,
- mutację inversion,
- elitaryzm,
- kryterium stopu czasowe,
- zapis wyników testów do pliku CSV.

---

## Technologie

- C++
- aplikacje konsolowe
- dynamiczna alokacja struktur danych
- obsługa plików wejściowych
- zapis wyników do plików tekstowych oraz CSV

---

