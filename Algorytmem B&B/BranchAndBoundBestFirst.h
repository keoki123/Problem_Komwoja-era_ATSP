#include <iostream>
#include <climits>
#include <chrono>

using namespace std;

// Węzeł reprezentujący jeden konkretny stan ścieżki w Best-First Search
struct BestFirstNode {
    int city;            // aktualne miasto, w którym teraz jestem
    int level;           // liczba miast wpisanych już do ścieżki
    int currentCost;     // koszt dotychczas zbudowanej ścieżki
    int bound;           // ograniczenie dolne wyliczone dla tego stanu
    int* path;           // tablica przechowująca aktualną ścieżkę
    bool* visited;       // tablica mówiąca, które miasta już odwiedziłem
    BestFirstNode* next; // wskaźnik na następny element w kolejce priorytetowej

    BestFirstNode(int n) {
        // Tworzę tablice potrzebne do przechowywania jednego stanu
        path = new int[n + 1];
        visited = new bool[n];

        // Na początku ustawiam pustą ścieżkę
        // i zaznaczam, że żadne miasto nie zostało jeszcze odwiedzone
        for (int i = 0; i < n; i++) {
            visited[i] = false;
            path[i] = -1;
        }

        path[n] = -1;
        bound = INT_MAX;
        next = nullptr;
    }

    ~BestFirstNode() {
        // Przy usuwaniu węzła usuwam pamięć zajmowaną przez jego tablice
        delete[] path;
        delete[] visited;
    }
};

// Własna kolejka priorytetowa uporządkowana rosnąco po bound
class BestFirstQueue {
public:
    BestFirstNode* head; // początek kolejki, czyli element z najmniejszym bound

    BestFirstQueue() {
        // Na początku kolejka jest pusta
        head = nullptr;
    }

    ~BestFirstQueue() {
        // Przy usuwaniu kolejki usuwam wszystkie elementy, które jeszcze w niej są
        while (!isEmpty()) {
            BestFirstNode* temp = pop();
            delete temp;
        }
    }

    bool isEmpty() {
        // Sprawdzam, czy kolejka jest pusta
        return head == nullptr;
    }

    // Wstawiam element tak, żeby kolejka była cały czas posortowana rosnąco po bound
    void push(BestFirstNode* node) {
        node->next = nullptr;

        // Jeśli kolejka jest pusta albo nowy element ma najmniejszy bound,
        // to wstawiam go na początek
        if (head == nullptr || node->bound < head->bound) {
            node->next = head;
            head = node;
            return;
        }

        BestFirstNode* current = head;

        // Idę po kolejce, aż znajdę miejsce,
        // w którym powinienem wstawić nowy element
        while (current->next != nullptr && current->next->bound <= node->bound) {
            current = current->next;
        }

        node->next = current->next;
        current->next = node;
    }

    // Pobieram element z najmniejszym bound, czyli z początku kolejki
    BestFirstNode* pop() {
        if (isEmpty()) {
            return nullptr;
        }

        BestFirstNode* temp = head;
        head = head->next;
        temp->next = nullptr;
        return temp;
    }
};

class BranchAndBoundBestFirst {
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

    BranchAndBoundBestFirst() {
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

    ~BranchAndBoundBestFirst() {
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

        // Jeśli takie wyjście istnieje, to dodaję je do ograniczenia
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

    // Kopiuję ścieżkę do bestPath
    void copyToBest(int* path) {
        for (int i = 0; i <= n; i++) {
            bestPath[i] = path[i];
        }
    }

    void solve(Graph& g, int startCity, bool choice, int initialBestCost = INT_MAX, int* initialBestPath = nullptr, double limitSeconds = 300.0) {
        // Na początku czyszczę stare dane
        clear();

        // Ustawiam dane wejściowe i parametry działania algorytmu
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
        if(choice){
            bestCost = initialBestCost;
    
            if (initialBestPath != nullptr) {
                copyToBest(initialBestPath);
            }       
        } else {
            bestCost = INT_MAX;
        }

        // Zapamiętuję moment startu algorytmu
        timeStart = chrono::high_resolution_clock::now();

        // Tworzę pustą kolejkę priorytetową
        BestFirstQueue queue;

        // ===================== TWORZĘ WĘZEŁ POCZĄTKOWY =====================
        BestFirstNode* root = new BestFirstNode(n);
        root->city = start;
        root->level = 1;
        root->currentCost = 0;
        root->visited[start] = true;
        root->path[0] = start;

        // Od razu liczę bound dla stanu początkowego
        root->bound = calculateBound(root->city, root->currentCost, root->visited);

        // Dodaję stan początkowy do kolejki
        queue.push(root);

        // ===================== GŁÓWNA PĘTLA BEST-FIRST SEARCH =====================
        while (!queue.isEmpty()) {
            // Sprawdzam, czy nie przekroczyłem limitu czasu (Na początku pętli)
            auto now = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(now - timeStart).count();

            if (elapsed >= timeLimitSeconds) {
                interrupted = true;
                finalExecutionTime = elapsed; // Zapisuję dokładny czas przerwania
                break;
            }

            // Pobieram stan z najmniejszym bound
            BestFirstNode* currentNode = queue.pop();

            // Jeśli sam bound tego stanu nie daje już szans na poprawę bestCost,
            // to od razu go odrzucam
            if (currentNode->bound >= bestCost) {
                delete currentNode;
                continue;
            }

            // Jeśli odwiedziłem już wszystkie miasta,
            // to sprawdzam powrót do miasta startowego
            if (currentNode->level == n) {
                if (matrix[currentNode->city][start] != -1) {
                    int totalCost = currentNode->currentCost + matrix[currentNode->city][start];

                    // Jeśli znalazłem lepszą pełną trasę, zapisuję ją jako najlepszą
                    if (totalCost < bestCost) {
                        bestCost = totalCost;
                        currentNode->path[n] = start;
                        copyToBest(currentNode->path);
                    }
                }

                delete currentNode;
                continue;
            }

            // ===================== ROZWIJAM AKTUALNY WĘZEŁ =====================
            for (int nextCity = 0; nextCity < n; nextCity++) {
                
                // Zabezpieczenie - sprawdzam czas wewnątrz pętli wstawiającej do kolejki
                auto nowInside = chrono::high_resolution_clock::now();
                double elapsedInside = chrono::duration<double>(nowInside - timeStart).count();
                if (elapsedInside >= timeLimitSeconds) {
                    interrupted = true;
                    finalExecutionTime = elapsedInside;
                    break;
                }

                if (!currentNode->visited[nextCity] && matrix[currentNode->city][nextCity] != -1) {
                    BestFirstNode* childNode = nullptr;
                    
                    // Zabezpieczenie przed brakiem pamięci RAM (OOM)
                    try {
                        childNode = new BestFirstNode(n);
                    } catch (const std::bad_alloc& e) {
                        interrupted = true;
                        // Jeśli brakło RAMu, też zatrzymujemy stoper
                        finalExecutionTime = elapsedInside;
                        break; 
                    }

                    // Ustawiam podstawowe dane nowego dziecka
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

                    // Liczę bound dla dziecka
                    childNode->bound = calculateBound(childNode->city, childNode->currentCost, childNode->visited);

                    // Jeśli dziecko ma sens, to dodaję je do kolejki
                    if (childNode->bound < bestCost) {
                        queue.push(childNode);
                    }
                    else {
                        // Jeśli dziecko i tak nie poprawi wyniku, to od razu je usuwam
                        delete childNode;
                    }
                }
            }

            // Wyskoczenie z pętli while, jeśli czas lub RAM się skończyły wewnątrz pętli for
            if (interrupted) {
                delete currentNode;
                break;
            }

            // Po rozwinięciu aktualnego stanu usuwam go z pamięci
            delete currentNode;
        }

        // Jeśli algorytm skończył się normalnie (przed czasem i bez błędu pamięci)
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

        // Jeśli algorytm został przerwany
        if (interrupted) {
            cout << "Algorytm zostal przerwany (limit czasu lub brak RAM)." << endl;
        }
    }
};