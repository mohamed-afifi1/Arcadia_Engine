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
            Table_Entry() : key(-1), value("") {} 

        };  
        int HashTableSize;
        int currentSize;
        vector<Table_Entry> table;

        int hash1(int key){ // Divison Method
            return key % 101;
        }
        int hash2(int key){
            return 97 - (key % 97); // A prime less than table size
        }
public:
    ConcretePlayerTable() {
        // TODO: Initialize your hash table
        HashTableSize = 101;
        currentSize = 0;
        table.resize(HashTableSize);
    }

    void insert(int playerID, string name) override {
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)
        int idx1 = hash1(playerID);
        int idx2 = hash2(playerID);
        int pos = idx1;
        int i = 0;
        while(table[pos].key != -1 && table[pos].key != playerID){ // Handle collisions using double hashing
            i++;
            if(i >= HashTableSize){ // Table full
                cout << "Table is Full" << endl;
                return;
            } 
            pos = (idx1 + i * idx2) % HashTableSize;
        }
        if(table[pos].key == -1){ // Handle updating existing key
            currentSize++; // increase size only if the key does not exist 
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
            if(i >= HashTableSize){ // to handle the case of searching for a player that does not exist when the table is full
                break;
            }
            pos = (idx1 + i * idx2) % HashTableSize;
        }

        return ""; // Key not found   
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
        int actualSize = 0;
        Skip_Node* tmp = head->forward[0];
        while(tmp != nullptr && actualSize < n){ // Handle case where the list has fewer than N players
            actualSize++;
            tmp = tmp->forward[0];
        }
        vector<int> topPlayers(actualSize);

        Skip_Node* current = head->forward[0];
        
        for(int i = 0; i < actualSize; i++){
            topPlayers[i] = current->playerID;
            current = current->forward[0];
        }
        return topPlayers;
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

    struct RBTreeNode {
        int id;
        int price;
        bool color;   // Black -> false, Red -> true
        RBTreeNode* parent;
        RBTreeNode* left;
        RBTreeNode* right;

        RBTreeNode(int id, int price) {
            this->id = id;
            this->price = price;
            this->color = true;
            this->parent = nullptr;
            this->left = nullptr;
            this->right = nullptr;
        }
    };

    RBTreeNode* root;


    void printTreeHelper(RBTreeNode* root, string indent, bool last) {
        if (root != nullptr) {
            cout << indent;
            if (last) {
                cout << "R----";
                indent += "   ";
            } else {
                cout << "L----";
                indent += "|  ";
            }

            string sColor = root->color ? "(RED)" : "(BLACK)";
            cout << "[" << root->id << ":" << root->price << "] " << sColor << endl;

            printTreeHelper(root->left, indent, false);
            printTreeHelper(root->right, indent, true);
        }
    }

    void rightRotate(RBTreeNode* child, RBTreeNode* parent) {
        // Right rotate child with parent
        if (parent->parent == nullptr) {
            root = child;
            child->parent = nullptr;
        }
        else {
            if(parent == parent->parent->right)
                parent->parent->right = child;
            else
                parent->parent->left = child;

            child->parent = parent->parent;
        }

        parent->parent = child;
        parent->left = child->right;
        if(child->right != nullptr)
            child->right->parent = parent;
        child->right = parent;
    }

    void leftRotate(RBTreeNode* child, RBTreeNode* parent) {
        // left rotate child with parent
        if (parent->parent == nullptr) {
            root = child;
            child->parent = nullptr;
        }
        else {
            if(parent == parent->parent->right)
                parent->parent->right = child;
            else
                parent->parent->left = child;

            child->parent = parent->parent;
        }

        parent->parent = child;
        parent->right = child->left;
        if(child->left != nullptr)
            child->left->parent = parent;
        child->left = parent;
    }

    void fixAfterInsertion(RBTreeNode* node) {
        // Case 0: Root is red
        // Case 1: Parent is black
        // Case 2: Parent and Uncle are red
        // Case 3: Parent is red, Uncle is black, and you are the right child
        // Case 4: Parent is red, Uncle is black, and you are the left child

        // Case 0
        if(root == node) {
            root->color = false;
            return;
        }

        // Case 1
        if(!node->parent->color) {
            return;
        }

        RBTreeNode* parent = node->parent;
        RBTreeNode* grandParent = parent->parent;

        if(grandParent == nullptr)
            return;

        RBTreeNode* uncle;

        if(parent == grandParent->left)
            uncle = grandParent->right;
        else
            uncle = grandParent->left;

        // Case 2
        if(uncle != nullptr && uncle->color) {
            parent->color = false;
            uncle->color = false;
            grandParent->color = true;
            fixAfterInsertion(grandParent);
            return;
        }

        // Case 3 & 4
        if(parent == grandParent->left) {
            if(node == parent->right) {
                leftRotate(node, parent);
                swap(node, parent);
            }

            grandParent->color = true; // Red
            parent->color = false; // Black
            rightRotate(parent, grandParent);
        }
        else {
            if(node == parent->left) {
                rightRotate(node, parent);
                swap(node, parent);
            }

            grandParent->color = true; // Red
            parent->color = false; // Black
            leftRotate(parent, grandParent);
        }
    }

    // O(n)
    RBTreeNode* searchItem(int itemID) {
        return DFS(itemID, root);
    }

    RBTreeNode* DFS(int itemID, RBTreeNode* node) {
        if(node == nullptr)
            return nullptr;
        if(node->id == itemID)
            return node;

        RBTreeNode* leftReturn = DFS(itemID, node->left);
        if(leftReturn != nullptr)
            return leftReturn;

        return DFS(itemID, node->right);
    }

    void fixAfterDeletion(RBTreeNode* node, RBTreeNode* parent) {
        // node is red, DB is make it black
        if(node != nullptr && node->color) {
            node->color = false;
            return;
        }

        // DB is root
        if(parent == nullptr)
            return;

        RBTreeNode* sibling;
        if(node == parent->left)
            sibling = parent->right;
        else
            sibling = parent->left;

        if(sibling != nullptr && sibling->color) {
            // sibling is red
            // recoloring
            parent->color = true; // red
            sibling->color = false; // black

            // rotation
            if(node == parent->left)
                leftRotate(sibling, parent);
            else
                rightRotate(sibling, parent);

            fixAfterDeletion(node, parent);
        }
        else {
            // sibling is black
            bool farChildRed = false;
            bool nearChildRed = false;

            if (sibling != nullptr) {
                if(node == parent->left) {
                    farChildRed = (sibling->right != nullptr && sibling->right->color);
                    nearChildRed = (sibling->left != nullptr && sibling->left->color);
                } else {
                    farChildRed = (sibling->left != nullptr && sibling->left->color);
                    nearChildRed = (sibling->right != nullptr && sibling->right->color);
                }
            }

            // sibling’s children are black
            if(!farChildRed && !nearChildRed) {
                // recoloring
                if(sibling != nullptr)
                    sibling->color = true; // red

                // propagate DB up
                if(parent->color) {
                    parent->color = false;
                    return;
                }

                fixAfterDeletion(parent, parent->parent);
            }
            else {
                // near red
                if(!farChildRed) {
                    RBTreeNode *nearChild;
                    if (node == parent->left) {
                        nearChild = sibling->left;
                        nearChild->color = false; // black
                        sibling->color = true; // red
                        rightRotate(nearChild, sibling);
                    } else {
                        nearChild = sibling->right;
                        nearChild->color = false; // black
                        sibling->color = true; // red
                        leftRotate(nearChild, sibling);
                    }

                    sibling = nearChild;
                }

                // far red
                RBTreeNode* farChild;
                if(node == parent->left) {
                    farChild = sibling->right;
                    // recoloring
                    farChild->color = false; // black
                    sibling->color = parent->color;
                    parent->color = false; // black

                    // rotation
                    leftRotate(sibling, parent);
                }
                else {
                    farChild = sibling->left;
                    // recoloring
                    farChild->color = false; // black
                    sibling->color = parent->color;
                    parent->color = false; // black

                    // rotation
                    rightRotate(sibling, parent);
                }
            }
        }
    }

    RBTreeNode* getMax(RBTreeNode* node) {
        RBTreeNode* curr = node;

        while(curr->right != nullptr)
            curr = curr->right;

        return curr;
    }

    void transplant(RBTreeNode* u, RBTreeNode* v) {
        if (u->parent == nullptr)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;

        if (v != nullptr)
            v->parent = u->parent;
    }

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
        root = nullptr;
    }

    // O(log n)
    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // left -> less than my price or equal price with less or equal id,
        // right -> greater than my price or equal price with greater id
        RBTreeNode* curr = root;
        RBTreeNode* prev = nullptr;
        RBTreeNode* newNode = new RBTreeNode(itemID, price);

        if(root == nullptr) {
            newNode->color = false;
            root = newNode;
            return;
        }

        while(curr != nullptr) {
            prev = curr;

            if(price < curr->price || (price == curr->price && itemID <= curr->id))
                curr = curr->left;
            else
                curr = curr->right;
        }

        newNode->parent = prev;

        if(price < prev->price || (price == prev->price && itemID <= prev->id))
            prev->left = newNode;
        else
            prev->right = newNode;

        fixAfterInsertion(newNode);
    }

    // O(n) search + O(log n) deletion
    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // Case 1: node does not have children
        // Case 2: node has one child
        // Case 3: node has two children

        RBTreeNode* node = searchItem(itemID);
        if(node == nullptr) {
            string msg = "No item with itemID: " + to_string(itemID);
            throw runtime_error(msg);
        }

        RBTreeNode* deletedNode = node;
        bool deletedNodeColor = deletedNode->color;
        RBTreeNode *child, *childParent;

        if(node->left != nullptr && node->right != nullptr) {
            deletedNode = getMax(node->left); // get predecessor
            deletedNodeColor = deletedNode->color;

            node->id = deletedNode->id;
            node->price = deletedNode->price;

            child = deletedNode->left;
            childParent = deletedNode->parent;

            transplant(deletedNode, child);
        }
        else {
            child = (node->left != nullptr) ? node->left : node->right;
            childParent = node->parent;
            transplant(node, child);
        }

        // If deleted node color is black, go to fix
        if(!deletedNodeColor) {
            fixAfterDeletion(child, childParent);
        }

        delete deletedNode;
    }

    void printTree() {
        printTreeHelper(root, "", true);
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
     // Calculate total sum
    int total = 0;
    for (int coin : coins) {
        total += coin;
    }
    
    int target = total / 2;
    
    // DP array: dp[i] = true if sum i is achievable
    vector<bool> dp(target + 1, false);
    dp[0] = true; // Base case: sum 0 is always achievable
    
    // For each coin, update possible sums
    for (int coin : coins) {
        // Traverse backwards to avoid using same coin twice
        for (int j = target; j >= coin; j--) {
            dp[j] = dp[j] || dp[j - coin];
        }
    }
    
    // Find closest achievable sum to target
    int closestSum = 0;
    for (int i = target; i >= 0; i--) {
        if (dp[i]) {
            closestSum = i;
            break;
        }
    }
    
    // Minimum difference = |closestSum - (total - closestSum)|
    return abs(2 * closestSum - total);
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int n = items.size();
    
    // DP array: dp[w] = max value achievable with weight w
    vector<int> dp(capacity + 1, 0);
    
    // Process each item
    for (int i = 0; i < n; i++) {
        int weight = items[i].first;
        int value = items[i].second;
        
        // Traverse backwards to ensure each item used only once
        for (int w = capacity; w >= weight; w--) {
            dp[w] = max(dp[w], dp[w - weight] + value);
        }
    }
    
    return dp[capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    int n = (int) s.length();
    const int mod = 1e9 + 7;
    vector<long long> dp(n + 2, 0);
    dp[0] = 1;
    dp[1] = 1;
    for(int i = 2; i <= n + 1; i++){
        dp[i] = (dp[i - 1] + dp[i - 2]) % mod;
    }
    long long result = 1;

    for(int i = 0; i < n; ){
        // If 'w' or 'm' exists, the message is impossible
        if(s[i] == 'w' || s[i] == 'm') {
            return 0; 
        }
        if(s[i] == 'u' || s[i] == 'n'){
            char c = s[i];
            int j = i;
            // count block length
            while (j < n && s[j] == c)
            {
                j++;
            }
            int len = j - i;
            result = (result * dp[len]) % mod;
            i = j;
        }
        else{
        i++;
        }
    }
    return result;
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
            if (!selected[v] && w < minEdge[v])
            {
                q.erase({minEdge[v], v});
                minEdge[v] = w;
                q.insert({w, v});
            }
    }

    if (count != n)
        return -1;
    return minBribe;
}

void addBinary(std::string& bin, int n)
{
    int carry = n;
    int i = bin.size() - 1;

    while (i >= 0 && carry > 0)
    {
        if (bin[i] == '0')
        {
            int bit = carry & 1;
            carry >>= 1;
            int sum = bit;
            if (sum == 1) bin[i] = '1';
        }
        else  // bin[i] == '1'
        {
            int bit = carry & 1;
            carry >>= 1;
            int sum = 1 + bit;
            if (sum == 1) bin[i] = '1';
            else if (sum == 2) { bin[i] = '0'; carry += 1; }
        }
        --i;
    }

    while (carry > 0)
    {
        bin.insert(bin.begin(), (carry & 1) ? '1' : '0');
        carry >>= 1;
    }
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // Assume directed graph 
    const int INF = -1;
    int *m = new int[n*n];
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
        {
            m[u*n+v] = (u == v) ? 0 : INF;
        }
    for (const auto& road : roads)
    {
        int u = road[0], v = road[1], w = road[2];
        m[u*n+v] = w;
    }
    for (int t = 0; t < n; t++)
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++)
            {
                if (m[u*n+t] == INF || m[t*n+v] == INF) continue;
                if (m[u*n+v] == INF || (m[u*n+t] + m[t*n+v] < m[u*n+v]))
                    m[u*n+v] = m[u*n+t] + m[t*n+v];
            }
    string sum = "0";
    for (int u = 0; u < n; u++)
        for (int v = u+1; v < n; v++)
        {
            int w = m[u*n+v];
            if (w == INF) continue;
            addBinary(sum, w);
        }
    delete[] m;
    return sum;
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    int Freq[26]={};
    int MaxFreq=0;
    int CountMax=0;
  //  int intervals=0;
    for (int i =0; i<tasks.size() ;i++) {
        Freq[tasks[i]-'A']++;
    }
    for (int i = 0; i < 26; i++)
        if (Freq[i] > MaxFreq) {
            MaxFreq=Freq[i];
        }
    for (int f = 0; f < 26; f++) {
        if (Freq[f] == MaxFreq) {
            CountMax++;
        }
    }
    int gaps=MaxFreq-1;
    int time=gaps* (n + 1) + CountMax;

    if (time > (int) tasks.size())
        return time;
    else
        return tasks.size();

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
