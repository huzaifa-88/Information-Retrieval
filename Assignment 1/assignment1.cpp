#include <iostream>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = filesystem;

struct Document{
    string doc_name;
    vector<int> word_position;
};

vector<string> readDocs(const string& filePath){
    vector<string> words;
    ifstream file(filePath);
    string word;
    while(file >> word){
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        words.push_back(word);
    }
    return words;
}

unordered_map<string, vector<Document>> buildingIndex(const string& folderPath){
    unordered_map<string, vector<Document>> index;

    for (const auto& entry : fs::directory_iterator(folderPath) ){
        string filePath = entry.path().string();
        vector<string> words = readDocs(filePath);

        for(size_t i = 0; i<words.size(); i++){
            string word = words[i];
            index[word].push_back({filePath, {static_cast<int>(i)}});
        }
    }

    return index;
}

void wordSearching(const unordered_map<string, vector<Document>>& index, const string& query){
    auto it = index.find(query);
    if(it != index.end()){
        const vector<Document>& documents = it->second;
        cout << "Found the word \"" << query << "\" in the following documents:\n";
        for(const auto& doc : documents){
            cout << "Document: " << doc.doc_name << ", Position(s): ";
            for (int pos : doc.word_position) {
                cout << pos << " ";
            }
            cout << "\n";
        }
    }
    else{
        cout << "No result found for \"" << query << "\".\n";
    }
}


vector<string> listDocuments(const string& folderPath) {
    vector<string> documents;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            documents.push_back(entry.path().filename().string());
        }
    }
    return documents;
}

void searchDocumentByName(const string& folderPath, const string& query) {
    bool found = false;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (filename.find(query) != string::npos) {
                cout << "Found document: " << filename << endl;
                found = true;
            }
        }
    }
    if (!found) {
        cout << "No documents found matching \"" << query << "\"." << endl;
    }
}
bool isInteger(const string& str) {
    for (char c : str) {
        if (!::isdigit(c)) {
            return false;
        }
    }
    return true;
}


int main()
{
    string folder_Path = ".";
    unordered_map<string, vector<Document>> index = buildingIndex(folder_Path);

    try{

        while (true) {
            cout << "\nChoose an option:\n";
            cout << "1. Search for a word\n";
            cout << "2. List all documents\n";
            cout << "3. Search for a document by name\n";
            cout << "4. Exit\n";
            cout << "Enter your choice (1-4): ";
            
            string op;
            cin >> op;
            if(isInteger(op)) {
                int choice = stoi(op);
                if (choice == 1) {
                    string query;
                    cout << "Enter a word to search: ";
                    cin >> query;
                    wordSearching(index, query);
                }
                else if (choice == 2) {
                    vector<string> documents = listDocuments(folder_Path);
                    cout << "Documents in the folder:\n";
                    for (const auto& doc : documents) {
                        cout << doc << endl;
                    }
                }
                else if (choice == 3) {
                    string query;
                    cout << "Enter a document name to search: ";
                    cin.ignore(); // Clear the input buffer
                    getline(cin, query);
                    searchDocumentByName(folder_Path, query);
                }
                else if (choice == 4) {
                    cout << "Exiting the program.\n";
                    break;
                }
                else {
                    cout << "Invalid choice. Please try again.\n";
                } 
            }
            else{
                cout << "Invalid choice. Please enter a number between 1 and 4.\n";
                continue;
            }
        }
    }
    catch(exception e){
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}