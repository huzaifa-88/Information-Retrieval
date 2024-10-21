#include <iostream>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

struct Document{
    string name;   //name of document in which word is present
    vector<int> position;    //position of that word
};

vector<string> readDocs(const string& filePath){
    vector<string> words;
    ifstream file(filePath);
    string word;
    while(file >> word){
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
            cout << "Document: " << doc.name << ", Position(s): ";
            for (int pos : doc.position) {
                cout << pos << " ";
            }
            cout << "\n";
        }
    }
    else{
        cout << "No result found for \"" << query << "\".\n";
    }
}

int main()
{
    string folder_Path = ".";
    unordered_map<string, vector<Document>> index = buildingIndex(folder_Path);
    string query;
    cout << "Enter a word: ";
    cin >> query;

    wordSearching(index, query);
    
    return 0;
}