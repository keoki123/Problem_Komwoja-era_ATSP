#include <iostream>
#include <climits>
#include <algorithm>

using namespace std;

class BruteForce {
public:
    int bestCost;   // najlepszy znaleziony koszt
    int* bestPath;  // najlepsza znaleziona ścieżka bez miasta startowego
    int start;      // ustalone miasto startowe

    BruteForce() {
        bestCost = INT_MAX;  // na początku ustawiany jest bardzo duży koszt
        bestPath = nullptr;  // brak zapisanej ścieżki
        start = -1;          // brak ustawionego startu
    }

    ~BruteForce() {
        // zwolnienie pamięci przechowującej najlepszą ścieżkę
        if (bestPath != nullptr) {
            delete[] bestPath;
        }
    }

    int calculateCost(int** matrix, int n, int* path) {
        int cost = 0;

        // przejście ze startu do pierwszego miasta w ścieżce
        cost += matrix[start][path[0]];

        // przejścia pomiędzy kolejnymi miastami w ścieżce
        for (int i = 0; i < n - 2; i++) {
            cost += matrix[path[i]][path[i + 1]];
        }

        // powrót z ostatniego miasta do miasta startowego
        cost += matrix[path[n - 2]][start];

        return cost;
    }

    void copyPath(int* from, int* to, int length) {
        // kopiowanie jednej ścieżki do drugiej
        for (int i = 0; i < length; i++) {
            to[i] = from[i];
        }
    }

    void solve(int** matrix, int n, int startCity) {
        start = startCity;     // zapamiętanie miasta startowego
        bestCost = INT_MAX;    // reset najlepszego kosztu

        // jeśli wcześniej była już jakaś zapisana ścieżka
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        // bestPath i path mają długość n-1, ponieważ nie przechowuje w nich miasta startowego
        bestPath = new int[n - 1];
        int* path = new int[n - 1];

        int index = 0;

        // wpisanie do path wszystkich miast poza startem
        for (int i = 0; i < n; i++) {
            if (i != start) {
                path[index] = i;
                index++;
            }
        }

        // policzenie kosztu pierwszej permutacji
        int cost = calculateCost(matrix, n, path);
        bestCost = cost;
        copyPath(path, bestPath, n - 1);

        // generowanie kolejnych permutacji miast poza startem, ponieważ start jest ustalony, algorytm sprawdza (n-1)! tras
        while (next_permutation(path, path + (n - 1))) {
            cost = calculateCost(matrix, n, path);

            // jeśli znaleziono lepszą trasę, zapisuje wynik
            if (cost < bestCost) {
                bestCost = cost;
                copyPath(path, bestPath, n - 1);
            }
        }

        // zwolnienie pamięci pomocniczej
        delete[] path;
    }

    void printResult(int n) {
        cout << "Najlepszy koszt BF: " << bestCost << endl;
        cout << "Najlepsza trasa BF: " << start << " ";

        // wypisanie kolejnych miast zapisanych w najlepszej ścieżce
        for (int i = 0; i < n - 1; i++) {
            cout << "-> " << bestPath[i] << " ";
        }

        // powrót do miasta startowego
        cout << "-> " << start << endl;
    }
};