#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;
namespace fs = std::filesystem;

// Function to tokenize a string into lowercase alphanumeric words
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    string word = "";
    for (char ch : text) {
        if (isalnum(ch)) {
            word += tolower(ch);
        } else if (!word.empty()) {
            tokens.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) tokens.push_back(word);
    return tokens;
}

// Function to calculate the Jaccard similarity between two sets
double calculateJaccard(const set<string>& docSet, const set<string>& querySet) {
    set<string> intersectionSet, unionSet;
    set_intersection(docSet.begin(), docSet.end(), querySet.begin(), querySet.end(), inserter(intersectionSet, intersectionSet.begin()));
    set_union(docSet.begin(), docSet.end(), querySet.begin(), querySet.end(), inserter(unionSet, unionSet.begin()));
    return static_cast<double>(intersectionSet.size()) / unionSet.size();
}

// BIM Retrieval Model
vector<pair<string, double>> retrieveDocumentsBIM(const vector<pair<string, string>>& documents, const string& query) {
    vector<pair<string, double>> scores;

    // Tokenize the query
    vector<string> queryTokens = tokenize(query);
    set<string> querySet(queryTokens.begin(), queryTokens.end());

    for (const auto& [fileName, content] : documents) {
        // Tokenize the document content
        vector<string> docTokens = tokenize(content);
        set<string> docSet(docTokens.begin(), docTokens.end());

        // Calculate Jaccard similarity
        double score = calculateJaccard(docSet, querySet);
        scores.emplace_back(fileName, score);
    }

    // Sort based on scores in descending order
    sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    return scores;
}

// Non-Overlapped List Model
vector<string> retrieveNonOverlapping(const vector<pair<string, string>>& documents, const vector<string>& terms) {
    vector<string> fileNames;
    unordered_set<string> fileNameSet;

    for (const auto& term : terms) {
        for (const auto& [fileName, content] : documents) {
            if (content.find(term) != string::npos) {
                fileNameSet.insert(fileName);
            }
        }
    }

    fileNames.assign(fileNameSet.begin(), fileNameSet.end());
    return fileNames;
}

// Function to build a graph from the document
unordered_map<string, vector<string>> buildGraph(const string& content) {
    unordered_map<string, vector<string>> graph;
    vector<string> tokens = tokenize(content);

    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = i + 1; j < tokens.size() && j <= i + 2; ++j) {  // Proximity of 2 words
            graph[tokens[i]].push_back(tokens[j]);
            graph[tokens[j]].push_back(tokens[i]);
        }
    }
    return graph;
}

// Function to explore the graph and retrieve documents connected to query nodes
bool isConnectedToQuery(const unordered_map<string, vector<string>>& graph, const vector<string>& queryTokens) {
    unordered_set<string> visited;
    queue<string> q;

    // Add query tokens to the queue
    for (const string& token : queryTokens) {
        if (graph.find(token) != graph.end()) {
            q.push(token);
            visited.insert(token);
        }
    }

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        for (const string& neighbor : graph.at(current)) {
            if (visited.find(neighbor) == visited.end()) {  // Not visited
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    // If any query token connects to other nodes in the graph, return true
    return !visited.empty();
}

// Function to retrieve documents using the Proximal Nodes Model
vector<string> retrieveProximalNodes(const vector<pair<string, string>>& documents, const vector<string>& queryTokens) {
    vector<string> relevantDocuments;

    for (const auto& [fileName, content] : documents) {
        auto graph = buildGraph(content);

        if (isConnectedToQuery(graph, queryTokens)) {
            relevantDocuments.push_back(fileName);
        }
    }

    return relevantDocuments;
}

// Function to read all text files in the given folder
vector<pair<string, string>> readDocumentsFromFolder(const string& folderPath) {
    vector<pair<string, string>> documents;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            ifstream file(entry.path());
            if (file) {
                string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                documents.emplace_back(entry.path().filename().string(), content);
            }
        }
    }
    return documents;
}

int main() {
    string folderPath = "./";  // Current folder
    vector<pair<string, string>> documents = readDocumentsFromFolder(folderPath);

    if (documents.empty()) {
        cout << "No text documents found in the folder.\n";
        return 1;
    }

    // User selects the model
    int modelChoice;
    while(true){
        cout << "---------------- Available Models ----------------\n";
        cout << "1. Binary Independence Model (BIM)\n";
        cout << "2. Non-Overlapped List Model\n";
        cout << "3. Proximal Nodes Model\n";
        cout << "4. Exit\n";
        cout << "Enter your choice (1-4): ";
        cin >> modelChoice;
        cin.ignore();  // To discard the newline character after the integer input
        if (modelChoice == 1) {
            // User input for BIM model
            string query;
            cout << "Enter your query for BIM model: ";
            getline(cin, query);

            // BIM Results
            cout << "BIM Results:\n";
            vector<pair<string, double>> bimResults = retrieveDocumentsBIM(documents, query);
            bool found = false;  // Flag to track if any relevant documents are found
            for (const auto& [fileName, score] : bimResults) {
                if (score > 0.0) {  // Only show documents with a positive score
                    cout << "File Name: " << fileName << ", Score: " << score << "\n";
                    found = true;
                }
            }

            if (!found) {  // If no relevant documents were found
                cout << "No relevant documents found.\n";
            }
        }
        else if (modelChoice == 2) {
            // User input for Non-Overlapped List Model
            int numTerms;
            cout << "Enter the number of query terms for Non-Overlapped List Model: ";
            cin >> numTerms;
            cin.ignore();  // To discard the newline character after the integer input

            vector<string> terms(numTerms);
            cout << "Enter the query terms:\n";
            for (int i = 0; i < numTerms; ++i) {
                getline(cin, terms[i]);
            }

            // Non-Overlapped List Model Results
            cout << "Non-Overlapped List Model Results:\n";
            vector<string> nonOverlappedResults = retrieveNonOverlapping(documents, terms);
            if (nonOverlappedResults.empty()) {
                cout << "No relevant documents found.\n";
            } else {
                for (const string& fileName : nonOverlappedResults) {
                    cout << "File Name: " << fileName << "\n";
                }
            }            
        }
        else if (modelChoice == 3) {
            // User input for Proximal Nodes Model
            string query;
            cout << "Enter your query: ";
            getline(cin, query);

            // Tokenize query
            vector<string> queryTokens = tokenize(query);

            // Retrieve documents using Proximal Nodes Model
            cout << "Proximal Nodes Model Results:\n";
            vector<string> results = retrieveProximalNodes(documents, queryTokens);
            if(results.empty()){
                cout << "No relevant documents found.\n";
            }
            else{
                for (const string& fileName : results) {
                    cout << "File Name: " << fileName << "\n";
                }
            }
        } 
        else if (modelChoice == 4) {
            break;
        }
        else {
            cout << "Invalid choice. Currently, only these Models are implemented.\n";
        }        
    }
    
    return 0;
}