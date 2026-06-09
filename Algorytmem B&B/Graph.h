#include <iostream>
#include <climits>
#include <cstdlib> 
#include <fstream>

using namespace std;

class Graph {
public:
    int size;      // liczba miast, czyli rozmiar macierzy kosztów
    int** matrix;  // dynamicznie alokowana macierz kosztów przejść między miastami

    Graph() {
        // Na początku graf jest pusty:
        // nie ma żadnych miast i nie ma zaalokowanej macierzy.
        size = 0;
        matrix = nullptr;
    }

    ~Graph() {
        // Destruktor klasy.
        // Przy usuwaniu obiektu Graph zwalniana jest pamięć zajmowana przez macierz.
        clear();
    }

    void create(int n) {
        // Funkcja tworzy nową macierz kosztów o rozmiarze n x n.
        // Jeśli wcześniej istniała już jakaś macierz, to najpierw trzeba ją usunąć

        clear();

        size = n;

        // Alokacja pamięci na tablicę wskaźników do kolejnych wierszy.
        matrix = new int*[size];

        // Alokacja pamięci na każdy wiersz macierzy.
        for (int i = 0; i < size; i++) {
            matrix[i] = new int[size];
        }
    }

    void generateRandom() {
        // Funkcja losowo wypełnia macierz kosztów.
        // Na przekątnej wpisywana jest wartość -1,
        // ponieważ przejście z miasta do samego siebie nie jest dozwolone.
        // Poza przekątną losowane są koszty z zakresu 10..99.
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    // Brak przejścia z miasta do samego siebie.
                    matrix[i][j] = -1;
                } else {
                    // Losowy koszt w zakresie 10..99.
                    matrix[i][j] = rand() % 90 + 10;
                }
            }
        }
    }

    void display() {
        // Funkcja wypisuje całą macierz kosztów na ekran.
        // Każdy wiersz macierzy jest wyświetlany w osobnej linii.
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    void clear() {
        // Funkcja zwalnia pamięć zajmowaną przez macierz.
        // Najpierw usuwane są wszystkie wiersze,
        // a dopiero potem tablica wskaźników do tych wierszy.
        if (matrix != nullptr) {
            for (int i = 0; i < size; i++) {
                delete[] matrix[i];
            }

            delete[] matrix;

            // Po usunięciu pamięci zerujemy dane,
            // żeby obiekt znowu był w stanie "pustego grafu".
            matrix = nullptr;
            size = 0;
        }
    }

    bool loadFromFile(string& filename) {
        // Funkcja wczytuje graf z pliku tekstowego.
        // W pierwszej linii pliku powinna znajdować się liczba miast,
        // a w kolejnych liniach pełna macierz kosztów.

        ifstream file(filename);

        // Sprawdzenie, czy plik udało się poprawnie otworzyć.
        if (!file.is_open()) {
            cout << "Nie mozna otworzyc pliku!" << endl;
            return false;
        }

        int n;

        // Wczytanie liczby miast z pierwszej linii pliku.
        file >> n;

        // Utworzenie macierzy o odpowiednim rozmiarze.
        create(n);

        // Wczytanie wszystkich wartości macierzy kosztów z pliku.
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                file >> matrix[i][j];
            }
        }

        file.close();
        return true;
    }
};