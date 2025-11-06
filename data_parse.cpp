#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>

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

// helper to associate names of songs artists with their artists and index in the larger vector for lookup by name
std::unordered_map<std::string,std::vector<std::pair<std::string,int>>> getTrack_Artist(const std::vector<song_data>& d){
    std::unordered_map<std::string,std::vector<std::pair<std::string,int>>> ret;
    ret.reserve(d.size());
    for (int i = 0; i < d.size(); i++){
        ret[d[i].track].emplace_back(std::make_pair(d[i].artist,i));
    }
    return ret;
}

// this is necessary to correctly parse through elements in csv with , in them (which are enclosed in "")
std::vector<std::string> parseRow(const std::string& line){
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

void normalize(std::vector<song_data>& songs){
    if (songs.empty()){
        return;
    }

    // find the minimum and maximum values of the duration and tempo

    auto minmaxDur = std::minmax_element(songs.begin(),songs.end(),[](const song_data& a, const song_data& b){return a.duration < b.duration;});

    auto minmaxTempo = std::minmax_element(songs.begin(),songs.end(),[](const song_data& a, const song_data& b){return a.tempo < b.tempo;});
    
    // store these so they won't be modified as we go through the data
    double durRange = minmaxDur.second->duration - minmaxDur.first->duration;
    double tempoRange = minmaxTempo.second->tempo - minmaxTempo.first->tempo;

    double durMin = minmaxDur.first->duration;
    double tempoMin = minmaxTempo.first->tempo;

    // currNormalized = (curr-min)/(max-min) 
    // normalize all values
    for (auto& song : songs){
        song.duration = (durRange == 0) ? 0 : (song.duration - durMin) / durRange;
        song.tempo = (tempoRange == 0) ? 0 : (song.tempo - tempoMin) / tempoRange;
    }
}

std::vector<song_data> loadData(){
    std::ifstream dataset("./dataset.csv");
    if (!dataset.is_open()){
        throw std::runtime_error("Failed to open file");
    }
    // main processing loop
    std::vector<song_data> data;
    data.reserve(100000);
    std::string line;
    getline(dataset,line); // skip the header line
    while (std::getline(dataset,line)){
        std::vector<std::string> rowData = parseRow(line);
        data.emplace_back(rowData);
    }
    normalize(data);
    return data;
}

int main() {
    /*for testing when data loaded is limited enough to print debug
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