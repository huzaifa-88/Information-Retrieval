#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <filesystem>

using namespace std;

namespace fs = filesystem;

// function to filter the query
string filter_string(const string& text){
    string filter;
    for(char ch : text){
        if(isalnum(ch) || isspace(ch)){
            filter += tolower(ch);
        }
    }
    return filter;
}

vector<string> split_string(const string& text){
    vector<string> keywords;
    stringstream ss(text);
    string word;
    while(ss >> word){
        keywords.push_back(word);
    }
    return keywords;
}

string read_file_content(const string& file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_path << endl;
        return "";
    }
    
    stringstream buffer;
    buffer << file.rdbuf();  // Read the whole file content into a stringstream
    return buffer.str();
}

vector<pair<string, int>> query_documents(const string& query, const vector<string>& filePaths){
    vector<string> query_keywords = split_string(filter_string(query));
    vector<pair<string, int>> doc_scores;

    for(const auto& filePath : filePaths){
        string doc_content = read_file_content(filePath);
        unordered_set<string> doc_keywords;
        vector<string> doc_keyword_list = split_string(filter_string(doc_content));  // Split and clean once
        doc_keywords.insert(doc_keyword_list.begin(), doc_keyword_list.end());  // Insert into unordered_set

        // unordered_set<string> doc_keywords(split_string(filter_string(doc_content)).begin(), split_string(filter_string(doc_content)).end());
        int match_count = 0;
        // Now Count the number of query keywords that appear in the document
        for(const string& keyword : query_keywords){
            if(doc_keywords.find(keyword) != doc_keywords.end()){
                match_count++;
            }
        }
        string file_name = fs::path(filePath).filename().string();
        doc_scores.push_back({file_name, match_count});
    }
    
    sort(doc_scores.begin(), doc_scores.end(), [](const pair<string, int>& a, const pair<string, int>& b){
        return b.second < a.second;
    });

    return doc_scores;

}

void display_Ranked_DOCS(const vector<pair<string, int>>& ranked_docs){
    cout << "Ranked Documents based on keyword matching:\n" << endl;
    for (const auto& [title, score] : ranked_docs) {
        if (score > 0) {
            cout << title << " (Matched Keywords: " << score << ")" << endl;
            string doc_content = read_file_content(title);
            cout<<"Content: " << doc_content <<endl;
        } 
        else {
            cout << title << ": No matching keywords." << endl;
        }
        cout << endl;
    }
    // if (ranked_docs.empty()) {
    //     cout << "No documents match your query." << endl;
    //     return;
    // }

    // cout << "Ranked Documents based on keyword matching:\n" << endl;
    // for (const auto& [title, score] : ranked_docs) {
    //     cout << title << " (Matched Keywords: " << score << ")" << endl;
    //     cout << endl;
    // }
}

int main()
{
    string folder_Path = "./";
    
    vector<string> filePaths;
    for(const auto& entry : fs::directory_iterator(folder_Path)){
        if(entry.path().extension() == ".txt"){
            filePaths.push_back(entry.path().string());
        }
    }

    if(filePaths.empty()){
        cout<<"No text files found in the current folder!"<<endl;
        return 1;
    }

    string user_query;
    cout<<"Enter Your Query:";
    getline(cin, user_query);
    vector<pair<string, int>> ranked_documents = query_documents(user_query, filePaths);
    display_Ranked_DOCS(ranked_documents);

    return 0;
}