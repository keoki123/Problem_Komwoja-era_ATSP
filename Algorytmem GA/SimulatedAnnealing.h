#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <climits>
#include <iomanip>

#include "Graph.h"

using namespace std;

class SimulatedAnnealing {
public:
    // Typy chlodzenia uzywane w algorytmie.
    enum CoolingType {
        GEOMETRIC = 1,
        LINEAR = 2
    };

    int bestCost;                 // najlepszy znaleziony koszt trasy
    vector<int> bestPath;         // najlepsza znaleziona trasa bez powtorzonego miasta na koncu
    double finalExecutionTime;    // czas wykonania algorytmu w sekundach

private:
    double initialTemperature;    // temperatura poczatkowa
    double currentTemperature;    // aktualna temperatura
    double minTemperature;        // minimalna temperatura, przy ktorej koncze algorytm

    double coolingRate;           // alpha dla chlodzenia geometrycznego: T = T * alpha
    double linearCoolingValue;    // beta dla chlodzenia liniowego: T = T - beta

    int epochLength;              // liczba prob wykonywanych przy jednej temperaturze
    CoolingType coolingType;      // aktualnie wybrany schemat chlodzenia

    mt19937 generator;            // generator liczb losowych

public:
    SimulatedAnnealing() {
        // Na poczatku ustawiam bardzo duzy koszt, bo nie mam jeszcze zadnego wyniku.
        bestCost = INT_MAX;

        // Na poczatku najlepsza sciezka jest pusta.
        bestPath.clear();

        // Czas wykonania przed uruchomieniem algorytmu wynosi 0.
        finalExecutionTime = 0.0;

        // Ustawiam domyslne parametry algorytmu.
        initialTemperature = 10000.0;
        currentTemperature = initialTemperature;
        minTemperature = 0.000001;

        // Domyslne chlodzenie geometryczne.
        coolingRate = 0.995;

        // Domyslna wartosc dla chlodzenia liniowego.
        linearCoolingValue = 1.0;

        // Domyslna dlugosc epoki.
        epochLength = 100;

        // Domyslnie wybieram chlodzenie geometryczne.
        coolingType = GEOMETRIC;

        // Inicjalizuje generator liczb losowych.
        random_device rd;
        generator = mt19937(rd());
    }

    // Ustawiam temperature poczatkowa.
    void setInitialTemperature(double value) {
        if (value > 0.0) {
            initialTemperature = value;
        }
    }

    // Ustawiam wspolczynnik alpha dla chlodzenia geometrycznego.
    void setCoolingRate(double value) {
        if (value > 0.0 && value < 1.0) {
            coolingRate = value;
        }
    }

    // Ustawiam wartosc beta dla chlodzenia liniowego.
    void setLinearCoolingValue(double value) {
        if (value > 0.0) {
            linearCoolingValue = value;
        }
    }

    // Ustawiam dlugosc epoki.
    void setEpochLength(int value) {
        if (value > 0) {
            epochLength = value;
        }
    }

    // Ustawiam schemat chlodzenia.
    void setCoolingType(int value) {
        if (value == GEOMETRIC) {
            coolingType = GEOMETRIC;
        } else if (value == LINEAR) {
            coolingType = LINEAR;
        }
    }

    // Zwracam aktualna dlugosc epoki.
    int getEpochLength() {
        return epochLength;
    }

    // Licze koszt podanej trasy.
    int calculatePathCost(Graph& graph, vector<int>& path) {
        int cost = 0;
        int n = graph.size;

        // Dodaje koszty przejsc miedzy kolejnymi miastami.
        for (int i = 0; i < n - 1; i++) {
            cost += graph.matrix[path[i]][path[i + 1]];
        }

        // Na koncu dodaje koszt powrotu z ostatniego miasta do pierwszego.
        cost += graph.matrix[path[n - 1]][path[0]];

        return cost;
    }

    // Generuje sasiada aktualnej trasy metoda insert.
    vector<int> generateNeighbour(vector<int>& currentPath) {
        // Kopiuje aktualna trase, zeby nie zmieniac jej od razu.
        vector<int> neighbour = currentPath;

        int n = (int)neighbour.size();

        // Dla bardzo malych tras nie ma sensu wykonywac operacji insert.
        if (n <= 2) {
            return neighbour;
        }

        // Losuje dwie pozycje w trasie.
        // fromPosition - pozycja miasta, ktore wyjmuje.
        // toPosition - pozycja, na ktora to miasto wstawiam.
        uniform_int_distribution<int> dist(0, n - 1);

        int fromPosition = dist(generator);
        int toPosition = dist(generator);

        // Pilnuje, zeby pozycje byly rozne.
        while (fromPosition == toPosition) {
            toPosition = dist(generator);
        }

        // Zapamietuje miasto, ktore chce przeniesc.
        int city = neighbour[fromPosition];

        // Usuwam miasto ze starej pozycji.
        neighbour.erase(neighbour.begin() + fromPosition);

        // Wstawiam miasto na nowa pozycje.
        neighbour.insert(neighbour.begin() + toPosition, city);

        return neighbour;
    }

    // Losuje liczbe rzeczywista z przedzialu od 0 do 1.
    double randomDouble() {
        uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(generator);
    }

    // Sprawdzam, czy powinienem zaakceptowac nowa trase.
    bool shouldAccept(int currentCost, int neighbourCost) {
        // Obliczam roznice kosztow.
        int delta = neighbourCost - currentCost;

        // Jesli nowa trasa jest lepsza, akceptuje ja zawsze.
        if (delta < 0) {
            return true;
        }

        // Jesli nowa trasa jest gorsza, akceptuje ja tylko z pewnym prawdopodobienstwem.
        // Wzor: p = exp(-delta / T)
        double probability = exp((-1.0 * delta) / currentTemperature);

        // Losuje liczbe od 0 do 1.
        double randomValue = randomDouble();

        // Jesli wylosowana liczba jest mniejsza od prawdopodobienstwa,
        // to akceptuje gorsze rozwiazanie.
        return randomValue < probability;
    }

    // Obnizam temperature zgodnie z wybranym schematem chlodzenia.
    void coolDown() {
        if (coolingType == GEOMETRIC) {
            // Chlodzenie geometryczne: T = T * alpha
            currentTemperature *= coolingRate;
        } else if (coolingType == LINEAR) {
            // Chlodzenie liniowe: T = T - beta
            currentTemperature -= linearCoolingValue;
        }

        // Nie pozwalam, zeby temperatura spadla ponizej minimalnej wartosci.
        if (currentTemperature < minTemperature) {
            currentTemperature = minTemperature;
        }
    }

    // Sprawdzam, czy minal zadany limit czasu.
    bool isTimeExceeded(chrono::steady_clock::time_point startTime, double stopTimeSeconds) {
        auto now = chrono::steady_clock::now();

        double elapsed = chrono::duration<double>(now - startTime).count();

        return elapsed >= stopTimeSeconds;
    }

    // Obliczam, ile sekund minelo od startu algorytmu.
    double getElapsedTime(chrono::steady_clock::time_point startTime) {
        auto now = chrono::steady_clock::now();

        return chrono::duration<double>(now - startTime).count();
    }

    // Glowna funkcja algorytmu Symulowanego Wyzarzania.
    void solve(Graph& graph, vector<int>& initialPath, double stopTimeSeconds) {
        int n = graph.size;

        // Resetuje wynik przed nowym uruchomieniem algorytmu.
        bestCost = INT_MAX;
        bestPath.clear();
        finalExecutionTime = 0.0;

        // Sprawdzam, czy przekazana sciezka poczatkowa ma poprawny rozmiar.
        if ((int)initialPath.size() != n) {
            cout << "Blad: niepoprawne rozwiazanie poczatkowe." << endl;
            return;
        }

        // Jako aktualna trase ustawiam rozwiazanie poczatkowe.
        vector<int> currentPath = initialPath;

        // Obliczam koszt trasy poczatkowej.
        int currentCost = calculatePathCost(graph, currentPath);

        // Na poczatku najlepsza trasa to trasa poczatkowa.
        bestCost = currentCost;
        bestPath = currentPath;

        // Ustawiam aktualna temperature na temperature poczatkowa.
        currentTemperature = initialTemperature;

        // Zapamietuje czas startu algorytmu.
        auto startTime = chrono::steady_clock::now();

        // Algorytm dziala, dopoki nie minie czas oraz temperatura nie spadnie do minimum.
        while (!isTimeExceeded(startTime, stopTimeSeconds) && currentTemperature > minTemperature) {

            // Wykonuje jedna epoke, czyli okreslona liczbe prob przy tej samej temperaturze.
            for (int i = 0; i < epochLength; i++) {

                // Sprawdzam czas takze w srodku epoki,
                // zeby nie przekroczyc ustawionego limitu.
                if (isTimeExceeded(startTime, stopTimeSeconds)) {
                    break;
                }

                // Tworze nowa trase sasiednia metoda insert.
                vector<int> neighbourPath = generateNeighbour(currentPath);

                // Obliczam koszt nowej trasy.
                int neighbourCost = calculatePathCost(graph, neighbourPath);

                // Sprawdzam, czy nowa trase zaakceptowac.
                if (shouldAccept(currentCost, neighbourCost)) {
                    currentPath = neighbourPath;
                    currentCost = neighbourCost;
                }

                // Jesli aktualna trasa jest najlepsza do tej pory,
                // to zapisuje ja jako najlepszy wynik.
                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    bestPath = currentPath;
                }
            }

            // Po zakonczeniu epoki obnizam temperature.
            coolDown();
        }

        // Po zakonczeniu algorytmu zapisuje calkowity czas wykonania.
        finalExecutionTime = getElapsedTime(startTime);
    }

    // Wyswietlam najlepszy znaleziony wynik.
    void displayResult() {
        cout << "Najlepszy koszt SA: " << bestCost << endl;
        cout << "Najlepsza trasa SA: ";

        for (int i = 0; i < (int)bestPath.size(); i++) {
            cout << bestPath[i] << " -> ";
        }

        // Dopisuje powrot do miasta poczatkowego.
        if (!bestPath.empty()) {
            cout << bestPath[0];
        }

        cout << endl;

        cout << "Czas wykonania [s]: "
             << fixed << setprecision(3) << finalExecutionTime << endl;
    }
};

#endif