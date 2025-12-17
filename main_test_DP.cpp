#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>
#include "ArcadiaEngine.h" 


void runTestCases() {
    InventorySystem inv;
    
    cout << "=== TEST: optimizeLootSplit ===" << endl;
    
    // Test 1: Perfect partition possible
    vector<int> coins1 = {1, 5, 11, 5};
    cout << "Test 1: {1, 5, 11, 5} -> Expected: 0, Got: " 
         << inv.optimizeLootSplit(4, coins1) << endl;
    // Two subsets: {11, 1} = 12 and {5, 5} = 10, diff = 2
    // Actually: {11} = 11 and {5, 5, 1} = 11, diff = 0
    
    // Test 2: Cannot partition perfectly
    vector<int> coins2 = {1, 6, 11, 5};
    cout << "Test 2: {1, 6, 11, 5} -> Expected: 1, Got: " 
         << inv.optimizeLootSplit(4, coins2) << endl;
    // Total = 23, best split: 11 vs 12
    
    // Test 3: All same values
    vector<int> coins3 = {3, 3, 3, 3};
    cout << "Test 3: {3, 3, 3, 3} -> Expected: 0, Got: " 
         << inv.optimizeLootSplit(4, coins3) << endl;
    
    // Test 4: Single element
    vector<int> coins4 = {10};
    cout << "Test 4: {10} -> Expected: 10, Got: " 
         << inv.optimizeLootSplit(1, coins4) << endl;
    
    // Test 5: Two elements
    vector<int> coins5 = {7, 3};
    cout << "Test 5: {7, 3} -> Expected: 4, Got: " 
         << inv.optimizeLootSplit(2, coins5) << endl;
    
    cout << "\n=== TEST: maximizeCarryValue ===" << endl;
    
    // Test 1: Standard knapsack
    vector<pair<int, int>> items1 = {{2, 6}, {2, 3}, {6, 5}, {5, 4}, {4, 6}};
    cout << "Test 1: capacity=10 -> Expected: 15, Got: " 
         << inv.maximizeCarryValue(10, items1) << endl;
    // Best: items with weights 2, 2, 4 -> values 6+3+6 = 15
    
    // Test 2: Can't fit anything
    vector<pair<int, int>> items2 = {{10, 100}, {20, 200}};
    cout << "Test 2: capacity=5 -> Expected: 0, Got: " 
         << inv.maximizeCarryValue(5, items2) << endl;
    
    // Test 3: All items fit
    vector<pair<int, int>> items3 = {{1, 5}, {2, 10}, {3, 15}};
    cout << "Test 3: capacity=10 -> Expected: 30, Got: " 
         << inv.maximizeCarryValue(10, items3) << endl;
    
    // Test 4: High value but heavy vs low value but light
    vector<pair<int, int>> items4 = {{8, 100}, {1, 10}, {1, 10}, {1, 10}};
    cout << "Test 4: capacity=10 -> Expected: 100, Got: " 
         << inv.maximizeCarryValue(10, items4) << endl;
    
    // Test 5: Zero capacity
    vector<pair<int, int>> items5 = {{1, 5}, {2, 10}};
    cout << "Test 5: capacity=0 -> Expected: 0, Got: " 
         << inv.maximizeCarryValue(0, items5) << endl;
    
    cout << "\n=== TEST: countStringPossibilities ===" << endl;
    
    // Test 1: Single pair "uu"
    cout << "Test 1: \"uu\" -> Expected: 2, Got: " 
         << inv.countStringPossibilities("uu") << endl;
    // Decodings: "uu" or "w"
    
    // Test 2: Single pair "nn"
    cout << "Test 2: \"nn\" -> Expected: 2, Got: " 
         << inv.countStringPossibilities("nn") << endl;
    // Decodings: "nn" or "m"
    
    // Test 3: Two pairs "uunn"
    cout << "Test 3: \"uunn\" -> Expected: 4, Got: " 
         << inv.countStringPossibilities("uunn") << endl;
    // Decodings: "uunn", "wnn", "uum", "wm"
    
    // Test 4: No special pairs
    cout << "Test 4: \"abc\" -> Expected: 1, Got: " 
         << inv.countStringPossibilities("abc") << endl;
    
    // Test 5: Multiple "uu" pairs
    cout << "Test 5: \"uuuu\" -> Expected: 5, Got: " 
         << inv.countStringPossibilities("uuuu") << endl;
    // Decodings: "uuuu", "wuu", "uuw", "ww", and wait...
    // Actually: dp[0]=1, dp[2]=2 (uu or w), dp[4]=dp[2]+dp[2]=2+2=4
    // Let me recalculate: "uuuu", "wuu", "uuw", "ww" = 4... 
    // Hmm, need to verify this
    
    // Test 6: Mixed characters
    cout << "Test 6: \"unnu\" -> Expected: 2, Got: " 
         << inv.countStringPossibilities("unnu") << endl;
    // Only one "nn" pair: "unnu" or "umu"
    
    // Test 7: Empty string
    cout << "Test 7: \"\" -> Expected: 1, Got: " 
         << inv.countStringPossibilities("") << endl;
    
    // Test 8: Complex case
    cout << "Test 8: \"uunnuu\" -> Expected: 8, Got: " 
         << inv.countStringPossibilities("uunnuu") << endl;
    // Multiple decodings possible
}

int main() {
    runTestCases();
    return 0;
}