#ifndef RAND_H
#define RAND_H

#include <iostream>
#include <climits>
#include <cstdlib>

using namespace std;

class Rand {
public:
    int bestCost;   // najlepszy znaleziony koszt
    int* bestPath;  // najlepsza znaleziona sciezka bez miasta startowego
    int start;      // ustalone miasto startowe

    Rand() {
        bestCost = INT_MAX;
        bestPath = nullptr;
        start = -1;
    }

    ~Rand() {
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }
    }

    int calculateCost(int** matrix, int n, int* path) {
        int cost = 0;

        cost += matrix[start][path[0]];

        for (int i = 0; i < n - 2; i++) {
            cost += matrix[path[i]][path[i + 1]];
        }

        cost += matrix[path[n - 2]][start];

        return cost;
    }

    void copyPath(int* from, int* to, int length) {
        for (int i = 0; i < length; i++) {
            to[i] = from[i];
        }
    }

    void solve(int** matrix, int n, int startCity, int iterations) {
        start = startCity;
        bestCost = INT_MAX;

        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        if (n <= 1) {
            return;
        }

        if (iterations <= 0) {
            iterations = 1;
        }

        bestPath = new int[n - 1];
        int* path = new int[n - 1];

        int index = 0;

        for (int i = 0; i < n; i++) {
            if (i != start) {
                path[index] = i;
                index++;
            }
        }

        for (int k = 0; k < iterations; k++) {
            // Fisher-Yates shuffle
            for (int i = n - 2; i > 0; i--) {
                int r = rand() % (i + 1);

                int temp = path[i];
                path[i] = path[r];
                path[r] = temp;
            }

            int cost = calculateCost(matrix, n, path);

            if (cost < bestCost) {
                bestCost = cost;
                copyPath(path, bestPath, n - 1);
            }
        }

        delete[] path;
    }

    void printResult(int n) {
        cout << "Najlepszy koszt RAND: " << bestCost << endl;
        cout << "Najlepsza trasa RAND: " << start << " ";

        for (int i = 0; i < n - 1; i++) {
            cout << "-> " << bestPath[i] << " ";
        }

        cout << "-> " << start << endl;
    }
};

#endif
