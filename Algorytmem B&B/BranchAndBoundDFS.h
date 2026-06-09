#include <iostream>
#include <climits>
#include <chrono>

using namespace std;

class BranchAndBoundDFS {
public:
    int bestCost;       // najlepszy znaleziony koszt pełnej trasy
    int* bestPath;      // najlepsza znaleziona ścieżka
    int* currentPath;   // ścieżka aktualnie budowana w rekurencji
    bool* visited;      // tablica informująca, które miasta zostały już odwiedzone

    int n;              // liczba miast w aktualnym grafie
    int start;          // miasto startowe
    int** matrix;       // wskaźnik na macierz kosztów grafu

    // Zmienne pomocnicze do kontroli czasu działania algorytmu.
    chrono::high_resolution_clock::time_point timeStart;
    double timeLimitSeconds;
    bool interrupted;
    
    // Dokładny czas działania algorytmu
    double finalExecutionTime;

    BranchAndBoundDFS() {
        // Wartości początkowe obiektu.
        bestCost = INT_MAX;
        bestPath = nullptr;
        currentPath = nullptr;
        visited = nullptr;

        n = 0;
        start = 0;
        matrix = nullptr;

        // Domyślny limit czasu działania: 300 sekund.
        timeLimitSeconds = 300.0;
        interrupted = false;
        finalExecutionTime = 0.0;
    }

    ~BranchAndBoundDFS() {
        // Przy usuwaniu obiektu zwalniana jest pamięć zaalokowana przez algorytm.
        clear();
    }

    void clear() {
        // Funkcja usuwa pamięć pomocniczą oraz resetuje pola obiektu.
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        if (currentPath != nullptr) {
            delete[] currentPath;
            currentPath = nullptr;
        }

        if (visited != nullptr) {
            delete[] visited;
            visited = nullptr;
        }

        bestCost = INT_MAX;
        n = 0;
        start = 0;
        matrix = nullptr;
        interrupted = false;
        finalExecutionTime = 0.0;
    }

    int firstMin(int city) {
        // Funkcja zwraca najtańsze możliwe wyjście z danego miasta.
        int minCost = INT_MAX;

        for (int j = 0; j < n; j++) {
            if (j != city && matrix[city][j] != -1 && matrix[city][j] < minCost) {
                minCost = matrix[city][j];
            }
        }

        return minCost;
    }

    int calculateBound(int currentCity, int currentCost) {
        // Funkcja oblicza uproszczone ograniczenie dolne dla aktualnego stanu.
        int bound = currentCost;

        // Szukanie najtańszego wyjścia z aktualnego miasta
        int minFromCurrent = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (!visited[j] && matrix[currentCity][j] != -1) {
                if (matrix[currentCity][j] < minFromCurrent) {
                    minFromCurrent = matrix[currentCity][j];
                }
            }
        }

        if (minFromCurrent != INT_MAX) {
            bound += minFromCurrent;
        }

        // Dla każdego nieodwiedzonego miasta dodaje jego najtańsze możliwe wyjście.
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                int m = firstMin(i);
                if (m != INT_MAX) {
                    bound += m;
                }
            }
        }

        return bound;
    }

    void copyCurrentToBest() {
        for (int i = 0; i < n; i++) {
            bestPath[i] = currentPath[i];
        }
        bestPath[n] = start;
    }

    void copyPathToBest(int* path) {
        for (int i = 0; i <= n; i++) {
            bestPath[i] = path[i];
        }
    }

    void dfs(int level, int currentCity, int currentCost) {
        // ===================== KONTROLA LIMITU CZASU =====================
        auto now = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(now - timeStart).count();

        if (elapsed >= timeLimitSeconds) {
            interrupted = true;
            finalExecutionTime = elapsed; // ZAPISUJĘ CZAS PRZERWANIA
            return;
        }

        // ===================== SPRAWDZENIE, CZY MAMY PEŁNĄ ŚCIEŻKĘ =====================
        if (level == n) {
            if (matrix[currentCity][start] != -1) {
                int totalCost = currentCost + matrix[currentCity][start];

                if (totalCost < bestCost) {
                    bestCost = totalCost;
                    copyCurrentToBest();
                }
            }
            return;
        }

        // ===================== OBLICZENIE OGRANICZENIA DOLNEGO =====================
        int bound = calculateBound(currentCity, currentCost);

        if (bound >= bestCost) {
            return;
        }

        // ===================== DFS - ROZWIJANIE DZIECI =====================
        for (int nextCity = 0; nextCity < n; nextCity++) {
            if (!visited[nextCity] && matrix[currentCity][nextCity] != -1) {
                
                visited[nextCity] = true;
                currentPath[level] = nextCity;

                dfs(level + 1, nextCity, currentCost + matrix[currentCity][nextCity]);

                visited[nextCity] = false;
                currentPath[level] = -1;

                if (interrupted) {
                    return;
                }
            }
        }
    }

    void solve(Graph& g, int startCity, bool useInitialSolution = false, int initialBestCost = INT_MAX, int* initialBestPath = nullptr, double limitSeconds = 300.0) {
        // Najpierw czyszczę stare dane.
        clear();

        // Ustawienie danych wejściowych z grafu.
        n = g.size;
        matrix = g.matrix;
        start = startCity;
        timeLimitSeconds = limitSeconds;
        interrupted = false;
        finalExecutionTime = 0.0;

        bestPath = new int[n + 1];
        currentPath = new int[n + 1];
        visited = new bool[n];

        for (int i = 0; i < n; i++) {
            bestPath[i] = -1;
            currentPath[i] = -1;
            visited[i] = false;
        }
        bestPath[n] = -1;
        currentPath[n] = -1;

        // ===================== ROZWIĄZANIE POCZĄTKOWE =====================
        if (useInitialSolution == true) {
            bestCost = initialBestCost;
            copyPathToBest(initialBestPath);
        }
        else {
            bestCost = INT_MAX;
        }

        visited[start] = true;
        currentPath[0] = start;

        // Zapamiętanie momentu startu algorytmu.
        timeStart = chrono::high_resolution_clock::now();

        // Uruchomienie rekurencyjnego DFS
        dfs(1, start, 0);

        // Jeśli algorytm skończył działanie bez przerwania przez czas:
        if (!interrupted) {
            auto endNow = chrono::high_resolution_clock::now();
            finalExecutionTime = chrono::duration<double>(endNow - timeStart).count();
        }
    }

    void displayResult() {
        if (bestCost == INT_MAX) {
            cout << "Nie znaleziono rozwiazania." << endl;
            return;
        }

        cout << "Najlepszy koszt: " << bestCost << endl;
        cout << "Najlepsza sciezka: ";

        for (int i = 0; i <= n; i++) {
            cout << bestPath[i];
            if (i < n) {
                cout << " -> ";
            }
        }

        cout << endl;

        if (interrupted) {
            cout << "Algorytm zostal przerwany przez limit czasu." << endl;
        }
    }
};