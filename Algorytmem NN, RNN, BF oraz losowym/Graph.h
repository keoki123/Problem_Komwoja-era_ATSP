#include <iostream>
#include <climits>
#include <cstdlib> 
#include <fstream>

using namespace std;

class Graph {
public:
    int size;      // liczba miast 
    int** matrix;  // dynamicznie alokowana macierz kosztów

    Graph() {
        size = 0;         // początkowo graf jest pusty
        matrix = nullptr; // brak zaalokowanej macierzy
    }

    ~Graph() {
        // przy niszczeniu obiektu zwalniam pamięć
        clear();
    }

    void create(int n) {
        // jeśli wcześniej istniała już jakaś macierz, usuwam ją
        clear();

        size = n;

        // alokacja pamięci na wiersze macierzy
        matrix = new int*[size];

        // alokacja pamięci na każdą kolumnę w każdym wierszu
        for (int i = 0; i < size; i++) {
            matrix[i] = new int[size];
        }
    }

    void generateRandom() {
        // generowanie losowej macierzy kosztów
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    // na przekątnej wpisuje -1, bo przejście z miasta do samego siebie nie istnieje
                    matrix[i][j] = -1;
                } else {
                    // losowy koszt z zakresu 1..100
                    matrix[i][j] = rand() % 100 + 1;
                }
            }
        }
    }

    void display() {
        // wyświetlenie całej macierzy na ekranie
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    void clear() {
        // jeśli macierz istnieje, zwalniam całą pamięć
        if (matrix != nullptr) {
            for (int i = 0; i < size; i++) {
                delete[] matrix[i];
            }

            delete[] matrix;
            matrix = nullptr;
            size = 0;
        }
    }

    bool loadFromFile(string& filename) {
        // otwarcie pliku do odczytu
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Nie mozna otworzyc pliku!" << endl;
            return false;
        }

        int n;

        // wczytanie rozmiaru macierzy
        file >> n;

        // utworzenie macierzy o odpowiednim rozmiarze
        create(n);

        // wczytanie wszystkich elementów macierzy z pliku
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                file >> matrix[i][j];
            }
        }

        file.close();
        return true;
    }
};