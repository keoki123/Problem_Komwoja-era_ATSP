#include <iostream>
#include <climits>
#include <chrono>

using namespace std;

// Węzeł reprezentujący jeden konkretny stan ścieżki w BFS
struct BFSNode {
    int city;           // aktualne miasto, w którym teraz jestem
    int level;          // liczba miast wpisanych już do ścieżki
    int currentCost;    // koszt dotychczas zbudowanej ścieżki
    int* path;          // tablica przechowująca aktualną ścieżkę
    bool* visited;      // tablica mówiąca, które miasta już odwiedziłem
    BFSNode* next;      // wskaźnik na następny element w kolejce

    // W konstruktorze tworzę tablice potrzebne dla jednego stanu BFS
    BFSNode(int n) {
        path = new int[n + 1];
        visited = new bool[n];

        // Na początku ustawiam ścieżkę jako pustą
        // i zaznaczam, że żadne miasto nie zostało jeszcze odwiedzone
        for (int i = 0; i < n; i++) {
            visited[i] = false;
            path[i] = -1;
        }

        path[n] = -1;
        next = nullptr;
    }

    // W destruktorze usuwam pamięć zaalokowaną dla tego węzła
    ~BFSNode() {
        delete[] path;
        delete[] visited;
    }
};

// Własna kolejka FIFO, której używam w BFS
class BFSQueue {
public:
    BFSNode* head; // początek kolejki, czyli element do pobrania jako pierwszy
    BFSNode* tail; // koniec kolejki, czyli miejsce gdzie dopisuję nowe elementy

    BFSQueue() {
        // Na początku kolejka jest pusta
        head = nullptr;
        tail = nullptr;
    }

    ~BFSQueue() {
        // Przy usuwaniu kolejki usuwam wszystkie elementy, które jeszcze w niej zostały
        while (!isEmpty()) {
            BFSNode* temp = pop();
            delete temp;
        }
    }

    bool isEmpty() {
        // Sprawdzam, czy kolejka jest pusta
        return head == nullptr;
    }

    // Dodaję nowy element na koniec kolejki
    void push(BFSNode* node) {
        node->next = nullptr;

        // Jeśli kolejka była pusta, to nowy element staje się i początkiem, i końcem
        if (tail == nullptr) {
            head = node;
            tail = node;
        }
        else {
            // Jeśli kolejka nie była pusta, to dopisuję element na koniec
            tail->next = node;
            tail = node;
        }
    }

    // Pobieram element z początku kolejki
    BFSNode* pop() {
        // Jeśli kolejka jest pusta, to nie mam czego pobrać
        if (isEmpty()) {
            return nullptr;
        }

        BFSNode* temp = head;
        head = head->next;

        // Jeśli po pobraniu kolejka zrobiła się pusta, to zeruję też tail
        if (head == nullptr) {
            tail = nullptr;
        }

        temp->next = nullptr;
        return temp;
    }
};

class BranchAndBoundBFS {
public:
    int bestCost;       // najlepszy znaleziony koszt pełnej trasy
    int* bestPath;      // najlepsza znaleziona ścieżka
    int n;              // liczba miast
    int start;          // miasto startowe
    int** matrix;       // wskaźnik na macierz kosztów

    chrono::high_resolution_clock::time_point timeStart;
    double timeLimitSeconds;
    bool interrupted;

    // Dokładny czas działania algorytmu bez czasu sprzątania pamięci
    double finalExecutionTime;

    BranchAndBoundBFS() {
        // Ustawiam wartości początkowe obiektu
        bestCost = INT_MAX;
        bestPath = nullptr;
        n = 0;
        start = 0;
        matrix = nullptr;
        timeLimitSeconds = 300.0;
        interrupted = false;
        finalExecutionTime = 0.0;
    }

    ~BranchAndBoundBFS() {
        // Przy usuwaniu obiektu czyszczę pamięć
        clear();
    }

    void clear() {
        // Usuwam najlepszą ścieżkę, jeśli była wcześniej zaalokowana
        if (bestPath != nullptr) {
            delete[] bestPath;
            bestPath = nullptr;
        }

        // Resetuję pozostałe pola obiektu
        bestCost = INT_MAX;
        n = 0;
        start = 0;
        matrix = nullptr;
        interrupted = false;
        finalExecutionTime = 0.0;
    }

    // Szukam najtańszego wyjścia z danego miasta
    int firstMin(int city) {
        int minCost = INT_MAX;

        for (int j = 0; j < n; j++) {
            if (j != city && matrix[city][j] != -1 && matrix[city][j] < minCost) {
                minCost = matrix[city][j];
            }
        }

        return minCost;
    }

    // Liczę ograniczenie dolne dla konkretnego stanu
    int calculateBound(int currentCity, int currentCost, bool* nodeVisited) {
        // Na początku do bound wpisuję koszt już zbudowanej części ścieżki
        int bound = currentCost;

        // Szukam najtańszego wyjścia z aktualnego miasta
        int minFromCurrent = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (!nodeVisited[j] && matrix[currentCity][j] != -1) {
                if (matrix[currentCity][j] < minFromCurrent) {
                    minFromCurrent = matrix[currentCity][j];
                }
            }
        }

        // Jeśli takie wyjście istnieje, to dodaję je do bound
        if (minFromCurrent != INT_MAX) {
            bound += minFromCurrent;
        }

        // Dla każdego jeszcze nieodwiedzonego miasta dodaję jego najtańsze wyjście
        for (int i = 0; i < n; i++) {
            if (!nodeVisited[i]) {
                int m = firstMin(i);
                if (m != INT_MAX) {
                    bound += m;
                }
            }
        }

        return bound;
    }

    // Kopiuję gotową ścieżkę do bestPath
    void copyToBest(int* path) {
        for (int i = 0; i <= n; i++) {
            bestPath[i] = path[i];
        }
    }

    void solve(Graph& g, int startCity, bool choice, int initialBestCost = INT_MAX, int* initialBestPath = nullptr, double limitSeconds = 300.0) {
        // Na początku czyszczę stare dane
        clear();

        // Podpinam aktualny graf i ustawiam parametry działania
        n = g.size;
        matrix = g.matrix;
        start = startCity;
        timeLimitSeconds = limitSeconds;
        interrupted = false;
        finalExecutionTime = 0.0;

        // Tworzę tablicę na najlepszą ścieżkę
        bestPath = new int[n + 1];
        for (int i = 0; i <= n; i++) {
            bestPath[i] = -1;
        }

        // Jeśli mam rozwiązanie początkowe, to ustawiam je jako pierwszy bestCost
        if(choice == true){
            // Ustawiam rozwiązanie początkowe, np. z NN
            bestCost = initialBestCost;
    
            if (initialBestPath != nullptr) {
                copyToBest(initialBestPath);
            }       
        } else {
            // Jeśli nie mam rozwiązania początkowego, to zaczynam od bardzo dużego kosztu
            bestCost = INT_MAX;
        }
        
        // Zapamiętuję moment startu algorytmu
        timeStart = chrono::high_resolution_clock::now();

        // Tworzę pustą kolejkę BFS
        BFSQueue queue;

        // ===================== TWORZĘ WĘZEŁ POCZĄTKOWY =====================
        BFSNode* root = new BFSNode(n);
        root->city = start;
        root->level = 1;
        root->currentCost = 0;
        root->visited[start] = true;
        root->path[0] = start;

        // Wrzucam stan początkowy do kolejki
        queue.push(root);

        // ===================== GŁÓWNA PĘTLA BFS =====================
        while (!queue.isEmpty()) {
            // Sprawdzam, czy nie przekroczyłem limitu czasu na początku głównej pętli
            auto now = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(now - timeStart).count();

            if (elapsed >= timeLimitSeconds) {
                interrupted = true;
                finalExecutionTime = elapsed; // Zapisuję dokładny czas
                break;
            }

            // Pobieram kolejny stan z początku kolejki
            BFSNode* currentNode = queue.pop();

            // Jeśli odwiedziłem już wszystkie miasta, to sprawdzam powrót do startu
            if (currentNode->level == n) {
                if (matrix[currentNode->city][start] != -1) {
                    int totalCost = currentNode->currentCost + matrix[currentNode->city][start];

                    // Jeśli znalazłem lepszą pełną trasę, to zapisuję ją jako najlepszą
                    if (totalCost < bestCost) {
                        bestCost = totalCost;
                        currentNode->path[n] = start;
                        copyToBest(currentNode->path);
                    }
                }

                delete currentNode;
                continue;
            }

            // Liczę ograniczenie dolne dla aktualnego stanu
            int bound = calculateBound(currentNode->city, currentNode->currentCost, currentNode->visited);

            // Jeśli bound jest gorszy lub równy bestCost, to odcinam tę gałąź
            if (bound >= bestCost) {
                delete currentNode;
                continue;
            }

            // ===================== ROZWIJAM AKTUALNY WĘZEŁ =====================
            for (int nextCity = 0; nextCity < n; nextCity++) {

                // Zabezpieczenie przed zablokowaniem systemu (sprawdzanie czasu wewnątrz)
                auto nowInside = chrono::high_resolution_clock::now();
                double elapsedInside = chrono::duration<double>(nowInside - timeStart).count();
                if (elapsedInside >= timeLimitSeconds) {
                    interrupted = true;
                    finalExecutionTime = elapsedInside;
                    break;
                }

                if (!currentNode->visited[nextCity] && matrix[currentNode->city][nextCity] != -1) {
                    BFSNode* childNode = nullptr;
                    
                    // Zabezpieczenie przed brakiem pamięci RAM (OOM)
                    try {
                        childNode = new BFSNode(n);
                    } catch (const std::bad_alloc& e) {
                        interrupted = true;
                        // Jeśli brakło RAMu, też zatrzymujemy stoper
                        finalExecutionTime = elapsedInside;
                        break; 
                    }

                    // Ustawiam podstawowe informacje o dziecku
                    childNode->city = nextCity;
                    childNode->level = currentNode->level + 1;
                    childNode->currentCost = currentNode->currentCost + matrix[currentNode->city][nextCity];

                    // Kopiuję stan rodzica do dziecka
                    for (int i = 0; i < n; i++) {
                        childNode->visited[i] = currentNode->visited[i];
                        childNode->path[i] = currentNode->path[i];
                    }

                    // Aktualizuję dziecko o nowe miasto
                    childNode->visited[nextCity] = true;
                    childNode->path[currentNode->level] = nextCity;

                    // Dodaję nowe dziecko na koniec kolejki
                    queue.push(childNode);
                }
            }

            // Wyskoczenie z pętli while, jeśli limit czasu lub RAM skończyły się podczas generowania dzieci
            if (interrupted) {
                delete currentNode;
                break;
            }

            // Po rozwinięciu aktualnego stanu usuwam go z pamięci
            delete currentNode;
        }

        // Jeśli algorytm przeszedł cały bez przerwania
        if (!interrupted) {
            auto endNow = chrono::high_resolution_clock::now();
            finalExecutionTime = chrono::duration<double>(endNow - timeStart).count();
        }
    }

    void displayResult() {
        // Jeśli nie znalazłem żadnego rozwiązania, wypisuję odpowiedni komunikat
        if (bestCost == INT_MAX) {
            cout << "Nie znaleziono rozwiazania." << endl;
            return;
        }

        cout << "Najlepszy koszt: " << bestCost << endl;
        cout << "Najlepsza sciezka: ";

        // Wypisuję kolejne miasta zapisane w najlepszej ścieżce
        for (int i = 0; i <= n; i++) {
            cout << bestPath[i];
            if (i < n) {
                cout << " -> ";
            }
        }

        cout << endl;

        // Zaktualizowany komunikat w przypadku przerwania
        if (interrupted) {
            cout << "Algorytm zostal przerwany (limit czasu lub brak RAM)." << endl;
        }
    }
};