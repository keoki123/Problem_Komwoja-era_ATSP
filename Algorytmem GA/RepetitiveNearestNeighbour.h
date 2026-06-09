#ifndef REPETITIVE_NEAREST_NEIGHBOUR_H
#define REPETITIVE_NEAREST_NEIGHBOUR_H

#include <iostream>
#include <climits>

using namespace std;

class RepetitiveNearestNeighbour {
public:
    int bestCost;        // najlepszy znaleziony koszt sposrod wszystkich startow
    int* bestPath;       // najlepsza znaleziona sciezka bez miasta startowego
    int bestStartCity;   // miasto startowe, dla ktorego uzyskano najlepszy wynik

    RepetitiveNearestNeighbour() {
        bestCost = INT_MAX;
        bestPath = nullptr;
        bestStartCity = -1;
    }

    ~RepetitiveNearestNeighbour() {
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }
    }

    void copyPath(int* from, int* to, int length) {
        for (int i = 0; i < length; i++) {
            to[i] = from[i];
        }
    }

    void solveFromCity(int** matrix, int n, int startCity, int currentCity,
                       bool* visited, int* currentPath, int level,
                       int currentCost, int& localBestCost, int* localBestPath) {
        if (level == n - 1) {
            int totalCost = currentCost + matrix[currentCity][startCity];

            if (totalCost < localBestCost) {
                localBestCost = totalCost;
                copyPath(currentPath, localBestPath, n - 1);
            }

            return;
        }

        int minCost = INT_MAX;

        for (int j = 0; j < n; j++) {
            if (!visited[j] && matrix[currentCity][j] < minCost) {
                minCost = matrix[currentCity][j];
            }
        }

        int* equalCities = new int[n];
        int equalCount = 0;

        for (int j = 0; j < n; j++) {
            if (!visited[j] && matrix[currentCity][j] == minCost) {
                equalCities[equalCount] = j;
                equalCount++;
            }
        }

        for (int i = 0; i < equalCount; i++) {
            int nextCity = equalCities[i];

            visited[nextCity] = true;
            currentPath[level] = nextCity;

            solveFromCity(matrix, n, startCity, nextCity, visited, currentPath,
                          level + 1, currentCost + matrix[currentCity][nextCity],
                          localBestCost, localBestPath);

            visited[nextCity] = false;
        }

        delete[] equalCities;
    }

    void solve(int** matrix, int n) {
        bestCost = INT_MAX;
        bestStartCity = -1;

        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        if (n <= 1) {
            return;
        }

        bestPath = new int[n - 1];

        bool* visited = new bool[n];
        int* currentPath = new int[n - 1];
        int* localBestPath = new int[n - 1];

        for (int startCity = 0; startCity < n; startCity++) {
            for (int i = 0; i < n; i++) {
                visited[i] = false;
            }

            visited[startCity] = true;

            int localBestCost = INT_MAX;

            solveFromCity(matrix, n, startCity, startCity, visited, currentPath,
                          0, 0, localBestCost, localBestPath);

            if (localBestCost < bestCost) {
                bestCost = localBestCost;
                bestStartCity = startCity;
                copyPath(localBestPath, bestPath, n - 1);
            }
        }

        delete[] visited;
        delete[] currentPath;
        delete[] localBestPath;
    }

    void printResult(int n) {
        cout << "Najlepszy koszt RNN: " << bestCost
             << " (start z miasta: " << bestStartCity << ")" << endl;

        cout << "Najlepsza trasa RNN: " << bestStartCity << " ";

        for (int i = 0; i < n - 1; i++) {
            cout << "-> " << bestPath[i] << " ";
        }

        cout << "-> " << bestStartCity << endl;
    }
};

#endif
