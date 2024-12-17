#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;
using namespace std;

vector<string> tokenize(const string &text) {
    vector<string> tokens;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        tokens.push_back(word);
    }
    return tokens;
}

float cosineSimilarity(const unordered_map<string, float> &vec1, const unordered_map<string, float> &vec2) {
    float dotProduct = 0.0f, magnitude1 = 0.0f, magnitude2 = 0.0f;
    for (const auto &[term, weight] : vec1) {
        if (vec2.find(term) != vec2.end()) {
            dotProduct += weight * vec2.at(term);
        }
        magnitude1 += weight * weight;
    }
    for (const auto &[term, weight] : vec2) {
        magnitude2 += weight * weight;
    }
    return (magnitude1 > 0 && magnitude2 > 0) ? (dotProduct / (sqrt(magnitude1) * sqrt(magnitude2))) : 0.0f;
}

class GeneralizedVectorModel {
private:
    string folderPath;
    unordered_map<string, unordered_map<string, float>> documentVectors; // Document vectors
    unordered_map<string, int> termFrequency;                             // Global term frequency
    unordered_set<string> documentNames;

    void processDocument(const string &docName, const string &content) {
        unordered_map<string, int> localFrequency;
        vector<string> tokens = tokenize(content);

        for (const string &token : tokens) {
            localFrequency[token]++;
            termFrequency[token]++;
        }

        float maxFrequency = 0;
        for (const auto &[term, freq] : localFrequency) {
            maxFrequency = max(maxFrequency, static_cast<float>(freq));
        }

        for (const auto &[term, freq] : localFrequency) {
            documentVectors[docName][term] = 0.5 + 0.5 * (freq / maxFrequency); // TF normalization
        }
        documentNames.insert(docName);
    }
    string stripExtension(const string &fileName) const {
        size_t lastDot = fileName.find_last_of('.');
        return (lastDot == string::npos) ? fileName : fileName.substr(0, lastDot);
    }

public:
    explicit GeneralizedVectorModel(const string &path) : folderPath(path) {}

    void indexDocuments() {
        for (const auto &entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                string fileName = entry.path().filename().string();
                ifstream file(entry.path());
                if (file.is_open()) {
                    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    processDocument(fileName, content);
                }
            }
        }
    }
    void searchByDocumentName(const string &docName) const {
        string strippedDocName = stripExtension(docName);
        for (const string &storedDocName : documentNames) {
            if (stripExtension(storedDocName) == strippedDocName) {
                cout << "Document '" << storedDocName << "' found." << endl;
                return;
            }
        }
        cout << "Document '" << docName << "' not found." << endl;
    }
    
    void searchByKeyword(const string &query) {
        unordered_map<string, float> queryVector;
        vector<string> queryTokens = tokenize(query);
        unordered_map<string, int> queryFrequency;

        for (const string &token : queryTokens) {
            queryFrequency[token]++;
        }

        for (const auto &[term, freq] : queryFrequency) {
            queryVector[term] = 0.5 + 0.5 * freq; // Simple TF
        }

        vector<pair<string, float>> rankings;

        for (const auto &[docName, vector] : documentVectors) {
            float similarity = cosineSimilarity(queryVector, vector);
            rankings.emplace_back(docName, similarity);
        }

        sort(rankings.begin(), rankings.end(), [](const auto &a, const auto &b) {
            return a.second > b.second;
        });

        cout << "Documents ranked by relevance:" << endl;
        for (const auto &[docName, similarity] : rankings) {
            if (similarity > 0) {
                cout << docName << " (Similarity: " << similarity << ")" << endl;
            }
        }
    }
};

int main() {
    string folderPath = "./";

    GeneralizedVectorModel gvm(folderPath);
    gvm.indexDocuments();

    while (true) {
        try{
            cout << "\nMenu:" << endl;
            cout << "1. Search by Document Name" << endl;
            cout << "2. Search by Keyword" << endl;
            cout << "3. Exit" << endl;

            int choice;
            cout << "Enter your choice: ";
            cin >> choice;

            cin.ignore(); // Clear input buffer
            if (choice == 1) {
                string docName;
                cout << "Enter document name: ";
                getline(cin, docName);
                gvm.searchByDocumentName(docName);
            } else if (choice == 2) {
                string keyword;
                cout << "Enter keyword: ";
                getline(cin, keyword);
                gvm.searchByKeyword(keyword);
            } else if (choice == 3) {
                cout << "Exiting program." << endl;
                break;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        catch (const exception &e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}




// #include <iostream>
// #include <fstream>
// #include <string>
// #include <unordered_map>
// #include <unordered_set>
// #include <vector>
// #include <filesystem>
// #include <sstream>
// #include <algorithm>
// #include <cmath>

// namespace fs = std::filesystem;
// using namespace std;

// // Function to tokenize and convert text to lowercase
// vector<string> tokenize(const string &text) {
//     vector<string> tokens;
//     stringstream ss(text);
//     string word;
//     while (ss >> word) {
//         transform(word.begin(), word.end(), word.begin(), ::tolower);
//         tokens.push_back(word);
//     }
//     return tokens;
// }

// // Function to calculate cosine similarity
// float cosineSimilarity(const unordered_map<string, float> &vec1, const unordered_map<string, float> &vec2) {
//     float dotProduct = 0.0f, magnitude1 = 0.0f, magnitude2 = 0.0f;
//     for (const auto &[term, weight] : vec1) {
//         if (vec2.find(term) != vec2.end()) {
//             dotProduct += weight * vec2.at(term);
//         }
//         magnitude1 += weight * weight;
//     }
//     for (const auto &[term, weight] : vec2) {
//         magnitude2 += weight * weight;
//     }
//     return (magnitude1 > 0 && magnitude2 > 0) ? (dotProduct / (sqrt(magnitude1) * sqrt(magnitude2))) : 0.0f;
// }

// // Main class for Generalized Vector Model
// class GeneralizedVectorModel {
// private:
//     string folderPath;
//     unordered_map<string, unordered_map<string, float>> documentVectors; // Document vectors
//     unordered_map<string, int> termFrequency;                             // Global term frequency
//     unordered_set<string> documentNames;

//     // Helper to process a document
//     void processDocument(const string &docName, const string &content) {
//         unordered_map<string, int> localFrequency;
//         vector<string> tokens = tokenize(content);

//         for (const string &token : tokens) {
//             localFrequency[token]++;
//             termFrequency[token]++;
//         }

//         float maxFrequency = 0;
//         for (const auto &[term, freq] : localFrequency) {
//             maxFrequency = max(maxFrequency, static_cast<float>(freq));
//         }

//         for (const auto &[term, freq] : localFrequency) {
//             documentVectors[docName][term] = 0.5 + 0.5 * (freq / maxFrequency); // TF normalization
//         }
//         documentNames.insert(docName);
//     }

// public:
//     explicit GeneralizedVectorModel(const string &path) : folderPath(path) {}

//     // Index documents in the folder
//     void indexDocuments() {
//         for (const auto &entry : fs::directory_iterator(folderPath)) {
//             if (entry.is_regular_file()) {
//                 string fileName = entry.path().filename().string();
//                 ifstream file(entry.path());
//                 if (file.is_open()) {
//                     string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
//                     processDocument(fileName, content);
//                 }
//             }
//         }
//     }

//     // Search by document name
//     void searchDocumentByName(const string& folderPath, const string& query) {
//     bool found = false;
//     for (const auto& entry : fs::directory_iterator(folderPath)) {
//         if (entry.is_regular_file()) {
//             string filename = entry.path().filename().string();
//             if (filename.find(query) != string::npos) {
//                 cout << "Found document: " << filename << endl;
//                 found = true;
//             }
//         }
//     }
//     if (!found) {
//         cout << "No documents found matching \"" << query << "\"." << endl;
//     }
// }

//     // Search by keyword
//     void searchByKeyword(const string &query) {
//         unordered_map<string, float> queryVector;
//         vector<string> queryTokens = tokenize(query);
//         unordered_map<string, int> queryFrequency;

//         for (const string &token : queryTokens) {
//             queryFrequency[token]++;
//         }

//         for (const auto &[term, freq] : queryFrequency) {
//             queryVector[term] = 0.5 + 0.5 * freq; // Simple TF
//         }

//         vector<pair<string, float>> rankings;

//         for (const auto &[docName, vector] : documentVectors) {
//             float similarity = cosineSimilarity(queryVector, vector);
//             rankings.emplace_back(docName, similarity);
//         }

//         sort(rankings.begin(), rankings.end(), [](const auto &a, const auto &b) {
//             return a.second > b.second;
//         });

//         cout << "Documents ranked by relevance:" << endl;
//         for (const auto &[docName, similarity] : rankings) {
//             if (similarity > 0) {
//                 cout << docName << " (Similarity: " << similarity << ")" << endl;
//             }
//         }
//     }
// };

// int main() {
//     string folderPath = "./";

//     GeneralizedVectorModel gvm(folderPath);
//     gvm.indexDocuments();

//     while (true) {
//         cout << "\n\nRetrieve Document" << endl;
//         cout << "1. Search by Document Name" << endl;
//         cout << "2. Search by Keyword" << endl;
//         cout << "3. Exit" << endl;
//         cout << "Enter your choice: ";

//         // string op;
//         // cin >> op;
//         int choice;
//         cin >> choice;

//         // cin.ignore(); // Clear input buffer
//         if(choice > 0 && choice < 4) {
//             // int choice = stoi(op);
//             if (choice == 1) {
//                 string docName;
//                 cout << "Enter document name: ";
//                 getline(cin, docName);
//                 gvm.searchDocumentByName(folderPath, docName);
//             } else if (choice == 2) {
//                 string keyword;
//                 cout << "Enter keyword: ";
//                 getline(cin, keyword);
//                 gvm.searchByKeyword(keyword);
//             } else if (choice == 3) {
//                 cout << "Exiting program." << endl;
//                 break;
//             } else {
//                 cout << "Invalid choice. Please try again." << endl;
//             }
//         }
//         else{
//             cout << "Invalid input. Please enter a valid integer (1-3)." << endl;
//         }
//     }

//     return 0;
// }

