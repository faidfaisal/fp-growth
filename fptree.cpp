#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;

int totalFrequentItemsets = 0; //Global counter to keep track of the total number of frequent item sets

struct FPNode { //define struc called FPNode
    string item; //string variable that stores the item name in the transaction
    int count;  //count to store the frequency
    FPNode* parent; //Pointer to the parent node in the FPtree
    FPNode* nodeLink; //Node to link items together (algorithm states they have to be linked in the header table)
    map<string, FPNode*> children; //map to store the child nodes 
                                   //String is the item name and FPNode* is the pointer to the FPNode child

    // Constructor for FPNode
    FPNode(string i = "", FPNode* p = nullptr)
        : item(i),         // item name
          count(1),        // initial count
          parent(p),       // pointer to parent
          nodeLink(nullptr) // next node link
    {}
};

    string cleanItem(const string& item) { 
    // Function to remove anything before "#" that may be present separating the features
    size_t hashPos = item.find('#'); // Find the position of '#' in the string
    if (hashPos == string::npos) 
        return item;  // If '#' not found, return the item
    return item.substr(hashPos + 1); // If found, return substring after '#'
}

void deleteFPTree(FPNode* node) { //Function to delete FPtree from memory. 
                                  //Needed to avoid memory leaks
                                  //Delete children first, then the parent.
    if (!node) return; //If node is null, do nothing
    for (auto& child : node->children) //recursively delete all children
        deleteFPTree(child.second); //delete FPNode pointer
    delete node;    //delete the current node
}

void insertTransaction(FPNode* current, //pointer to the current node in the FPtree
                                        //Tells us where the transaction is being inserted
                       const vector<string>& transaction, //vector of items in a transaction
                       map<string, vector<FPNode*>>& headerTable, //headerTable that links each unique item to all its occurrences in the tree
                       int count = 1, //integer to count the number of transactions that occurred
                       int index = 0) //index for the current item in the transaction we are currently processing
{
    if (index >= transaction.size()) //Base case: If we already processed all items in the transaction
        return;

    string item = transaction[index]; //Get the current item in transaction and store it to item
    FPNode* nextNode;   //Declare a pointer to the next node in the tree

    if (current->children.count(item)) {  //Check to see if the current node already has a child for this item
        current->children[item]->count += count; //Increment the count of that child
        nextNode = current->children[item]; //Move to the pointer to that existing child
    } else { //Second Case: The item does not exist as a child of the current node 
             //If this is the case, we have to make a new node.
        nextNode = new FPNode(item, current); //Create a new node for this item (CurrentNode as the parent)
        nextNode->count = count; //Set the count of the node to the frequency
        current->children[item] = nextNode; //link the new child node to the current node by adding it to the children hashmap 

        if (!headerTable[item].empty()) { //check to see if the header table already has nodes for this item
                                          //if this is the case, get the last node added for this item and link it to the new node
                                          //this creates a linking between all nodes for the same item so we can find them easily later
            FPNode* last = headerTable[item].back(); 
            last->nodeLink = nextNode;
        }
        headerTable[item].push_back(nextNode); //add the new node to the header table 
    }

    insertTransaction(nextNode, transaction, headerTable, count, index + 1); //recursive call to insert the next item in the transaction
}

void printFPTree(FPNode* node, int depth = 0, bool isLast = true) { 
    // Function to print the FP-tree in a readable format
    if (depth == 0) { //Check to see if ROOT
        cout << "ROOT\n"; //if true, print ROOT
    } else {
        cout << string((depth - 1) * 6, ' ');  //Create spaces for the indentation when printing FPTree
                                               // depth - 1 so that the first level under the root has 0 spaces
                                               // *6 each level gets 6 spaces to make the Tree
                                              // Note: Can be changed to *6, *8, etc., to make tree easier to read
        // Print branch symbol
        if (isLast) {
            cout << "\\-- "; // Last child in this level
        } else {
            cout << "|-- ";  // Not the last child
        }
        // Print the item and its count
        cout << cleanItem(node->item) << " [" << node->count << "]\n";
    }
    // Iterate through children
    int index = 0;
    int numChildren = node->children.size(); // assign the total number of children for the current node to numChildren
    for (auto it = node->children.begin(); it != node->children.end(); ++it, ++index) {  //loop through all children of the current node
        bool lastChild = (index == numChildren - 1); // Check if this child is the last
        printFPTree(it->second, depth + 1, lastChild); // Recursive call for child node until all nodes in the tree are printed
    }
}

void mineFPTree(FPNode* root, map<string, vector<FPNode*>>& headerTable, int minSupport, vector<string> currentPattern = {}) { 
    // Function to recursively mine frequent patterns from the FP-tree
    // root - pointer to the root of the current FP tree
    // headerTable - links each item to all its nodes in the FP tree
    // minSupport - the minimum frequency an itemset must have to be considered frequent (also known as the threshold)
    // currentPattern - the frequent itemset we are building as we go deeper in the tree

    vector<pair<string, int>> items; //vector to store the list of items and their frequency in the current tree

    for (auto& entry : headerTable) { //Loop through each item in the header table
        int total = 0; //variable to store count of the item
        for (FPNode* n : entry.second)  // loop through each FP-tree node that contains the item
                                        // entry.second represents all the FP-tree ndoes that contain the item
            total += n->count;          // counthe number of tiems this item appears in the tree
        if (total >= minSupport)        //if the count is greater than the minimum support threshold, then it is considered frequent 
            items.push_back({entry.first, total}); //store the item name and it's total frequency in the items vector
    }
    // Sort items by support (ascending) - stated by the FPTree algorithm
    sort(items.begin(), items.end(), [](auto& a, auto& b) { return a.second < b.second; });

    for (auto& entry : items) { // loop through each frequent item 
                                // entry is a map where <string,int> are stored
        string item = entry.first; // entry.first represents the item name (for example: "Eggs")
        int support = entry.second; //entry.second represents the frequency of that item

        vector<string> newPattern = currentPattern; // Make a vector that creates of a copy of the pattern of items we currently have
        newPattern.push_back(item); // add the current item to the new pattern
    
        cout << "{ ";                   // Print the current frequent pattern
        for (const string& s : newPattern)
            cout << cleanItem(s) << " ";
        cout << "} : " << support << "\n";

        totalFrequentItemsets++; // increment count for each printed frequent pattern

        vector<pair<vector<string>, int>> conditionalPatterns; //create a vector to store all peaths leading up to the occurences of the current tiem
                                                               //The reason for this is because to find the larger patterns that contain the current item, we need to look at all the transactions that contain it
        for (FPNode* node : headerTable[item]) { //loop through every FP-tree node that contains the current item (we use the hash function for this)
            vector<string> path;        //Create a new vector to store the items along the path from the root to the current node
            int pathCount = node->count; //store the frequency of that node to pathCount
            FPNode* parent = node->parent; //start from the parent node of the current node in order to trace back to the root (we do not include the item at the ucrrent node because we only consider the items before it)
            while (parent && !parent->item.empty()) { //while there is a parent node that is not null and is not the root
                                                      // add the parents item to path and move up to the parent's parent
                path.push_back(parent->item);
                parent = parent->parent;
            }
            if (!path.empty()) {   //check to see if path is non-empty
                reverse(path.begin(), path.end()); //reverse the vector so the items go from root -> leaf
                                                   // The algorithm description in the textbook mentions we always collect from bottom->up and then construct up->bottom
                conditionalPatterns.push_back({path, pathCount}); //add this path to the conditionalPatterns vector
            }
        }

        if (!conditionalPatterns.empty()) { // check to see if the vector has any conditional patterns
            unordered_map<string, int> conditionalFreq; // hashmap to store item name and the frequency of each item in the conditionalFreq vector
            for (auto& pattern : conditionalPatterns) // loop through each item in the vector
                                                      // pattern.first is the vector of items in the path
                                                      // pattern.second is the count of that path
                for (auto& pathItem : pattern.first) // loop through each item in the path
                    conditionalFreq[pathItem] += pattern.second; //for each item in the path, add the count of to it's total in the map

            FPNode* conditionalRoot = new FPNode(); // Create a new FP tree root for the conditional tree of the current item
            map<string, vector<FPNode*>> conditionalHeader; // headerTable for the conditional tree
                                                            // LInks each item to all nodes in the conditional tree 

            for (auto& pattern : conditionalPatterns) { // loop over each conditional path 
                vector<string> filteredTransaction;  // vector that will store the frequent patterns of the condtional tree
                for (auto& pathItem : pattern.first) { // loop through each item in the current conditional path
                    if (conditionalFreq[pathItem] >= minSupport) // check to see if the frequency of the item is ATLEAST the support
                        filteredTransaction.push_back(pathItem); // if it is, add it to the filtered transaction for building the conditional FP-tree
                }
                sort(filteredTransaction.begin(), filteredTransaction.end(), // sort the filtered transcactions with respect to the frequencies
                                                                             // crucial part of the FP Growth Algorithm
                     [&](const string& a, const string& b) {
                         return conditionalFreq[a] > conditionalFreq[b];
                     });
                if (!filteredTransaction.empty()) // If it still contains at least one frequent item
                    insertTransaction(conditionalRoot, filteredTransaction, conditionalHeader, pattern.second); //Insert the filtered and sorted transacation into the conditional FP-tree
            }
            //After all conditional paths are insereted, recursively call the mineFPtree function 
            if (!conditionalHeader.empty())
                mineFPTree(conditionalRoot, conditionalHeader, minSupport, newPattern);

            deleteFPTree(conditionalRoot); //free the memory for the conditional FP-tree
        }
    }
}

int main() {
    cout << "FP-Growth Algorithm\n";
    cout << "Available Datasets:\n";
    cout << " 1. Skin Segmentation\n";
    cout << " 2. Adult\n";
    cout << " 3. Agaricus-Lepiota (Mushroom)\n";
    cout << " 4. Car Evaluation\n";
    cout << " 5. Nursery\n";
    cout << " 6. Letter Recognition\n";
    cout << " 7. King-Rook vs King-Pawn (Chess)\n";
    cout << " 8. Poker Hand Testing\n";
    cout << " 9. Pen-Based Recognition\n";
    cout << "10. Contraceptive Method Choice (CMC)\n";
    cout << "Enter dataset number (1-10): ";

    int choice;
    cin >> choice; //stores user input dataset number

    string inputFile; //string to store dataset filename
    vector<string> attributeNames; // Vector to store the attribute names
                                   // This is needed to ensure that features of different items do not get counted incorrectly
    switch (choice) { //Case statement to determine which dataset and attributeName to load
        case 1:
            inputFile = "skin_segmentation.data";
            attributeNames = {"B", "G", "R", "Y"};
            break;
        case 2:
            inputFile = "adult.data";
            attributeNames = {"age: ","workclass: ","fnlwgt: ","education: ","education-num: ","marital-status: ",
                              "occupation: ","relationship: ","race: ","sex: ","capital-gain: ","capital-loss: ",
                              "hours-per-week: ","native-country: ","income: "};
            break;
        case 3:
            inputFile = "agaricus-lepiota.data";
            attributeNames = {"poisonous", "cap-shape","cap-surface","cap-color","bruises","odor","gill-attachment",
                              "gill-spacing","gill-size","gill-color","stalk-shape","stalk-root",
                              "stalk-surface-above-ring","stalk-surface-below-ring","stalk-color-above-ring",
                              "stalk-color-below-ring","veil-type","veil-color","ring-number","ring-type",
                              "spore-print-color","population","habitat"};
            break;
        case 4:
            inputFile = "car.data";
            attributeNames = {"buying", "maint", "doors", "persons", "lug_boot", "safety", "class"};
            break;
        case 5:
            inputFile = "nursery.data";
            attributeNames = {"parents","has_nurs","form","children","housing", "finance","social","health","class"};
            break;
        case 6:
            inputFile = "letter-recognition.data";
            attributeNames = {"lettr", "x-box", "y-box","width","high","onpix","x-bar",
                            "y-bar","x2bar","y2bar","xybar","x2ybr","xy2br","x-ege",
                            "xegvy","y-ege","yegvx"};
            break;
        case 7:
            inputFile = "kr-vs-kp.data";
            attributeNames = {"bkblk","bknwy", "bkno8","bknoa","bkspr","bkxbq","bkxcr","bkxwp",
                            "blxwp","bxqsq","cntxt","dsopp","dwipd","hdchk","katri","mulch",
                            "qxmsq","r2ar8","reskd","reskr","rimmx","rkxwp","rxmsq","simpl",
                            "skach","skewr","skrxp","spcop","stlmt","thrsk","wkcti","wkna8",
                            "wknck","wkovl","wkpos","wtoeg"};
            break;
        case 8:
            inputFile = "poker-hand-testing.dat";
            attributeNames = { "S1", "C1", "S2", "C2", "S3", "C3", "S4", "C4", "S5", "C5", "CLASS"};
            break;
        case 9:
            inputFile = "pen_based.data";
            attributeNames = { "Attribute1", "Attribute2", "Attribute3", "Attribute4", "Attribute5", "Attribute6", "Attribute7", 
            "Attribute8", "Attribute9", "Attribute10", "Attribute11", "Attribute12", "Attribute13", "Attribute14", "Attribute15",
            "Attribute16", "Class"};
            break;
        case 10:
            inputFile = "cmc.data";
            attributeNames = { "wife_age", "wife_edu", "husband_edu", "num_children", "wife_religion", "wife_working", "husband_occupation",
            "standard_of_living_index", "media_exposure", "contraceptive_method"};
            break;
        default:
            cerr << "Invalid choice\n";
            return 1;
    }

    int minSupport;
    cout << "Enter minimum support: ";
    cin >> minSupport; //Stores the minimum support entered by the user
    ifstream fin(inputFile); //open the dataset file specified by the user (inputFile stores that data)
        if (!fin.is_open()) {
        cerr << "Cannot open file: " << inputFile << "\n";
        return 1;
    }

    vector<vector<string>> transactions; //Vector to store all the transacations
    string line;
    while (getline(fin, line)) {
        if (line.empty()) // If the line is empty, skip it
        continue;

    for (char &c : line) { //Replace all commas with spaces
                           //Part of the preprocessing done to ensure correct file reading
        if (c == ',') c = ' ';
    }

    vector<string> transaction; //Create an empty vector to stroe a single transacation
    stringstream ss(line);  
    string attributeValue; // represents a single value for one attribute in a transaction row
    int index = 0; 

    while (ss >> attributeValue) { //extract each attribute value from a dataset line and pair it with it's corresponding attirbute name from the variable - AttirbuteName
        if (!attributeValue.empty() && attributeValue.back() == '\r') //check if string is not empty and if lsat character is carriage return 
            attributeValue.pop_back(); //if both are true, remove that lhe last character

        if (!attributeValue.empty() && index < attributeNames.size()) //check if string is not empty and the current index of the vector is still within the bounds 
            transaction.push_back(attributeNames[index] + ":" + attributeValue); //if both are true, pair the corresponding attirbute name with it's value from the dataset 

        index++; //advance to the next index
    }

    if (!transaction.empty())
        transactions.push_back(transaction); //store the processed dataset row into the list of all transacations
    }

    fin.close(); //close the input file 

    auto startTime = high_resolution_clock::now(); //Start measuring execution time

    unordered_map<string, int> itemCount; //Hash table to store the item in the transacation and how many times it appears across all transacations
                                          //Allows us to keep track of the frequency of each item in the dataset
    for (const auto& transaction : transactions) //loop through each transactoin in all transactions
        for (const auto& item : transaction)  //loop through each item in the current transaction
            itemCount[item]++;                //count how many times each item appears in all transacations

    vector<vector<string>> filteredTransactions; //Create a new vector called filteredTransactions (this will store the transacations that are below the threshold)
    for (const auto& transaction : transactions) { //loop through each transaction in transacations
        vector<string> filtered;  //For each transcation, create a temporary filtered vector
        for (const auto& item : transaction) //loop through each item in the transaction 
            if (itemCount[item] >= minSupport) //cehck if the frequency of the item is atleast minSupport (also known as the threshold)
                filtered.push_back(item);  //If the frequency >= minSupport, add it to filtered
        sort(filtered.begin(), filtered.end(), //Sort the filtered transacations in descending order of the respective frequencies
             [&](const string& a, const string& b) {
                 return itemCount[a] > itemCount[b];
             });
        if (!filtered.empty()) 
            filteredTransactions.push_back(filtered); //Add the filtered transactions to the filteredTransactions vector
    }

    FPNode* root = new FPNode(); //Create an FP-tree root node 
    map<string, vector<FPNode*>> headerTable; //Create a header table which stores pointers to all nodes of each item in the FP tree
    for (const auto& transaction : filteredTransactions) //For each transactions in the filteredTransacation vector, insert it into the FP tree
        insertTransaction(root, transaction, headerTable); 

    cout << "\nConstructed FP-tree Structure:\n"; //Print FP-tree structure
    printFPTree(root);

    cout << "\nFrequent Itemsets Found:\n"; //Print the frequent itemsets found
    mineFPTree(root, headerTable, minSupport);

    cout << "\nTotal Frequent Itemsets Found: " << totalFrequentItemsets << "\n"; //Print the total number of frequent itemsets found

    deleteFPTree(root); //delete FP-tree from memory 

    auto endTime = high_resolution_clock::now(); //Record the endtime
    duration<double> elapsedSeconds = endTime - startTime; //compute the elapsed time since StartTime - gives us how long the algorithm took to run
    cout << "\nAlgorithm finished in " << elapsedSeconds.count() << " seconds.\n"; //Print how long the alogrithm took to run

    return 0;
}