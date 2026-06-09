#include <iostream>
#include <iomanip>
#include <string>

#include "Graph.h"
#include "GeneticAlgorithm.h"

using namespace std;

void displayCrossoverMethod(int method) {
    if (method == GeneticAlgorithm::OX) {
        cout << "OX";
    } else if (method == GeneticAlgorithm::PMX) {
        cout << "PMX";
    } else {
        cout << "nieznana";
    }
}

void displayMutationMethod(int method) {
    if (method == GeneticAlgorithm::SWAP) {
        cout << "swap";
    } else if (method == GeneticAlgorithm::INVERSION) {
        cout << "inversion";
    } else {
        cout << "nieznana";
    }
}

void displaySelectionMethod(int method) {
    if (method == GeneticAlgorithm::TOURNAMENT) {
        cout << "turniejowa";
    } else if (method == GeneticAlgorithm::ROULETTE) {
        cout << "ruletkowa";
    } else {
        cout << "nieznana";
    }
}

int main() {
    Graph graph;
    GeneticAlgorithm ga;

    bool graphReady = false;
    double stopTimeSeconds = 120.0;

    int choice;

    do {
        cout << "==================== MENU ====================" << endl;
        cout << "1. Wczytanie danych z pliku" << endl;
        cout << "2. Wyswietlenie macierzy" << endl;
        cout << "3. Wprowadzenie kryterium stopu czasowego" << endl;
        cout << "4. Ustawienie wielkosci populacji poczatkowej" << endl;
        cout << "5. Ustawienie wspolczynnika mutacji" << endl;
        cout << "6. Ustawienie wspolczynnika krzyzowania" << endl;
        cout << "7. Wybor metody krzyzowania" << endl;
        cout << "8. Wybor metody mutacji" << endl;
        cout << "9. Wybor metody selekcji" << endl;
        cout << "10. Uruchomienie algorytmu genetycznego" << endl;
        cout << "11. Testy automatyczne algorytmu genetycznego dla 10 instancji" << endl;
        cout << "0. Wyjscie" << endl;
        cout << "==============================================" << endl;

        cout << "Aktualne ustawienia:" << endl;
        cout << "Czas stopu: " << stopTimeSeconds << " s" << endl;
        cout << "Populacja: " << ga.getPopulationSize() << endl;
        cout << "Mutacja: " << ga.getMutationRate() << endl;
        cout << "Krzyzowanie: " << ga.getCrossoverRate() << endl;

        cout << "Metoda krzyzowania: ";
        displayCrossoverMethod(ga.getCrossoverMethod());
        cout << endl;

        cout << "Metoda mutacji: ";
        displayMutationMethod(ga.getMutationMethod());
        cout << endl;

        cout << "Metoda selekcji: ";
        displaySelectionMethod(ga.getSelectionMethod());
        cout << endl;

        if (graphReady) {
            cout << "Graf: wczytany, liczba miast = " << graph.size << endl;
        } else {
            cout << "Graf: brak wczytanych danych" << endl;
        }

        cout << "==============================================" << endl;
        cout << "Wybierz opcje: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
            case 0: {
                cout << "Koniec programu." << endl;
                break;
            }

            case 1: {
                string filename;

                cout << "Podaj nazwe pliku: ";
                cin >> filename;

                if (graph.loadFromFile(filename)) {
                    graphReady = true;
                    cout << "Plik wczytany poprawnie." << endl;
                    cout << "Liczba miast: " << graph.size << endl;
                } else {
                    graphReady = false;
                    cout << "Nie udalo sie wczytac pliku." << endl;
                }

                cout << endl;
                break;
            }

            case 2: {
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

            case 3: {
                cout << "Podaj czas dzialania algorytmu w sekundach: ";
                cin >> stopTimeSeconds;

                if (stopTimeSeconds <= 0.0 || stopTimeSeconds > 900.0) {
                    stopTimeSeconds = 120.0;
                    cout << "Niepoprawna wartosc. Ustawiono 120 sekund." << endl;
                } else {
                    cout << "Ustawiono czas stopu: " << stopTimeSeconds << " s" << endl;
                }

                cout << endl;
                break;
            }

            case 4: {
                int populationSize;

                cout << "Podaj wielkosc populacji, np. 500: ";
                cin >> populationSize;

                if (populationSize < 10 || populationSize > 5000) {
                    cout << "Niepoprawna wartosc. Populacja powinna byc z zakresu 10 - 5000." << endl;
                } else {
                    ga.setPopulationSize(populationSize);
                    cout << "Ustawiono wielkosc populacji: " << ga.getPopulationSize() << endl;
                }

                cout << endl;
                break;
            }

            case 5: {
                double mutationRate;

                cout << "Podaj wspolczynnik mutacji z zakresu 0..1, np. 0.10: ";
                cin >> mutationRate;

                if (mutationRate < 0.0 || mutationRate > 1.0) {
                    cout << "Niepoprawna wartosc." << endl;
                } else {
                    ga.setMutationRate(mutationRate);
                    cout << "Ustawiono wspolczynnik mutacji: " << ga.getMutationRate() << endl;
                }

                cout << endl;
                break;
            }

            case 6: {
                double crossoverRate;

                cout << "Podaj wspolczynnik krzyzowania z zakresu 0..1, np. 0.90: ";
                cin >> crossoverRate;

                if (crossoverRate < 0.0 || crossoverRate > 1.0) {
                    cout << "Niepoprawna wartosc." << endl;
                } else {
                    ga.setCrossoverRate(crossoverRate);
                    cout << "Ustawiono wspolczynnik krzyzowania: " << ga.getCrossoverRate() << endl;
                }

                cout << endl;
                break;
            }

            case 7: {
                int method;

                cout << "Wybierz metode krzyzowania:" << endl;
                cout << "1. OX - Order Crossover" << endl;
                cout << "2. PMX - Partially Mapped Crossover" << endl;
                cout << "Wybor: ";
                cin >> method;

                if (method != GeneticAlgorithm::OX && method != GeneticAlgorithm::PMX) {
                    cout << "Niepoprawny wybor." << endl;
                } else {
                    ga.setCrossoverMethod(method);

                    cout << "Ustawiono metode krzyzowania: ";
                    displayCrossoverMethod(ga.getCrossoverMethod());
                    cout << endl;
                }

                cout << endl;
                break;
            }

            case 8: {
                int method;

                cout << "Wybierz metode mutacji:" << endl;
                cout << "1. Swap - zamiana dwoch miast" << endl;
                cout << "2. Inversion - odwrocenie fragmentu trasy" << endl;
                cout << "Wybor: ";
                cin >> method;

                if (method != GeneticAlgorithm::SWAP && method != GeneticAlgorithm::INVERSION) {
                    cout << "Niepoprawny wybor." << endl;
                } else {
                    ga.setMutationMethod(method);

                    cout << "Ustawiono metode mutacji: ";
                    displayMutationMethod(ga.getMutationMethod());
                    cout << endl;
                }

                cout << endl;
                break;
            }

            case 9: {
                int method;

                cout << "Wybierz metode selekcji:" << endl;
                cout << "1. Turniejowa" << endl;
                cout << "2. Ruletkowa" << endl;
                cout << "Wybor: ";
                cin >> method;

                if (method != GeneticAlgorithm::TOURNAMENT && method != GeneticAlgorithm::ROULETTE) {
                    cout << "Niepoprawny wybor." << endl;
                } else {
                    ga.setSelectionMethod(method);

                    cout << "Ustawiono metode selekcji: ";
                    displaySelectionMethod(ga.getSelectionMethod());
                    cout << endl;
                }

                cout << endl;
                break;
            }

            case 10: {
                if (!graphReady) {
                    cout << "Najpierw wczytaj dane z pliku." << endl;
                    cout << endl;
                    break;
                }

                cout << "Uruchamiam algorytm genetyczny..." << endl;
                cout << "Czas stopu: " << stopTimeSeconds << " s" << endl;
                cout << "Rozmiar populacji: " << ga.getPopulationSize() << endl;
                cout << "Wspolczynnik mutacji: " << ga.getMutationRate() << endl;
                cout << "Wspolczynnik krzyzowania: " << ga.getCrossoverRate() << endl;

                cout << "Metoda krzyzowania: ";
                displayCrossoverMethod(ga.getCrossoverMethod());
                cout << endl;

                cout << "Metoda mutacji: ";
                displayMutationMethod(ga.getMutationMethod());
                cout << endl;

                cout << "Metoda selekcji: ";
                displaySelectionMethod(ga.getSelectionMethod());
                cout << endl;

                cout << endl;

                ga.solve(graph, stopTimeSeconds);

                cout << "============== WYNIK ==============" << endl;
                cout << "Najlepszy koszt: " << ga.bestCost << endl;

                cout << "Najlepsza trasa: ";
                ga.displayPath(ga.bestPath);

                cout << "Sredni koszt ostatniej populacji: "
                     << fixed << setprecision(2) << ga.finalAverageCost << endl;

                cout << "Liczba wykonanych pokolen: " << ga.generationsCount << endl;

                cout << "Czas wykonania [s]: "
                     << fixed << setprecision(3) << ga.finalExecutionTime << endl;

                cout << "Czas znalezienia najlepszego wyniku [s]: "
                     << fixed << setprecision(3) << ga.bestFoundTime << endl;

                cout << "===================================" << endl;
                cout << endl;

                break;
            }
			
			case 11: {
		    int repetitions = 1;
		
		    // Jedno uruchomienie jednej instancji trwa 60 sekund.
		    // Czyli jedna instancja: 10 powtorzen * 60 s = okolo 10 minut.
		    double testStopTimeSeconds = 900;
		
		    // Stale parametry bazowe algorytmu genetycznego.
		    // W pozniejszych badaniach bedziemy zmieniac tylko jedna rzecz,
		    // np. populacje, metode mutacji, krzyzowania albo selekcji.
		    int testPopulationSize = 1000;
		    double testMutationRate = 0.10;
		    double testCrossoverRate = 0.90;
		
		    int testCrossoverMethod = GeneticAlgorithm::PMX;
		    int testMutationMethod = GeneticAlgorithm::INVERSION;
		    int testSelectionMethod = GeneticAlgorithm::TOURNAMENT;
		
		    string files[10] = {
		        "ftv170.atsp"
		    };
		
		    int opt[10] = {
		        2755
		    };
		
		    ofstream results("wyniki_ga_test_rozmiar.csv");
		
		    if (!results.is_open()) {
		        cout << "Nie mozna otworzyc pliku wynikowego!" << endl;
		        cout << endl;
		        break;
		    }
		
		    results << "plik;n;opt;powtorzenia;czas_stopu_s;"
		            << "populacja;wsp_mutacji;wsp_krzyzowania;metoda_krzyzowania;metoda_mutacji;metoda_selekcji;"
		            << "sredni_najlepszy_koszt;sredni_blad_najlepszego_pct;"
		            << "sredni_koszt_ostatniej_populacji;sredni_blad_ostatniej_populacji_pct;"
		            << "najlepszy_koszt;najlepszy_blad_pct;"
		            << "sredni_czas_znalezienia_najlepszego_s;sredni_czas_wykonania_s;srednia_liczba_pokolen"
		            << endl;
		
		    for (int i = 0; i < 1; i++) {
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
		
		        double sumBestCost = 0.0;
		        double sumBestError = 0.0;
		
		        double sumAveragePopulationCost = 0.0;
		        double sumAveragePopulationError = 0.0;
		
		        double sumBestFoundTime = 0.0;
		        double sumExecutionTime = 0.0;
		        double sumGenerations = 0.0;
		
		        int bestTestCost = INT_MAX;
		
		        for (int test = 1; test <= repetitions; test++) {
		            GeneticAlgorithm ga;
		
		            ga.setPopulationSize(testPopulationSize);
		            ga.setMutationRate(testMutationRate);
		            ga.setCrossoverRate(testCrossoverRate);
		            ga.setCrossoverMethod(testCrossoverMethod);
		            ga.setMutationMethod(testMutationMethod);
		            ga.setSelectionMethod(testSelectionMethod);
		
		            ga.solve(testGraph, testStopTimeSeconds);
		
		            int resultCost = ga.bestCost;
		            double resultError = ((double)(resultCost - opt[i]) / opt[i]) * 100.0;
		
		            double averagePopulationCost = ga.finalAverageCost;
		            double averagePopulationError = ((averagePopulationCost - opt[i]) / opt[i]) * 100.0;
		
		            sumBestCost += resultCost;
		            sumBestError += resultError;
		
		            sumAveragePopulationCost += averagePopulationCost;
		            sumAveragePopulationError += averagePopulationError;
		
		            sumBestFoundTime += ga.bestFoundTime;
		            sumExecutionTime += ga.finalExecutionTime;
		            sumGenerations += ga.generationsCount;
		
		            if (resultCost < bestTestCost) {
		                bestTestCost = resultCost;
		            }
		
		            cout << "Test " << test << "/" << repetitions
		                 << " | koszt: " << resultCost
		                 << " | blad: " << fixed << setprecision(2) << resultError << "%"
		                 << " | sredni koszt populacji: " << fixed << setprecision(2) << averagePopulationCost
		                 << " | czas najlepszego: " << fixed << setprecision(3) << ga.bestFoundTime << " s"
		                 << " | czas: " << fixed << setprecision(3) << ga.finalExecutionTime << " s"
		                 << " | pokolenia: " << ga.generationsCount
		                 << endl;
		        }
		
		        double averageBestCost = sumBestCost / repetitions;
		        double averageBestError = sumBestError / repetitions;
		
		        double averagePopulationCost = sumAveragePopulationCost / repetitions;
		        double averagePopulationError = sumAveragePopulationError / repetitions;
		
		        double averageBestFoundTime = sumBestFoundTime / repetitions;
		        double averageExecutionTime = sumExecutionTime / repetitions;
		        double averageGenerations = sumGenerations / repetitions;
		
		        double bestError = ((double)(bestTestCost - opt[i]) / opt[i]) * 100.0;
		
		        string crossoverName = "OX";
		        if (testCrossoverMethod == GeneticAlgorithm::PMX) {
		            crossoverName = "PMX";
		        }
		
		        string mutationName = "SWAP";
		        if (testMutationMethod == GeneticAlgorithm::INVERSION) {
		            mutationName = "INVERSION";
		        }
		
		        string selectionName = "TOURNAMENT";
		        if (testSelectionMethod == GeneticAlgorithm::ROULETTE) {
		            selectionName = "ROULETTE";
		        }
		
		        results << files[i] << ";"
                << testGraph.size << ";"
                << opt[i] << ";"
                << repetitions << ";"
                << fixed << setprecision(3) << testStopTimeSeconds << ";"
                << testPopulationSize << ";"
                << fixed << setprecision(2) << testMutationRate << ";"
                << fixed << setprecision(2) << testCrossoverRate << ";"
                << crossoverName << ";"
                << mutationName << ";"
                << selectionName << ";"
                << fixed << setprecision(2) << averageBestCost << ";"
                << fixed << setprecision(2) << averageBestError << ";"
                << fixed << setprecision(2) << averagePopulationCost << ";"
                << fixed << setprecision(2) << averagePopulationError << ";"
                << bestTestCost << ";"
                << fixed << setprecision(2) << bestError << ";"
                << fixed << setprecision(3) << averageBestFoundTime << ";"
                << fixed << setprecision(3) << averageExecutionTime << ";"
                << fixed << setprecision(2) << averageGenerations
                << endl;

		        cout << endl;
		        cout << "Podsumowanie dla " << files[i] << ":" << endl;
		        cout << "Sredni najlepszy koszt: " << fixed << setprecision(2) << averageBestCost << endl;
		        cout << "Sredni blad najlepszego: " << fixed << setprecision(2) << averageBestError << "%" << endl;
		        cout << "Sredni koszt ostatniej populacji: " << fixed << setprecision(2) << averagePopulationCost << endl;
		        cout << "Sredni blad ostatniej populacji: " << fixed << setprecision(2) << averagePopulationError << "%" << endl;
		        cout << "Najlepszy koszt z powtorzen: " << bestTestCost << endl;
		        cout << "Najlepszy blad: " << fixed << setprecision(2) << bestError << "%" << endl;
		        cout << "Sredni czas znalezienia najlepszego: "
		             << fixed << setprecision(3) << averageBestFoundTime << " s" << endl;
		        cout << "Sredni czas wykonania: "
		             << fixed << setprecision(3) << averageExecutionTime << " s" << endl;
		        cout << "Srednia liczba pokolen: "
		             << fixed << setprecision(2) << averageGenerations << endl;
		        cout << endl;
		    }
		
		    results.close();
		
		    cout << "Testy zakonczone." << endl;
		    cout << "Wyniki zapisano do pliku wyniki_ga_test_rozmiar.csv" << endl;
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