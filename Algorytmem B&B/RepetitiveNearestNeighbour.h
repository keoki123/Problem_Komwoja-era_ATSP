#include <iostream>
#include <climits>

using namespace std;

class RepetitiveNearestNeighbour {
public:
    int bestCost;        // najlepszy znaleziony koszt spośród wszystkich startów
    int* bestPath;       // najlepsza znaleziona ścieżka bez miasta startowego
    int bestStartCity;   // miasto startowe, dla którego uzyskano najlepszy wynik

    RepetitiveNearestNeighbour() {
        bestCost = INT_MAX;     // na początku ustawiam bardzo duży koszt
        bestPath = nullptr;     // brak zapisanej ścieżki
        bestStartCity = -1;     // brak wybranego miasta startowego
    }

    ~RepetitiveNearestNeighbour() {
        // zwolnienie pamięci przechowującej najlepszą ścieżkę
        if (bestPath != nullptr) {
            delete[] bestPath;
        }
    }

    void copyPath(int* from, int* to, int length) {
        // kopiowanie jednej ścieżki do drugiej
        for (int i = 0; i < length; i++) {
            to[i] = from[i];
        }
    }

    void solveFromCity(int** matrix, int n, int startCity, int currentCity, bool* visited, int* currentPath, int level, int currentCost, int& localBestCost, int* localBestPath) {

        // jeśli osiągnełem poziom n-1, to odwiedziłem już wszystkie miasta poza startowym
        if (level == n - 1) {
            // dodaje koszt powrotu do miasta startowego
            int totalCost = currentCost + matrix[currentCity][startCity];

            // jeśli otrzymana trasa jest lepsza, zapisuje ją jako najlepszą lokalnie
            if (totalCost < localBestCost) {
                localBestCost = totalCost;
                copyPath(currentPath, localBestPath, n - 1);
            }
            return;
        }

        int minCost = INT_MAX;

        // szukam najmniejszego kosztu przejścia do nieodwiedzonego miasta
        for (int j = 0; j < n; j++) {
            if (!visited[j] && matrix[currentCity][j] < minCost) {
                minCost = matrix[currentCity][j];
            }
        }

        // tablica do przechowania wszystkich miast, które mają taki sam minimalny koszt przejścia
        int* equalCities = new int[n];
        int equalCount = 0;

        // zapisuje wszystkie miasta, które mają koszt równy minCost
        for (int j = 0; j < n; j++) {
            if (!visited[j] && matrix[currentCity][j] == minCost) {
                equalCities[equalCount] = j;
                equalCount++;
            }
        }

        // dla każdego miasta o minimalnym koszcie wykonuje osobne zejście rekurencyjne
        for (int i = 0; i < equalCount; i++) {
            int nextCity = equalCities[i];

            // oznaczam wybrane miasto jako odwiedzone
            visited[nextCity] = true;

            // zapisuje je na bieżącej pozycji ścieżki
            currentPath[level] = nextCity;

            // przechodzę rekurencyjnie dalej
            solveFromCity(matrix, n, startCity, nextCity, visited, currentPath, level + 1, currentCost + matrix[currentCity][nextCity], localBestCost, localBestPath);

            // cofam oznaczenie odwiedzenia, żeby można było sprawdzić inne możliwości
            visited[nextCity] = false;
        }

        // zwalniam pamięć pomocniczej tablicy
        delete[] equalCities;
    }

    void solve(int** matrix, int n) {
        bestCost = INT_MAX;     // reset najlepszego kosztu globalnego
        bestStartCity = -1;     // reset najlepszego miasta startowego

        // jeśli wcześniej była już zapisana jakaś ścieżka, usuwam ją
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        // tablica przechowująca najlepszą ścieżkę globalną
        bestPath = new int[n - 1];

        // tablice pomocnicze
        bool* visited = new bool[n];
        int* currentPath = new int[n - 1];
        int* localBestPath = new int[n - 1];

        // uruchamiam algorytm dla każdego możliwego miasta startowego
        for (int startCity = 0; startCity < n; startCity++) {
            // na początku żadne miasto nie jest odwiedzone
            for (int i = 0; i < n; i++) {
                visited[i] = false;
            }

            // miasto startowe oznaczam jako odwiedzone
            visited[startCity] = true;

            // najlepszy koszt dla aktualnego miasta startowego
            int localBestCost = INT_MAX;

            // uruchomienie rekurencyjnego przeszukiwania
            solveFromCity(matrix, n, startCity, startCity, visited, currentPath, 0, 0, localBestCost, localBestPath);

            // jeśli wynik dla tego startu jest lepszy od globalnego, zapisuje go
            if (localBestCost < bestCost) {
                bestCost = localBestCost;
                bestStartCity = startCity;
                copyPath(localBestPath, bestPath, n - 1);
            }
        }

        // zwolnienie pamięci pomocniczej
        delete[] visited;
        delete[] currentPath;
        delete[] localBestPath;
    }

    void printResult(int n) {
        cout << "Najlepszy koszt RNN: " << bestCost
             << " (Start z miasta: " << bestStartCity << ")" << endl;

        cout << "Najlepsza trasa RNN: " << bestStartCity << " ";

        // wypisanie kolejnych miast najlepszej ścieżki
        for (int i = 0; i < n - 1; i++) {
            cout << "-> " << bestPath[i] << " ";
        }

        // powrót do miasta startowego
        cout << "-> " << bestStartCity << endl;
    }
};