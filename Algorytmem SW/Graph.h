#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Graph {
public:
    int size;      // liczba miast
    int** matrix;  // macierz kosztow

    Graph() {
        size = 0;
        matrix = nullptr;
    }

    ~Graph() {
        clear();
    }

    // Tworze dynamiczna macierz kosztow o rozmiarze n x n.
    void create(int n) {
        clear();

        size = n;
        matrix = new int*[size];

        for (int i = 0; i < size; i++) {
            matrix[i] = new int[size];
        }
    }

    // Usuwam aktualna macierz z pamieci.
    void clear() {
        if (matrix != nullptr) {
            for (int i = 0; i < size; i++) {
                delete[] matrix[i];
            }

            delete[] matrix;
            matrix = nullptr;
            size = 0;
        }
    }

    // Wyswietlam aktualna macierz kosztow.
    void display() {
        if (matrix == nullptr || size <= 0) {
            cout << "Graf jest pusty." << endl;
            return;
        }

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Sprawdzam, czy linia zawiera tylko jedna liczbe.
    // Dzieki temu rozpoznaje stary prosty format pliku.
    bool isOnlyNumberLine(string line) {
        stringstream ss(line);
        int value;
        string extra;

        if (!(ss >> value)) {
            return false;
        }

        if (ss >> extra) {
            return false;
        }

        return true;
    }

    // Wczytuje prosty format:
    // pierwsza liczba to rozmiar n, a potem jest macierz n x n.
    bool readSimpleMatrixFormat(string filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Nie mozna otworzyc pliku!" << endl;
            return false;
        }

        int n;
        file >> n;

        if (n <= 0) {
            cout << "Niepoprawny rozmiar grafu w pliku." << endl;
            file.close();
            return false;
        }

        create(n);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (!(file >> matrix[i][j])) {
                    cout << "Blad podczas wczytywania macierzy." << endl;
                    clear();
                    file.close();
                    return false;
                }
            }
        }

        file.close();
        return true;
    }

    // Wczytuje format TSPLIB / ATSP.
    // Program szuka DIMENSION oraz EDGE_WEIGHT_SECTION.
    bool readATSPFormat(string filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Nie mozna otworzyc pliku!" << endl;
            return false;
        }

        string line;
        int n = 0;
        bool foundSection = false;

        // Czytam naglowek pliku .atsp.
        while (getline(file, line)) {
            // Zamieniam dwukropek na spacje, zeby latwiej odczytac np. DIMENSION: 171.
            for (int i = 0; i < (int)line.length(); i++) {
                if (line[i] == ':') {
                    line[i] = ' ';
                }
            }

            string keyword;
            stringstream ss(line);
            ss >> keyword;

            // Pobieram liczbe miast.
            if (keyword == "DIMENSION") {
                ss >> n;
            }

            // Po tej sekcji zaczyna sie macierz kosztow.
            if (keyword == "EDGE_WEIGHT_SECTION") {
                foundSection = true;
                break;
            }
        }

        if (n <= 0) {
            cout << "Nie znaleziono poprawnego DIMENSION w pliku ATSP." << endl;
            file.close();
            return false;
        }

        if (!foundSection) {
            cout << "Nie znaleziono EDGE_WEIGHT_SECTION w pliku ATSP." << endl;
            file.close();
            return false;
        }

        create(n);

        string token;
        int counter = 0;
        int neededValues = n * n;

        // Wczytuje kolejne wartosci macierzy.
        while (counter < neededValues && file >> token) {
            if (token == "EOF") {
                break;
            }

            int value = stoi(token);

            // Zamieniam numer kolejnej liczby na indeks wiersza i kolumny.
            int row = counter / n;
            int col = counter % n;

            matrix[row][col] = value;
            counter++;
        }

        file.close();

        if (counter != neededValues) {
            cout << "Blad: plik nie zawiera pelnej macierzy kosztow." << endl;
            clear();
            return false;
        }

        return true;
    }

    // Funkcja glowna do wczytywania pliku.
    // Najpierw rozpoznaje format, potem wywoluje odpowiednia metode.
    bool loadFromFile(string filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Nie mozna otworzyc pliku!" << endl;
            return false;
        }

        string firstLine;
        getline(file, firstLine);
        file.close();

        // Jesli pierwsza linia to jedna liczba, wczytuje prosty format.
        if (isOnlyNumberLine(firstLine)) {
            return readSimpleMatrixFormat(filename);
        }

        // W przeciwnym przypadku traktuje plik jako TSPLIB / ATSP.
        return readATSPFormat(filename);
    }
};

#endif