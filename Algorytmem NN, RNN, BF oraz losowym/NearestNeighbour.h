#include <iostream>
#include <climits>

using namespace std;

class NearestNeighbour {
public:
    int bestCost;   // koszt znalezionej trasy
    int* bestPath;  // zapisana kolejność odwiedzanych miast bez miasta startowego
    int start;      // ustalone miasto startowe

    NearestNeighbour() {
        bestCost = INT_MAX;  // początkowo bardzo duży koszt
        bestPath = nullptr;  // brak zapisanej ścieżki
        start = -1;          // brak ustawionego startu
    }

    ~NearestNeighbour() {
        // zwolnienie pamięci przechowującej ścieżkę
        if (bestPath != nullptr) {
            delete[] bestPath;
        }
    }

    void solve(int** matrix, int n, int startCity) {
        start = startCity;   // ustawienie miasta startowego
        bestCost = INT_MAX;  // reset kosztu

        // jeśli wcześniej była już jakaś ścieżka, usuwam ją
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        // bestPath przechowuje n-1 miast, bo start zapisuje osobno
        bestPath = new int[n - 1];
        bool* visited = new bool[n];

        // na początku żadne miasto nie jest odwiedzone
        for (int i = 0; i < n; i++) {
            visited[i] = false;
        }

        int current = start;      // aktualnie jestem w mieście startowym
        visited[start] = true;    // oznaczam start jako odwiedzony
        int cost = 0;             // koszt budowanej trasy

        // wykonujemy n-1 kroków, bo muszę odwiedzić wszystkie miasta poza startem
        for (int i = 0; i < n - 1; i++) {
            int nextCity = -1;        // najbliższe jeszcze nieodwiedzone miasto
            int minCost = INT_MAX;    // najmniejszy znaleziony koszt przejścia

            // szukanie najbliższego nieodwiedzonego miasta
            for (int j = 0; j < n; j++) {
                if (!visited[j] && matrix[current][j] < minCost) {
                    minCost = matrix[current][j];
                    nextCity = j;
                }
            }

            // zapisanie wybranego miasta do ścieżki
            bestPath[i] = nextCity;

            // oznaczenie tego miasta jako odwiedzonego
            visited[nextCity] = true;

            // dodanie kosztu przejścia do całkowitego kosztu trasy
            cost += minCost;

            // przejście do nowego aktualnego miasta
            current = nextCity;
        }

        // po odwiedzeniu wszystkich miast wracamy do startu
        cost += matrix[current][start];

        // zapis końcowego kosztu trasy
        bestCost = cost;

        // zwolnienie pamięci pomocniczej
        delete[] visited;
    }

    void printResult(int n) {
        cout << "Najlepszy koszt: " << bestCost << endl;
        cout << "Najlepsza trasa: " << start << " ";

        // wypisanie kolejnych miast trasy
        for (int i = 0; i < n - 1; i++) {
            cout << "-> " << bestPath[i] << " ";
        }

        // powrót do miasta startowego
        cout << "-> " << start << endl;
    }
};