#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>



/*
Container for all relevant song data from a dataset of spotify songs
*/
struct song_data {
    // tags
    std::string artist;
    std::string album;
    std::string track;
    std::string genre;
    
    // numbers for distance
    double duration; // needs to be normalized
    double energy;
    double speechiness;
    double acousticness;
    double instrumentalness; 
    double valence;
    double tempo; // needs to be normalized

    song_data(std::vector<std::string> d):
        artist(d[2]),
        album(d[3]),
        track(d[4]),
        genre(d[20]),
        duration(stod(d[6])),
        energy(stod(d[9])),
        speechiness(stod(d[13])), 
        acousticness(stod(d[14])),
        instrumentalness(stod(d[15])),
        valence(stod(d[17])),
        tempo(stod(d[18]))
    {}
};

// this is necessary to correctly parse through elements in csv with , in them (which are enclosed in "")
std::vector<std::string> parseRow(std::string& line){
    // there will always be 21 elements max in vector so reserve in advance
    std::vector<std::string> data;
    data.reserve(21);
    bool quotes = false;
    std::string curr;
    for (char c : line){
        if (c == '"'){
            quotes = !quotes;
        }
        else if (c == ',' && !quotes){
            data.push_back(curr);
            curr.clear();
        }
        else if (c == ';'){
            curr += ", ";
        }
        else {
            if (c != '"'){
                curr += c;
            }
        }
    }
    data.push_back(curr);
    return data;
}

std::vector<song_data> loadData(){
    std::ifstream dataset("./dataset.csv");
    if (!dataset.is_open()){
        throw std::runtime_error("Failed to open file");
    }
    // main processing loop
    std::vector<song_data> data;
    std::string line;
    getline(dataset,line); // skip the header line
    while (std::getline(dataset,line)){
        std::vector<std::string> rowData = parseRow(line);
        data.emplace_back(rowData);
    }
    return data;
}



// TODO: normalize duration and tempo
//std::vector<song_data> normalize(std::vector<song_data>& songs){}

int main() {
    /* for testing when data loaded < 15 rows
    auto vec = loadData();
    for (auto sd : vec){
        std::cout << "Track info: ";
        std::cout << "Artist: " << sd.artist << std::endl;
        std::cout << "Album: " << sd.album<< std::endl;
        std::cout << "Song: " << sd.track << std::endl;
        std::cout << "Genre: " << sd.genre << std::endl;

        std::cout << "Duration: " << sd.duration << std::endl;
        std::cout << "Energy :" << sd.energy << std::endl;
        std::cout << "Speechiness: " << sd.speechiness << std::endl;
        std::cout << "Acousticness: " << sd.acousticness << std::endl;
        std::cout << "Instrumentalness: " << sd.instrumentalness << std::endl;
        std::cout << "Valence: " << sd.valence << std::endl;
        std::cout << "Tempo: " << sd.tempo << std::endl;

        std::cout << "\n";
    }*/
   
    return 0;
}