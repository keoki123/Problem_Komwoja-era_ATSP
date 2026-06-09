#include <iostream>
#include <ctime>
#include <chrono>
#include <climits>
#include <iomanip>
#include "Graph.h"
#include "BranchAndBoundDFS.h"
#include "BranchAndBoundBFS.h"
#include "BranchAndBoundBestFirst.h"
#include "RepetitiveNearestNeighbour.h"

using namespace std;

int main() {
    // Ustawienie ziarna generatora liczb losowych.
    srand(time(NULL));

    int choice;              // numer opcji wybranej przez użytkownika z menu
    Graph g;                 // obiekt przechowujący aktualny graf
    bool GraphReady = false; // informacja, czy graf został już wczytany lub wygenerowany

    do {
        // ===================== MENU PROGRAMU =====================
        cout << "1. Wczytanie macierzy z pliku." << endl;
        cout << "2. Utworzenie randomowewgo grafu." << endl;
        cout << "3. Wyswitelnie macierzy." << endl;
        cout << "4. B&B - Depth First Search." << endl;
        cout << "5. B&B - Breadth Firts Search." << endl;   
		cout << "6. B&B - Best First Search." << endl;   
		cout << "7. Testy automatyczne." << endl;  
        cout << "0. Wyjscie." << endl;
        cout << endl;
        cout << "Wybierz opcje: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
            case 0: {
                // Wyjście z programu
                break;
            }

            case 1: {
                // ===================== WCZYTANIE GRAFU Z PLIKU =====================
                // Użytkownik podaje nazwę pliku, a program próbuje wczytać z niego macierz kosztów.
                string filename;

                cout << "Podaj nazwe pliku: ";
                cin >> filename;

                if (g.loadFromFile(filename)) {
                    GraphReady = true;
                    cout << "Plik wczytany!" << endl;
                }

                cout << endl;
                cout << endl;
                break;
            }

            case 2: {
                // ===================== GENEROWANIE LOSOWEGO GRAFU =====================
                // Program tworzy nową losową instancję ATSP o zadanej liczbie wierzchołków.
                int n;
                cout << "Podaj liczbe wierzcholkow: ";
                cin >> n;
                cout << endl;

                // Najpierw czyszczenie starego grafu, jeśli istniał,
                // a potem utworzenie nowego i wypełnienie go losowymi kosztami.
                g.clear();
                g.create(n);
                g.generateRandom();
                GraphReady = true;

                cout << endl;
                cout << endl;
                break;
            }

            case 3: {
                // ===================== WYŚWIETLENIE AKTUALNEJ MACIERZY =====================
                // Ta opcja pokazuje aktualnie wczytaną lub wygenerowaną macierz kosztów.
                if (GraphReady == true) {
                    g.display();
                } else {
                    cout << "Wygeneruj najpierw graf lub go zaladuj z pliku" << endl;
                }

                cout << endl;
                cout << endl;
                break;
            }

            case 4: {
    			// ===================== URUCHOMIENIE B&B DFS =====================
    			if (GraphReady == true) {	
    			    // Najpierw wyznaczane jest rozwiązanie początkowe metodą Nearest Neighbour.
    			    // To rozwiązanie może później posłużyć jako początkowy bestCost.
    			   RepetitiveNearestNeighbour rnn;
					rnn.solve(g.matrix, g.size);

                    // Utworzenie pełnej ścieżki w formacie wymaganym przez algorytm B&B.
                    // Ścieżka ma postać: start -> kolejne miasta -> start.
					int* initialPath = new int[g.size + 1];
					initialPath[0] = 0;

					for (int i = 0; i < g.size - 1; i++) {
   	 					initialPath[i + 1] = rnn.bestPath[i];
					}

					initialPath[g.size] = 0;
    			
        			BranchAndBoundDFS dfs;

        			dfs.solve(g, 0, true, rnn.bestCost, initialPath, 300.0); // 300 sekund = 5 minut

                    // Czas wykonania podany w milisekundach.
        			double duration = dfs.finalExecutionTime;

                    // Wyświetlenie najlepszego kosztu i ścieżki znalezionej przez DFS.
        			dfs.displayResult();
        			cout << "Czas wykonania [s]: " << fixed << setprecision(3) << duration << endl;
        			// Zwolnienie pamięci zaalokowanej na ścieżkę początkową.
       	 			delete[] initialPath;
    			} else {
       		 		cout << "Wygeneruj najpierw graf lub wczytaj go z pliku!" << endl;
    			}
    			cout << endl;
    			cout << endl;
    			break;
			}

			case 5: {
    			// ===================== URUCHOMIENIE B&B BFS =====================
    			if (GraphReady == true) {
                    // Wyznaczenie rozwiązania początkowego metodą NN.
    				RepetitiveNearestNeighbour rnn;
					rnn.solve(g.matrix, g.size);

                    // Zbudowanie pełnej ścieżki początkowej.
					int* initialPath = new int[g.size + 1];
					initialPath[0] = 0;

					for (int i = 0; i < g.size - 1; i++) {
   	 					initialPath[i + 1] = rnn.bestPath[i];
					}

					initialPath[g.size] = 0;
    				
        			BranchAndBoundBFS bfs;

                    // Pomiar czasu działania BFS.
        			bfs.solve(g, 0, true, rnn.bestCost, initialPath, 300.0);
					double duration = bfs.finalExecutionTime;

                    // Wyświetlenie wyniku końcowego BFS.
        			bfs.displayResult();
       	 			cout << "Czas wykonania [s]: " << fixed << setprecision(3) << duration << endl;

                    // Zwolnienie pamięci zaalokowanej na ścieżkę początkową.
       	 			delete[] initialPath;
    			} else {
        			cout << "Wygeneruj najpierw graf lub wczytaj go z pliku!" << endl;
    			}

    			cout << endl;
    			cout << endl;
    			break;
			}

			case 6: {
			    // ===================== URUCHOMIENIE B&B BEST-FIRST SEARCH =====================
			    if (GraphReady == true) {
                    // Najpierw obliczane jest rozwiązanie początkowe metodą NN.
			        RepetitiveNearestNeighbour rnn;
			        rnn.solve(g.matrix, g.size);
			
                    // Zbudowanie pełnej ścieżki początkowej dla algorytmu Best-First.
			        int* initialPath = new int[g.size + 1];
			        initialPath[0] = 0;
			
			        for (int i = 0; i < g.size - 1; i++) {
			            initialPath[i + 1] = rnn.bestPath[i];
			        }
			
			        initialPath[g.size] = 0;
			
			        BranchAndBoundBestFirst bestFirst;
			
                    // Pomiar czasu działania Best-First Search.
			        bestFirst.solve(g, 0, true, rnn.bestCost, initialPath, 300.0);
					double duration = bestFirst.finalExecutionTime;
			
                    // Wyświetlenie końcowego wyniku działania algorytmu.
			        bestFirst.displayResult();
			        cout << "Czas wykonania [s]: " << fixed << setprecision(3) << duration << endl;
			
                    // Zwolnienie pamięci po ścieżce początkowej.
			        delete[] initialPath;
			    }
			    else {
			        cout << "Wygeneruj najpierw graf lub wczytaj go z pliku!" << endl;
			    }
			
			    cout << endl;
			    cout << endl;
			    break;
			}

			case 7: {
			    // ===================== TESTY AUTOMATYCZNE =====================
			    // Wyniki średnie dla kolejnych wartości N będą zapisywane do pliku.
			    ofstream results("wyniki.txt");
			
			    if (!results.is_open()) {
			        cout << "Nie mozna otworzyc pliku wynikowego!" << endl;
			        cout << endl;
			        cout << endl;
			        break;
			    }
			
                // Nagłówek pliku wynikowego:
                // N, średni czas oraz procent przerwanych instancji.
			    results << "N;avg_time_s;interrupt_pct" << endl;
			
                // Pętla po kolejnych rozmiarach problemu.
			    for (int n = 4; n <= 13; n++) {
			        double sumTime = 0.0;     // suma czasów dla 100 testów
			        int interruptedCount = 0; // liczba instancji przerwanych limitem czasu
			
                    // Dla każdej wartości N generowanych jest 100 losowych instancji.
			        for (int test = 1; test <= 100; test++) {
			            Graph testGraph;
			            testGraph.create(n);
			            testGraph.generateRandom();
			
			            // Rozwiazanie poczatkowe z NN.
                        // Może ono zostać przekazane do algorytmu B&B jako początkowa górna granica.
			            RepetitiveNearestNeighbour rnn;
			            rnn.solve(testGraph.matrix, testGraph.size);
			
			            int* initialPath = nullptr;
			            if (rnn.bestCost != INT_MAX) {
			                initialPath = new int[n + 1];
			                initialPath[0] = 0;
			
			                for (int i = 0; i < n - 1; i++) {
			                    initialPath[i + 1] = rnn.bestPath[i];
			                }
			
			                initialPath[n] = 0;
			            }
			
			            // ===================== TESTOWANY ALGORYTM =====================
                        // W tej wersji testowany jest DFS.
			            BranchAndBoundDFS dfs;
			            dfs.solve(testGraph, 0, false, rnn.bestCost, initialPath, 300.0);
			
                        // Czas w sekundach.
			            double time = dfs.finalExecutionTime;
			            sumTime += time;
			
                        // Jeśli algorytm został przerwany przez limit czasu,
                        // zwiększam licznik przerwanych instancji.
			            if (dfs.interrupted) {
			                interruptedCount++;
			            }
			
                        // Zwolnienie pamięci zaalokowanej na ścieżkę początkową.
			            if (initialPath != nullptr) {
			                delete[] initialPath;
			            }
			
			            cout << "N = " << n << ", test = " << test << " zakonczony." << endl;
			        }
			
                    // Obliczenie średniego czasu oraz procentu przerwanych instancji.
			        double avgTime = sumTime / 100;
			        double interruptPct = (interruptedCount / 100) * 100.0;
			
                    // Zapis uśrednionych wyników do pliku.
			        results << n << ";"
			                << fixed << setprecision(6) << avgTime << ";"
			                << fixed << setprecision(2) << interruptPct
			                << endl;
			
			        cout << "Dla N = " << n << " sredni czas = " << avgTime
			             << " s, procent przerwan = " << interruptPct << "%" << endl;
			    }
			
			    results.close();
			
			    cout << "Testy zakonczone. Wyniki zapisano do pliku wyniki.txt" << endl;
			    cout << endl;
			    cout << endl;
			    break;
			}

            default: {
                // ===================== BŁĘDNA OPCJA =====================
                // Jeśli użytkownik poda numer spoza dostępnego zakresu,
                // program wypisuje komunikat o błędzie.
                cout << "ERROR: nie poprawna opcja!" << endl;
                cout << endl;
                cout << endl;
                break;
            }
        }

    } while (choice != 0);  
    return 0;
}