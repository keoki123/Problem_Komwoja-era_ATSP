#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <climits>

#include "Graph.h"

using namespace std;

class GeneticAlgorithm {
public:
    enum CrossoverMethod {
        OX = 1,
        PMX = 2
    };

    enum MutationMethod {
        SWAP = 1,
        INVERSION = 2
    };

    enum SelectionMethod {
        TOURNAMENT = 1,
        ROULETTE = 2
    };

    struct Individual {
        vector<int> path;
        int cost;

        Individual() {
            cost = INT_MAX;
        }
    };

    int bestCost;
    vector<int> bestPath;

    double finalExecutionTime;
    double finalAverageCost;
    double bestFoundTime;

    int generationsCount;

private:
    int populationSize;
    double mutationRate;
    double crossoverRate;

    CrossoverMethod crossoverMethod;
    MutationMethod mutationMethod;
    SelectionMethod selectionMethod;

    vector<Individual> population;
    mt19937 generator;

public:
    GeneticAlgorithm() {
        bestCost = INT_MAX;
        bestPath.clear();

        finalExecutionTime = 0.0;
        finalAverageCost = 0.0;
        bestFoundTime = 0.0;

        generationsCount = 0;

        // Domyslne mocniejsze parametry.
        populationSize = 500;
        mutationRate = 0.10;
        crossoverRate = 0.90;

        crossoverMethod = OX;
        mutationMethod = SWAP;
        selectionMethod = TOURNAMENT;

        random_device rd;
        generator = mt19937(rd());
    }

    void setPopulationSize(int value) {
        if (value >= 2) {
            populationSize = value;
        }
    }

    void setMutationRate(double value) {
        if (value >= 0.0 && value <= 1.0) {
            mutationRate = value;
        }
    }

    void setCrossoverRate(double value) {
        if (value >= 0.0 && value <= 1.0) {
            crossoverRate = value;
        }
    }

    void setCrossoverMethod(int value) {
        if (value == OX) {
            crossoverMethod = OX;
        } else if (value == PMX) {
            crossoverMethod = PMX;
        }
    }

    void setMutationMethod(int value) {
        if (value == SWAP) {
            mutationMethod = SWAP;
        } else if (value == INVERSION) {
            mutationMethod = INVERSION;
        }
    }

    void setSelectionMethod(int value) {
        if (value == TOURNAMENT) {
            selectionMethod = TOURNAMENT;
        } else if (value == ROULETTE) {
            selectionMethod = ROULETTE;
        }
    }

    int getPopulationSize() {
        return populationSize;
    }

    double getMutationRate() {
        return mutationRate;
    }

    double getCrossoverRate() {
        return crossoverRate;
    }

    int getCrossoverMethod() {
        return crossoverMethod;
    }

    int getMutationMethod() {
        return mutationMethod;
    }

    int getSelectionMethod() {
        return selectionMethod;
    }

    double getElapsedTime(chrono::steady_clock::time_point startTime) {
        auto now = chrono::steady_clock::now();
        return chrono::duration<double>(now - startTime).count();
    }

    bool isTimeExceeded(chrono::steady_clock::time_point startTime, double stopTimeSeconds) {
        return getElapsedTime(startTime) >= stopTimeSeconds;
    }

    int calculatePathCost(Graph& graph, vector<int>& path) {
        int cost = 0;
        int n = graph.size;

        for (int i = 0; i < n - 1; i++) {
            cost += graph.matrix[path[i]][path[i + 1]];
        }

        cost += graph.matrix[path[n - 1]][path[0]];

        return cost;
    }

    vector<int> createRandomPath(int n) {
        vector<int> path;

        for (int i = 0; i < n; i++) {
            path.push_back(i);
        }

        shuffle(path.begin(), path.end(), generator);

        return path;
    }

    void evaluateIndividual(Graph& graph, Individual& individual) {
        individual.cost = calculatePathCost(graph, individual.path);
    }

    void updateBest(Individual& individual, double currentTime) {
        if (individual.cost < bestCost) {
            bestCost = individual.cost;
            bestPath = individual.path;
            bestFoundTime = currentTime;
        }
    }

    void createInitialPopulation(Graph& graph, chrono::steady_clock::time_point startTime) {
        population.clear();

        int n = graph.size;

        // Czysty algorytm genetyczny:
        // populacja poczatkowa sklada sie tylko z losowych permutacji miast.
        while ((int)population.size() < populationSize) {
            Individual individual;

            individual.path = createRandomPath(n);

            evaluateIndividual(graph, individual);

            population.push_back(individual);

            double currentTime = getElapsedTime(startTime);
            updateBest(individual, currentTime);
        }
    }

    bool shouldDo(double probability) {
        uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(generator) < probability;
    }

    Individual tournamentSelection() {
        int actualSize = (int)population.size();

        int tournamentSize = 7;

        if (actualSize < tournamentSize) {
            tournamentSize = actualSize;
        }

        uniform_int_distribution<int> dist(0, actualSize - 1);

        Individual best = population[dist(generator)];

        for (int i = 1; i < tournamentSize; i++) {
            Individual candidate = population[dist(generator)];

            if (candidate.cost < best.cost) {
                best = candidate;
            }
        }

        return best;
    }

    Individual rouletteSelection() {
        int actualSize = (int)population.size();

        double totalFitness = 0.0;

        for (int i = 0; i < actualSize; i++) {
            totalFitness += 1.0 / (population[i].cost + 1.0);
        }

        uniform_real_distribution<double> dist(0.0, totalFitness);
        double randomValue = dist(generator);

        double currentSum = 0.0;

        for (int i = 0; i < actualSize; i++) {
            currentSum += 1.0 / (population[i].cost + 1.0);

            if (currentSum >= randomValue) {
                return population[i];
            }
        }

        return population[actualSize - 1];
    }

    Individual selectParent() {
        if (selectionMethod == TOURNAMENT) {
            return tournamentSelection();
        }

        return rouletteSelection();
    }

    bool containsCity(vector<int>& path, int city) {
        for (int i = 0; i < (int)path.size(); i++) {
            if (path[i] == city) {
                return true;
            }
        }

        return false;
    }

    int findCityPosition(vector<int>& path, int city) {
        for (int i = 0; i < (int)path.size(); i++) {
            if (path[i] == city) {
                return i;
            }
        }

        return -1;
    }

    vector<int> orderCrossover(vector<int>& parent1, vector<int>& parent2) {
        int n = (int)parent1.size();
        vector<int> child(n, -1);

        uniform_int_distribution<int> dist(0, n - 1);

        int left = dist(generator);
        int right = dist(generator);

        if (left > right) {
            swap(left, right);
        }

        for (int i = left; i <= right; i++) {
            child[i] = parent1[i];
        }

        int childPosition = (right + 1) % n;
        int parentPosition = (right + 1) % n;

        while (containsCity(child, -1)) {
            int city = parent2[parentPosition];

            if (!containsCity(child, city)) {
                child[childPosition] = city;
                childPosition = (childPosition + 1) % n;
            }

            parentPosition = (parentPosition + 1) % n;
        }

        return child;
    }

    vector<int> partiallyMappedCrossover(vector<int>& parent1, vector<int>& parent2) {
        int n = (int)parent1.size();
        vector<int> child(n, -1);

        uniform_int_distribution<int> dist(0, n - 1);

        int left = dist(generator);
        int right = dist(generator);

        if (left > right) {
            swap(left, right);
        }

        for (int i = left; i <= right; i++) {
            child[i] = parent1[i];
        }

        for (int i = left; i <= right; i++) {
            int cityFromParent2 = parent2[i];

            if (!containsCity(child, cityFromParent2)) {
                int position = i;

                while (child[position] != -1) {
                    int mappedCity = parent1[position];
                    position = findCityPosition(parent2, mappedCity);

                    if (position == -1) {
                        break;
                    }
                }

                if (position != -1) {
                    child[position] = cityFromParent2;
                }
            }
        }

        for (int i = 0; i < n; i++) {
            if (child[i] == -1) {
                child[i] = parent2[i];
            }
        }

        return child;
    }

    vector<int> crossover(vector<int>& parent1, vector<int>& parent2) {
        if (crossoverMethod == OX) {
            return orderCrossover(parent1, parent2);
        }

        return partiallyMappedCrossover(parent1, parent2);
    }

    void swapMutation(vector<int>& path) {
        int n = (int)path.size();

        if (n <= 1) {
            return;
        }

        uniform_int_distribution<int> dist(0, n - 1);

        int first = dist(generator);
        int second = dist(generator);

        while (first == second) {
            second = dist(generator);
        }

        swap(path[first], path[second]);
    }

    void inversionMutation(vector<int>& path) {
        int n = (int)path.size();

        if (n <= 1) {
            return;
        }

        uniform_int_distribution<int> dist(0, n - 1);

        int left = dist(generator);
        int right = dist(generator);

        if (left > right) {
            swap(left, right);
        }

        reverse(path.begin() + left, path.begin() + right + 1);
    }

    void mutate(vector<int>& path) {
        if (mutationMethod == SWAP) {
            swapMutation(path);
        } else {
            inversionMutation(path);
        }
    }

    double calculateAverageCost() {
        if (population.empty()) {
            return 0.0;
        }

        double sum = 0.0;

        for (int i = 0; i < (int)population.size(); i++) {
            sum += population[i].cost;
        }

        return sum / population.size();
    }

    void addEliteIndividuals(vector<Individual>& newPopulation, chrono::steady_clock::time_point startTime) {
        vector<Individual> sortedPopulation = population;

        sort(sortedPopulation.begin(), sortedPopulation.end(),
             [](const Individual& a, const Individual& b) {
                 return a.cost < b.cost;
             });

        int eliteCount = populationSize / 20;

        if (eliteCount < 1) {
            eliteCount = 1;
        }

        if (eliteCount > 10) {
            eliteCount = 10;
        }

        for (int i = 0; i < eliteCount && i < (int)sortedPopulation.size(); i++) {
            newPopulation.push_back(sortedPopulation[i]);

            double currentTime = getElapsedTime(startTime);
            updateBest(sortedPopulation[i], currentTime);
        }
    }

    void solve(Graph& graph, double stopTimeSeconds) {
        bestCost = INT_MAX;
        bestPath.clear();

        finalExecutionTime = 0.0;
        finalAverageCost = 0.0;
        bestFoundTime = 0.0;

        generationsCount = 0;
        population.clear();

        if (graph.matrix == nullptr || graph.size <= 1) {
            cout << "Blad: graf jest pusty albo ma zbyt malo miast." << endl;
            return;
        }

        if (populationSize < 2) {
            populationSize = 2;
        }

        auto startTime = chrono::steady_clock::now();

        createInitialPopulation(graph, startTime);

        while (!isTimeExceeded(startTime, stopTimeSeconds)) {
            vector<Individual> newPopulation;

            addEliteIndividuals(newPopulation, startTime);

            while ((int)newPopulation.size() < populationSize) {
                if (isTimeExceeded(startTime, stopTimeSeconds)) {
                    break;
                }

                Individual parent1 = selectParent();
                Individual parent2 = selectParent();

                Individual child1;
                Individual child2;

                if (shouldDo(crossoverRate)) {
                    child1.path = crossover(parent1.path, parent2.path);
                    child2.path = crossover(parent2.path, parent1.path);
                } else {
                    child1.path = parent1.path;
                    child2.path = parent2.path;
                }

                if (shouldDo(mutationRate)) {
                    mutate(child1.path);
                }

                if (shouldDo(mutationRate)) {
                    mutate(child2.path);
                }

                evaluateIndividual(graph, child1);

                double currentTime1 = getElapsedTime(startTime);
                updateBest(child1, currentTime1);

                newPopulation.push_back(child1);

                if ((int)newPopulation.size() < populationSize) {
                    evaluateIndividual(graph, child2);

                    double currentTime2 = getElapsedTime(startTime);
                    updateBest(child2, currentTime2);

                    newPopulation.push_back(child2);
                }
            }

            population = newPopulation;
            generationsCount++;
        }

        finalExecutionTime = getElapsedTime(startTime);
        finalAverageCost = calculateAverageCost();
    }

    void displayPath(vector<int>& path) {
        if (path.empty()) {
            cout << "Brak trasy." << endl;
            return;
        }

        for (int i = 0; i < (int)path.size(); i++) {
            cout << path[i] << " -> ";
        }

        cout << path[0] << endl;
    }
};

#endif