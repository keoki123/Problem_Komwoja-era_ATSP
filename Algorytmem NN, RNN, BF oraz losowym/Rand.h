#include <iostream>
#include <climits>
#include <cstdlib> 

using namespace std;

class Rand {
public:
    int bestCost;   // najlepszy znaleziony koszt
    int* bestPath;  // najlepsza znaleziona ścieżka bez miasta startowego
    int start;      // ustalone miasto startowe

    Rand() {
        bestCost = INT_MAX;  // na początku ustawiam bardzo duży koszt
        bestPath = nullptr;  // brak zapisanej ścieżki
        start = -1;          // brak ustawionego startu
    }

    ~Rand() {
        // zwolnienie pamięci przechowującej najlepszą ścieżkę
        if (bestPath != nullptr) {
            delete[] bestPath;
        }
    }

    int calculateCost(int** matrix, int n, int* path) {
        int cost = 0;

        // przejście ze startu do pierwszego miasta w ścieżce
        cost += matrix[start][path[0]];

        // przejścia pomiędzy kolejnymi miastami
        for (int i = 0; i < n - 2; i++) {
            cost += matrix[path[i]][path[i + 1]];
        }

        // powrót z ostatniego miasta do startu
        cost += matrix[path[n - 2]][start];

        return cost;
    }

    void copyPath(int* from, int* to, int length) {
        // kopiowanie jednej ścieżki do drugiej
        for (int i = 0; i < length; i++) {
            to[i] = from[i];
        }
    }

    void solve(int** matrix, int n, int startCity, int iterations) {
        start = startCity;   // ustawienie miasta startowego
        bestCost = INT_MAX;  // reset najlepszego kosztu

        // jeśli wcześniej była już jakaś ścieżka, usuwam ją
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        // bestPath i path mają długość n-1, bo miasto startowe przechowuje osobno
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

        // wykonuje zadaną liczbę losowych prób
        for (int k = 0; k < iterations; k++) {

            // losowe mieszanie tablicy path
            // dzięki temu otrzymuje losową kolejność odwiedzenia miast
            for (int i = n - 2; i > 0; i--) {
                int r = rand() % (i + 1);

                int temp = path[i];
                path[i] = path[r];
                path[r] = temp;
            }

            // obliczenie kosztu aktualnie wylosowanej trasy
            int cost = calculateCost(matrix, n, path);

            // jeśli wylosowana trasa jest lepsza od dotychczasowej najlepsze zapisuje jej koszt i przebieg
            if (cost < bestCost) {
                bestCost = cost;
                copyPath(path, bestPath, n - 1);
            }
        }

        // zwolnienie pamięci pomocniczej
        delete[] path;
    }

    void printResult(int n) {
        cout << "Najlepszy koszt RAND: " << bestCost << endl;
        cout << "Najlepsza trasa RAND: " << start << " ";

        // wypisanie kolejnych miast najlepszej znalezionej trasy
        for (int i = 0; i < n - 1; i++) {
            cout << "-> " << bestPath[i] << " ";
        }

        // powrót do miasta startowego
        cout << "-> " << start << endl;
    }
};