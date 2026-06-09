#include <iostream>
#include <ctime>
#include <chrono>
#include <climits>
#include <iomanip>
#include "Graph.h"
#include "BruteForce.h"
#include "NearestNeighbour.h"
#include "RepetitiveNearestNeighbour.h"
#include "Rand.h"

using namespace std;

int main() {
    // ustawienie ziarna generatora liczb losowych, dzięki temu przy każdym uruchomieniu programu losowania będą inne
    srand(time(NULL));

    int choice;       // wybór opcji z menu
    int StartCity;    // miasto startowe podawane przez użytkownika
    Graph g;          // obiekt przechowujący graf
    bool GraphReady = false;  // informacja, czy graf został już wczytany lub wygenerowany

    do {
        // wyświetlenie menu programu
        cout << "1. Wczytanie macierzy z pliku." << endl;
        cout << "2. Utworzenie randomowewgo grafu." << endl;
        cout << "3. Wyswitelnie macierzy." << endl;
        cout << "4. Metoda Brute Force." << endl;
        cout << "5. Metoda Nearest Neighbour." << endl;
        cout << "6. Metoda Repetitive Nearest Neighbour." << endl;
        cout << "7. Metoda RAND" << endl;
        cout << "8. Testy automatyczne." << endl;
        cout << "0. Wyjscie." << endl;
        cout << endl;
        cout << "Wybierz opcje: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
            case 0: {
                // zakończenie programu
                break;
            }

            case 1: {
                // wczytanie grafu z pliku tekstowego
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
                // wygenerowanie losowego grafu o zadanej liczbie wierzchołków
                int n;
                cout << "Podaj liczbe wierzcholkow: ";
                cin >> n;
                cout << endl;

                g.clear();
                g.create(n);
                g.generateRandom();
                GraphReady = true;

                cout << endl;
                cout << endl;
                break;
            }

            case 3: {
                // wyświetlenie macierzy kosztów
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
                // uruchomienie algorytmu Brute Force
                if (GraphReady == true) {
                    BruteForce bf;
                    bf.solve(g.matrix, g.size, 0);
                    bf.printResult(g.size);
                } else {
                    cout << "Wygeneruj najpierw graf lub go zaladuj z pliku" << endl;
                }

                cout << endl;
                cout << endl;
                break;
            }

            case 5: {
                // uruchomienie algorytmu Nearest Neighbour
                if (GraphReady == true) {
                    NearestNeighbour nn;

                    cout << "Podaj miasto startowe: ";
                    cin >> StartCity;
                    cout << endl;

                    nn.solve(g.matrix, g.size, StartCity);
                    nn.printResult(g.size);
                } else {
                    cout << "Wygeneruj najpierw graf lub go zaladuj z pliku" << endl;
                }

                cout << endl;
                cout << endl;
                break;
            }

            case 6: {
                // uruchomienie algorytmu Repetitive Nearest Neighbour
                if (GraphReady == true) {
                    RepetitiveNearestNeighbour rnn;

                    rnn.solve(g.matrix, g.size);
                    rnn.printResult(g.size);
                } else {
                    cout << "Wygeneruj najpierw graf lub go zaladuj z pliku" << endl;
                }

                cout << endl;
                cout << endl;
                break;
            }

            case 7: {
                // uruchomienie algorytmu losowego RAND
                if (GraphReady == true) {
                    Rand r;
                    int iterations;

                    cout << "Podaj miasto startowe: ";
                    cin >> StartCity;

                    cout << "Podaj liczbe permutacji: ";
                    cin >> iterations;
                    cout << endl;

                    r.solve(g.matrix, g.size, StartCity, iterations);
                    r.printResult(g.size);
                } else {
                    cout << "Wygeneruj najpierw graf lub go zaladuj z pliku" << endl;
                }

                cout << endl;
                cout << endl;
                break;
            }

            case 8: {
                // uruchomienie testów automatycznych wyniki zostaną zapisane do pliku tekstowego
                ofstream results("wyniki.txt");

                if (!results.is_open()) {
                    cout << "Nie mozna otworzyc pliku wynikowego!" << endl;
                    cout << endl;
                    cout << endl;
                    break;
                }

                // ustawienie formatu zapisu liczb zmiennoprzecinkowych
                results << fixed << setprecision(9);

                // nagłówek pliku wynikowego
                results << "N;test;BF_cost;BF_time;NN_cost;RNN_cost;RAND_cost" << endl;

                // pętla po rozmiarach problemu
                for (int n = 10; n <= 14; n++) {

                    // dla każdego rozmiaru wykonujemy 100 testów
                    for (int test = 1; test <= 100; test++) {
                        Graph testGraph;
                        testGraph.create(n);
                        testGraph.generateRandom();

                        BruteForce bf;
                        NearestNeighbour nn;
                        RepetitiveNearestNeighbour rnn;
                        Rand r;

                        // pomiar czasu działania Brute Force
                        auto startBF = chrono::high_resolution_clock::now();
                        bf.solve(testGraph.matrix, testGraph.size, 0);
                        auto stopBF = chrono::high_resolution_clock::now();
                        chrono::duration<double> timeBF = stopBF - startBF;

                        // uruchomienie pozostałych algorytmów
                        nn.solve(testGraph.matrix, testGraph.size, 0);
                        rnn.solve(testGraph.matrix, testGraph.size);
                        r.solve(testGraph.matrix, testGraph.size, 0, 10 * n);

                        // zapis wyników do pliku
                        results << n << ";"
                                << test << ";"
                                << bf.bestCost << ";" << timeBF.count() << ";"
                                << nn.bestCost << ";"
                                << rnn.bestCost << ";"
                                << r.bestCost << ";"
                                << endl;

                        // informacja w konsoli o zakończeniu pojedynczego testu
                        cout << "Zakonczono test " << test << " dla N = " << n << endl;
                    }
                }

                results.close();

                cout << "Testy zakonczone. Wyniki zapisano do pliku." << endl;
                cout << endl;
                cout << endl;
                break;
            }

            default: {
                // obsługa błędnego wyboru z menu
                cout << "ERROR: nie poprawna opcja!" << endl;
                cout << endl;
                cout << endl;
                break;
            }
        }

    } while (choice != 0);  // program działa dopóki użytkownik nie wybierze 0

    return 0;
}