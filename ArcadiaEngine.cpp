// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution
    struct Table_Entry{
            int key;
            string value;
            Table_Entry(int k, string v): key(k), value(v) {}
            Table_Entry() {}

        };  
        int HashTableSize;
        double loadFactor;
        int currentSize;
        Table_Entry* table;

        int hash1(int key){ // Multiplication method
            double a = 0.6180339887;
            double frac = key * a - floor(key * a); // Get fractional part
            return floor(HashTableSize * frac); // Scale to table size
        }
        int hash2(int key){
            return 89 - (key % 89); // A prime less than table size
        }
        void rehash(){
            int oldSize = HashTableSize;
            HashTableSize = HashTableSize * 2 + 1; // Force size to be odd
            Table_Entry* oldTable = table; // Save old table
            table = new Table_Entry[HashTableSize]; // Create new table with the new size
            for(int i = 0; i < HashTableSize; i++){
                table[i] = Table_Entry(-1, ""); // -1 indicates empty slot
            }
            currentSize = 0;
            // Reinsert old entries
            for(int i = 0; i < oldSize; i++){
                if(oldTable[i].key != -1){
                    insert(oldTable[i].key, oldTable[i].value);
                }
            }
            delete[] oldTable;
        }
public:
    ConcretePlayerTable() {
        // TODO: Initialize your hash table
        HashTableSize = 101;
        loadFactor = 0.75;
        currentSize = 0;
        table = new Table_Entry[HashTableSize];
        for(int i = 0; i < HashTableSize; i++){
            table[i] = Table_Entry(-1, ""); // -1 indicates empty slot
        }
    }

    void insert(int playerID, string name) override {
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)
        if((double)(currentSize + 1) / HashTableSize > loadFactor){ // Check load factor
            rehash(); 
        }
        int idx1 = hash1(playerID);
        int idx2 = hash2(playerID);
        int pos = idx1;
        int i = 0;
        while(table[pos].key != -1 && table[pos].key != playerID){ // Handle collisions using double hashing
            i++;
            pos = (idx1 + i * idx2) % HashTableSize;
        }
        if(table[pos].key == -1){ // Handle updating existing key
            currentSize++;
        }
        table[pos] = Table_Entry(playerID, name);
    }

    string search(int playerID) override {
        // TODO: Implement double hashing search
        int idx1 = hash1(playerID);
        int idx2 = hash2(playerID);
        int pos = idx1;
        int i = 0;
        while(table[pos].key != -1){ // Continue until an empty slot is found
            if(table[pos].key == playerID){
                return table[pos].value; // Found the key
            }
            i++;
            pos = (idx1 + i * idx2) % HashTableSize;
        }

        return ""; // Key not found   
    }
    ~ConcretePlayerTable() {
        delete[] table;
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers
    struct Skip_Node{
        int playerID;
        int score;
        vector<Skip_Node*> forward; // Forward pointers for different levels
        Skip_Node(int id, int sc, int level): playerID(id), score(sc) {
            forward.resize(level + 1, nullptr);
        }
    };
    Skip_Node* head;
    static const int maxLevel = 16; // Maximum level for skip list
    const float p = 0.5; // Probability for level generation
    int currentLevel; // Current highest level in the skip list
    int randomLevel(){ // Generate random level for new node
        int level = 0;
        while(((float)rand() / RAND_MAX) < p && level < maxLevel){
            level++;
        }
        return level;
    }
    bool isBetter(int scoreA, int idA, int scoreB, int idB) {
        if (scoreA != scoreB) {
            return scoreA > scoreB; // Descending Score
        }
        return idA < idB; // Tie-breaker
    }

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
        srand((unsigned)time(NULL)); // Seed for randomness
        currentLevel = 0;
        head = new Skip_Node(-1, -1, maxLevel);
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
        Skip_Node* update[maxLevel + 1]; // Track nodes at each level
        Skip_Node* current = head;
        for(int i = currentLevel; i >= 0; i--){
            while(current->forward[i] != nullptr && 
                isBetter(current->forward[i]->score, current->forward[i]->playerID, score, playerID)){
                current = current->forward[i];
            }
            update[i] = current; // Store last node at this level
        }

        int n_levels = randomLevel();
        if(n_levels > currentLevel){
            for(int i = currentLevel + 1; i <= n_levels; i++){  
                update[i] = head; // New levels point to head
            }
            currentLevel = n_levels;
        }
        Skip_Node* newNode = new Skip_Node(playerID, score, n_levels);
        for(int i = 0; i <= n_levels; i++){
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
        Skip_Node* current = head;
        Skip_Node* target = nullptr;

        while(current->forward[0] != nullptr){
            if(current->forward[0]->playerID == playerID){
                target = current->forward[0];
                break;
            }
            current = current->forward[0];
        }
        if(target == nullptr) return; // Player not found
        Skip_Node* update[maxLevel + 1];
        current = head;

        for(int i = currentLevel; i >= 0; i--){
            while(current->forward[i] != nullptr &&
                isBetter(current->forward[i]->score, current->forward[i]->playerID, target->score, target->playerID)){
                current = current->forward[i];
                }
            update[i] = current;
        }
        for(int i = 0; i <= currentLevel; i++){
            if(update[i]->forward[i] != target) break; // No need to update further levels
            update[i]->forward[i] = target->forward[i]; 
        }  
        delete target; 
        while(currentLevel > 0 && head->forward[currentLevel] == nullptr){ // Adjust current level
            currentLevel--;
        }
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        vector<int> result;
        Skip_Node* current = head->forward[0]; // Start from the lowest level
        while(current != nullptr && result.size() < n){
            result.push_back(current->playerID);
            current = current->forward[0];
        }
        return result;
    }
    ~ConcreteLeaderboard() {
        // Clean up skip list nodes
        Skip_Node* current = head;
        while(current != nullptr){
            Skip_Node* next = current->forward[0];
            delete current;
            current = next;
        }
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2
    return 0;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool pathVisit(int n, const vector<vector<int>>& edges, int source, int dest, vector<int>& visited) {
    if (source == dest)
        return true;
    visited[source] = true;
    for (int v : edges[source])
        if (!visited[v])
            if (pathVisit(n, edges, v, dest, visited))
                return true;
    return false;
}

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    vector<int> visited(n);
    return pathVisit(n, edges, source, dest, visited);
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    using Weight = long long;
    using V = int;
    using Edge = pair<Weight, V>;

    vector<vector<Edge>> adj(n);
    for (const auto& road : roadData)
    {
        V u = road[0], v = road[1];
        Weight goldCost = road[2], silverCost = road[3];
        Weight cost = goldCost*goldRate + silverCost*silverRate;
        adj[u].push_back({cost, v});
        adj[v].push_back({cost, u});
    }

    vector<Weight> minEdge(n, LLONG_MAX);
    vector<bool> selected(n, false);
    set<Edge> q;
    for (V v = 1; v < n; v++)
        q.insert({LLONG_MAX, v});
    q.insert({0, 0});
    minEdge[0] = 0;

    Weight minBribe = 0;
    int count = 0;
    while (!q.empty())
    {
        auto min = q.begin();
        auto [minW, u] = *min;
        q.erase(min);
        minBribe += minW;
        selected[u] = true;
        count++;

        for (auto [w, v] : adj[u])
        {
            if (!selected[v] && w < minEdge[v])
            {
                q.erase({minEdge[v], v});
                minEdge[v] = w;
                q.insert({w, v});
            }
        }
    }

    if (count != n)
        return -1;
    return minBribe;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() { 
        return new ConcretePlayerTable(); 
    }

    Leaderboard* createLeaderboard() { 
        return new ConcreteLeaderboard(); 
    }

    AuctionTree* createAuctionTree() { 
        return new ConcreteAuctionTree(); 
    }
}
