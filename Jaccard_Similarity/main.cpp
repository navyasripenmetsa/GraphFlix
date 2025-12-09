#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include<unordered_set>
#include<vector>
using namespace std;
vector<string> genreList = {
    "Action","Adventure","Animation","Children","Comedy",
    "Crime","Documentary","Drama","Fantasy","Film-Noir",
    "Horror","IMAX","Musical","Mystery","Romance",
    "Sci-Fi","Thriller","War","Western"};
unordered_map<string,int> genreIndex;

bool parse_movie_line(const string &line, int &movieId, string &title, string &genres) {
    int n = line.size();
    int i = 0;
    
    int commaPos = line.find(',', i); // Parse Movie-id
    if (commaPos == string::npos) return false;
    movieId = stoi(line.substr(i, commaPos - i));
    i = commaPos + 1;

    title.clear();  // Parse Title
    if (line[i] == '"') {
        // Title is quoted
        i++; // skip opening quote
        int endQuote = line.find('"', i);
        if (endQuote == string::npos) return false;
        title = line.substr(i, endQuote - i);
        i = endQuote + 1; // move past closing quote
        if (line[i] == ',') i++; // skip comma
    } 
    else {
        // Title is NOT quoted
        int nextComma = line.find(',', i);
        if (nextComma == string::npos) return false;
        title = line.substr(i, nextComma - i);
        i = nextComma + 1;
    }

    genres = line.substr(i); // Parse genres_list

return true;
}
unordered_set<string> parse_genre_string_into_set(string raw) {
    unordered_set<string> genre_set;
    if(raw[0]=='[') {
        int i=2;
        int nxt_quote = raw.find("'",i);
        genre_set.insert(raw.substr(i,nxt_quote-i));
    }
    else {
        int i=3;
        int nxt_quote;
        while(i<raw.size()) {
            nxt_quote = raw.find("'",i);
            genre_set.insert(raw.substr(i,nxt_quote-i));
            i=nxt_quote+4;
        }
    }
return genre_set;
}
double Jaccard_Similarity(unordered_set<string>s1,unordered_set<string>s2) {
    int A[19] = {0};
    int B[19] = {0};
    // Build bit array for s1
    for (auto &g : s1) {
        if (genreIndex.count(g)) {
            int idx = genreIndex[g];
            A[idx] = 1;
        }
    }
    // Build bit array for s2
    for (auto &g : s2) {
        if (genreIndex.count(g)) {
            int idx = genreIndex[g];
            B[idx] = 1;
        }
    }
    int intersection = 0;
    int uni = 0;
    for (int i = 0; i < 19; i++) {
        if (A[i] == 1 || B[i] == 1) {
            uni++;
        }
        if (A[i] == 1 && B[i] == 1) {
            intersection++;
        }
    }
    if (uni == 0) return 0.0;
    return ((double)intersection)/((double)uni);
}

int main() {
    ifstream file("GraphFlix_Dataset/movies_clean.csv");
    if (!file.is_open()) {
    cout << "Error opening file\n";
    return 1;
    }

    for (int i = 0; i < genreList.size(); i++) {
    genreIndex[genreList[i]] = i;
    }
    unordered_map<int, string> movieTitles;
    unordered_map<int, string> rawGenreStrings;
    unordered_map<int, unordered_set<string>> movieGenres;

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        int id;
        string title, genres;
        if (!parse_movie_line(line, id, title, genres)) {
            cout << "Failed to parse: " << line << endl;
            continue;
        }
        movieTitles[id] = title;
        rawGenreStrings[id] = genres;
    }
    file.close();

    for (auto &p : rawGenreStrings) {
        int id = p.first;
        string raw = p.second;
        movieGenres[id] = parse_genre_string_into_set(raw);
    }

    int m1,m2;
    cout<<"Enter First Movie ID : ";
    cin>>m1;
    cout<<"Enter Second Movie ID : ";
    cin>>m2;
    double similarity = Jaccard_Similarity(movieGenres[m1],movieGenres[m2]);
    cout << "Jaccard similarity between "<<movieTitles[m1] << "  AND  " << movieTitles[m2]<<" : "<<similarity<<"\n";
}