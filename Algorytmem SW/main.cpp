#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include <iomanip>
#include <string>

#include "Graph.h"
#include "RepetitiveNearestNeighbour.h"
#include "Rand.h"
#include "SimulatedAnnealing.h"

using namespace std;

// Oznaczenia metod tworzenia rozwiazania poczatkowego.
int INITIAL_RNN = 1;
int INITIAL_RAND = 2;
int INITIAL_NATURAL = 3;

// Wyswietlenie trasy razem z powrotem do miasta poczatkowego.
void printPath(vector<int>& path) {
    if (path.empty()) {
        cout << "Brak sciezki." << endl;
        return;
    }

    for (int i = 0; i < (int)path.size(); i++) {
        cout << path[i] << " -> ";
    }

    cout << path[0] << endl;
}

// Obliczenie kosztu trasy na podstawie macierzy kosztow.
int calculatePathCost(Graph& graph, vector<int>& path) {
    int cost = 0;
    int n = graph.size;

    for (int i = 0; i < n - 1; i++) {
        cost += graph.matrix[path[i]][path[i + 1]];
    }

    cost += graph.matrix[path[n - 1]][path[0]];

    return cost;
}

// Utworzenie prostej trasy 0 -> 1 -> 2 -> ...
vector<int> createNaturalPath(int n) {
    vector<int> path;

    for (int i = 0; i < n; i++) {
        path.push_back(i);
    }

    return path;
}

// Zamiana wyniku RNN na wektor trasy.
vector<int> createPathFromRNN(RepetitiveNearestNeighbour& rnn, int n) {
    vector<int> path;

    path.push_back(rnn.bestStartCity);

    for (int i = 0; i < n - 1; i++) {
        path.push_back(rnn.bestPath[i]);
    }

    return path;
}

// Zamiana wyniku RAND na wektor trasy.
vector<int> createPathFromRand(Rand& randAlgorithm, int n) {
    vector<int> path;

    path.push_back(randAlgorithm.start);

    for (int i = 0; i < n - 1; i++) {
        path.push_back(randAlgorithm.bestPath[i]);
    }

    return path;
}

// Obliczenie rozwiazania poczatkowego wybrana metoda.
vector<int> calculateInitialPath(Graph& graph, int method) {
    if (method == INITIAL_RNN) {
        RepetitiveNearestNeighbour rnn;
        rnn.solve(graph.matrix, graph.size);

        return createPathFromRNN(rnn, graph.size);
    }

    if (method == INITIAL_RAND) {
        Rand randAlgorithm;
        int startCity;
        int iterations;

        cout << "Podaj miasto startowe dla RAND: ";
        cin >> startCity;

        if (startCity < 0 || startCity >= graph.size) {
            cout << "Niepoprawne miasto startowe. Ustawiono 0." << endl;
            startCity = 0;
        }

        cout << "Podaj liczbe losowych permutacji, np. 1 albo 100: ";
        cin >> iterations;

        if (iterations <= 0) {
            iterations = 1;
        }

        randAlgorithm.solve(graph.matrix, graph.size, startCity, iterations);

        return createPathFromRand(randAlgorithm, graph.size);
    }

    return createNaturalPath(graph.size);
}

int main() {
    srand(time(NULL));

    Graph graph;
    bool graphReady = false;

    vector<int> initialPath;
    bool initialPathReady = false;
    int initialPathCost = INT_MAX;

    // Domyslne parametry algorytmu.
    double stopTimeSeconds = 900.0;
    double initialTemperature = 10000.0;
    double coolingRate = 0.995;
    double linearCoolingValue = 1.0;

    int epochMultiplier = 10;
    int coolingType = SimulatedAnnealing::GEOMETRIC;
    int initialMethod = INITIAL_RNN;

    int choice;

    do {
        cout << "==================== MENU ====================" << endl;
        cout << "1. Wczytanie danych z pliku" << endl;
        cout << "2. Wprowadzenie kryterium stopu (czas dzialania algorytmu)" << endl;
        cout << "3. Obliczanie rozwiazania poczatkowego" << endl;
        cout << "4. Modyfikacja domyslnych ustawien algorytmu" << endl;
        cout << "5. Uruchomienie algorytmu, wyswietlenie dlugosci drogi i samej drogi" << endl;
        cout << "6. Wyswietlenie macierzy" << endl;
        cout << "7. Testy automatyczne dla 10 instancji" << endl;
        cout << "0. Wyjscie" << endl;
        cout << "==============================================" << endl;
        cout << "Wybierz opcje: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
            case 0: {
                break;
            }

            case 1: {
                string filename;

                cout << "Podaj nazwe pliku .atsp albo pliku z macierza: ";
                cin >> filename;

                if (graph.loadFromFile(filename)) {
                    graphReady = true;

                    // Po wczytaniu nowego grafu trzeba policzyc start od nowa.
                    initialPath.clear();
                    initialPathReady = false;
                    initialPathCost = INT_MAX;

                    cout << "Plik wczytany poprawnie." << endl;
                    cout << "Liczba miast: " << graph.size << endl;
                } else {
                    cout << "Nie udalo sie wczytac pliku." << endl;
                }

                cout << endl;
                break;
            }

            case 2: {
                cout << "Aktualny czas dzialania: " << stopTimeSeconds << " s" << endl;
                cout << "Podaj nowy czas dzialania algorytmu [s]: ";
                cin >> stopTimeSeconds;

                if (stopTimeSeconds <= 0.0) {
                    stopTimeSeconds = 60.0;
                    cout << "Niepoprawna wartosc. Ustawiono 60 s." << endl;
                }

                cout << "Ustawiono czas stopu: " << stopTimeSeconds << " s" << endl;
                cout << endl;
                break;
            }

            case 3: {
                if (!graphReady) {
                    cout << "Najpierw wczytaj dane z pliku." << endl;
                    cout << endl;
                    break;
                }

                cout << "Wybierz metode rozwiazania poczatkowego:" << endl;
                cout << "1. RNN - Repetitive Nearest Neighbour" << endl;
                cout << "2. RAND - losowa trasa" << endl;
                cout << "3. Naturalna trasa 0 -> 1 -> 2 -> ..." << endl;
                cout << "Wybor: ";
                cin >> initialMethod;

                if (initialMethod < 1 || initialMethod > 3) {
                    cout << "Niepoprawny wybor. Ustawiono RNN." << endl;
                    initialMethod = INITIAL_RNN;
                }

                initialPath = calculateInitialPath(graph, initialMethod);
                initialPathCost = calculatePathCost(graph, initialPath);
                initialPathReady = true;

                cout << "Rozwiazanie poczatkowe obliczone." << endl;
                cout << "Dlugosc drogi poczatkowej: " << initialPathCost << endl;
                cout << "Droga poczatkowa: ";
                printPath(initialPath);
                cout << endl;

                break;
            }

            case 4: {
                int option;

                do {
                    cout << "========== USTAWIENIA ALGORYTMU ==========" << endl;
                    cout << "1. Temperatura poczatkowa: " << initialTemperature << endl;
                    cout << "2. Chlodzenie geometryczne alpha: " << coolingRate << endl;
                    cout << "3. Chlodzenie liniowe beta: " << linearCoolingValue << endl;
                    cout << "4. Dlugosc epoki: " << epochMultiplier << " * n" << endl;
                    cout << "5. Schemat chlodzenia: ";

                    if (coolingType == SimulatedAnnealing::GEOMETRIC) {
                        cout << "geometryczny" << endl;
                    } else {
                        cout << "liniowy" << endl;
                    }

                    cout << "6. Domyslna metoda startowa: ";

                    if (initialMethod == INITIAL_RNN) {
                        cout << "RNN" << endl;
                    } else if (initialMethod == INITIAL_RAND) {
                        cout << "RAND" << endl;
                    } else {
                        cout << "naturalna" << endl;
                    }

                    cout << "0. Powrot" << endl;
                    cout << "==========================================" << endl;
                    cout << "Wybierz opcje: ";
                    cin >> option;
                    cout << endl;

                    switch (option) {
                        case 1: {
                            cout << "Podaj temperature poczatkowa, np. 10000: ";
                            cin >> initialTemperature;

                            if (initialTemperature <= 0.0) {
                                initialTemperature = 10000.0;
                                cout << "Niepoprawna wartosc. Ustawiono 10000." << endl;
                            }

                            break;
                        }

                        case 2: {
                            cout << "Podaj alpha, np. 0.995: ";
                            cin >> coolingRate;

                            if (coolingRate <= 0.0 || coolingRate >= 1.0) {
                                coolingRate = 0.995;
                                cout << "Niepoprawna wartosc. Ustawiono 0.995." << endl;
                            }

                            break;
                        }

                        case 3: {
                            cout << "Podaj beta, np. 1.0: ";
                            cin >> linearCoolingValue;

                            if (linearCoolingValue <= 0.0) {
                                linearCoolingValue = 1.0;
                                cout << "Niepoprawna wartosc. Ustawiono 1.0." << endl;
                            }

                            break;
                        }

                        case 4: {
                            cout << "Podaj mnoznik dlugosci epoki, np. 1, 5, 10, 50: ";
                            cin >> epochMultiplier;

                            if (epochMultiplier <= 0) {
                                epochMultiplier = 10;
                                cout << "Niepoprawna wartosc. Ustawiono 10." << endl;
                            }

                            break;
                        }

                        case 5: {
                            cout << "Wybierz schemat chlodzenia:" << endl;
                            cout << "1. Geometryczny: T = T * alpha" << endl;
                            cout << "2. Liniowy: T = T - beta" << endl;
                            cout << "Wybor: ";
                            cin >> coolingType;

                            if (coolingType != SimulatedAnnealing::GEOMETRIC &&
                                coolingType != SimulatedAnnealing::LINEAR) {
                                coolingType = SimulatedAnnealing::GEOMETRIC;
                                cout << "Niepoprawny wybor. Ustawiono geometryczny." << endl;
                            }

                            break;
                        }

                        case 6: {
                            cout << "Wybierz domyslna metode startowa:" << endl;
                            cout << "1. RNN" << endl;
                            cout << "2. RAND" << endl;
                            cout << "3. Naturalna" << endl;
                            cout << "Wybor: ";
                            cin >> initialMethod;

                            if (initialMethod < 1 || initialMethod > 3) {
                                initialMethod = INITIAL_RNN;
                                cout << "Niepoprawny wybor. Ustawiono RNN." << endl;
                            }

                            // Po zmianie metody startowej start trzeba policzyc od nowa.
                            initialPath.clear();
                            initialPathReady = false;
                            initialPathCost = INT_MAX;

                            break;
                        }

                        case 0: {
                            break;
                        }

                        default: {
                            cout << "Niepoprawna opcja." << endl;
                            break;
                        }
                    }

                    cout << endl;
                } while (option != 0);

                break;
            }

            case 5: {
                if (!graphReady) {
                    cout << "Najpierw wczytaj dane z pliku." << endl;
                    cout << endl;
                    break;
                }

                if (!initialPathReady) {
                    cout << "Rozwiazanie poczatkowe nie bylo jeszcze obliczone." << endl;
                    cout << "Program obliczy je automatycznie wybrana metoda." << endl;

                    initialPath = calculateInitialPath(graph, initialMethod);
                    initialPathCost = calculatePathCost(graph, initialPath);
                    initialPathReady = true;

                    cout << "Dlugosc drogi poczatkowej: " << initialPathCost << endl;
                }

                int epochLength = epochMultiplier * graph.size;

                SimulatedAnnealing sa;

                // Przekazanie ustawien do algorytmu wyzarzania.
                sa.setInitialTemperature(initialTemperature);
                sa.setCoolingRate(coolingRate);
                sa.setLinearCoolingValue(linearCoolingValue);
                sa.setEpochLength(epochLength);
                sa.setCoolingType(coolingType);

                cout << "Uruchamiam Symulowane Wyzarzanie..." << endl;
                cout << "Czas stopu: " << stopTimeSeconds << " s" << endl;
                cout << "Temperatura poczatkowa: " << initialTemperature << endl;
                cout << "Dlugosc epoki: " << epochLength << endl;

                if (coolingType == SimulatedAnnealing::GEOMETRIC) {
                    cout << "Schemat chlodzenia: geometryczny, alpha = " << coolingRate << endl;
                } else {
                    cout << "Schemat chlodzenia: liniowy, beta = " << linearCoolingValue << endl;
                }

                cout << endl;

                sa.solve(graph, initialPath, stopTimeSeconds);

                cout << "Wynik algorytmu:" << endl;
                sa.displayResult();

                cout << endl;
                break;
            }

            case 6: {
                if (!graphReady) {
                    cout << "Najpierw wczytaj dane z pliku." << endl;
                    cout << endl;
                    break;
                }

                cout << "Macierz kosztow:" << endl;
                graph.display();

                cout << endl;
                break;
            }
            
            case 7: {
                int repetitions = 100;

                string files[10] = {
                    "ftv33.atsp",
                    "ftv35.atsp",
                    "ftv38.atsp",
                    "ftv44.atsp",
                    "ftv47.atsp",
                    "ftv55.atsp",
                    "ftv64.atsp",
                    "ftv70.atsp",
                    "kro124p.atsp",
                    "ftv170.atsp"
                };

                int opt[10] = {
                    1286,      // wpisz OPT dla ftv33
                    1473,      // wpisz OPT dla ftv35
                    1530,      // wpisz OPT dla ftv38
                    1613,      // wpisz OPT dla ftv44
                    1776,      // wpisz OPT dla ftv47
                    1608,      // wpisz OPT dla ftv55
                    1839,      // wpisz OPT dla ftv64
                    1950,      // wpisz OPT dla ftv70
                    36230,   // OPT dla kro123p
                    2755       // wpisz OPT dla rbg323
                };

                ofstream results("wyniki_testow.csv");

                if (!results.is_open()) {
                    cout << "Nie mozna otworzyc pliku wynikowego!" << endl;
                    cout << endl;
                    break;
                }

                results << "plik;n;opt;powtorzenia;sredni_koszt;sredni_blad_pct;sredni_czas_s;najlepszy_koszt;najlepszy_blad_pct" << endl;

                for (int i = 0; i < 10; i++) {
                    Graph testGraph;

                    cout << "==========================================" << endl;
                    cout << "Testowana instancja: " << files[i] << endl;
                    cout << "==========================================" << endl;

                    if (!testGraph.loadFromFile(files[i])) {
                        cout << "Nie udalo sie wczytac pliku: " << files[i] << endl;
                        cout << endl;
                        continue;
                    }

                    if (opt[i] <= 0) {
                        cout << "Brak poprawnej wartosci OPT dla pliku: " << files[i] << endl;
                        cout << "Uzupelnij tablice opt[] w kodzie." << endl;
                        cout << endl;
                        continue;
                    }

                    double sumCost = 0.0;
                    double sumError = 0.0;
                    double sumTime = 0.0;

                    int bestTestCost = INT_MAX;

                    for (int test = 1; test <= repetitions; test++) {
                        vector<int> startPath;

                        // Obliczam rozwiazanie poczatkowe bez dodatkowego pytania uzytkownika.
                        if (initialMethod == INITIAL_RNN) {
                            RepetitiveNearestNeighbour rnn;
                            rnn.solve(testGraph.matrix, testGraph.size);
                            startPath = createPathFromRNN(rnn, testGraph.size);
                        } else if (initialMethod == INITIAL_RAND) {
                            Rand randAlgorithm;
                            randAlgorithm.solve(testGraph.matrix, testGraph.size, 0, 100);
                            startPath = createPathFromRand(randAlgorithm, testGraph.size);
                        } else {
                            startPath = createNaturalPath(testGraph.size);
                        }

                        int epochLength = epochMultiplier * testGraph.size;

                        SimulatedAnnealing sa;

                        sa.setInitialTemperature(initialTemperature);
                        sa.setCoolingRate(coolingRate);
                        sa.setLinearCoolingValue(linearCoolingValue);
                        sa.setEpochLength(epochLength);
                        sa.setCoolingType(coolingType);

                        sa.solve(testGraph, startPath, stopTimeSeconds);

                        int resultCost = sa.bestCost;
                        double error = ((double)(resultCost - opt[i]) / opt[i]) * 100.0;

                        sumCost += resultCost;
                        sumError += error;
                        sumTime += sa.finalExecutionTime;

                        if (resultCost < bestTestCost) {
                            bestTestCost = resultCost;
                        }

                        cout << "Test " << test << "/" << repetitions
                             << " | koszt: " << resultCost
                             << " | blad: " << fixed << setprecision(2) << error << "%"
                             << " | czas: " << fixed << setprecision(3) << sa.finalExecutionTime << " s"
                             << endl;
                    }

                    double averageCost = sumCost / repetitions;
                    double averageError = sumError / repetitions;
                    double averageTime = sumTime / repetitions;
                    double bestError = ((double)(bestTestCost - opt[i]) / opt[i]) * 100.0;

                    results << files[i] << ";"
                            << testGraph.size << ";"
                            << opt[i] << ";"
                            << repetitions << ";"
                            << fixed << setprecision(2) << averageCost << ";"
                            << fixed << setprecision(2) << averageError << ";"
                            << fixed << setprecision(3) << averageTime << ";"
                            << bestTestCost << ";"
                            << fixed << setprecision(2) << bestError
                            << endl;

                    cout << endl;
                    cout << "Podsumowanie dla " << files[i] << ":" << endl;
                    cout << "Sredni koszt: " << fixed << setprecision(2) << averageCost << endl;
                    cout << "Sredni blad: " << fixed << setprecision(2) << averageError << "%" << endl;
                    cout << "Sredni czas: " << fixed << setprecision(3) << averageTime << " s" << endl;
                    cout << "Najlepszy koszt: " << bestTestCost << endl;
                    cout << "Najlepszy blad: " << fixed << setprecision(2) << bestError << "%" << endl;
                    cout << endl;
                }

                results.close();

                cout << "Testy zakonczone." << endl;
                cout << "Wyniki zapisano do pliku wyniki_testow.csv" << endl;
                cout << endl;

                break;
            }

            default: {
                cout << "Niepoprawna opcja." << endl;
                cout << endl;
                break;
            }
        }

    } while (choice != 0);

    return 0;
}