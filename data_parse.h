#pragma once
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem> // need c++ 17

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

    // for debug purposes
    void Print(){
              std::cout << "Artist: " << artist << std::endl;
        std::cout << "Album: " << album<< std::endl;
        std::cout << "Song: " << track << std::endl;
        std::cout << "Genre: " << genre << std::endl;

        std::cout << "Duration: " << duration << std::endl;
        std::cout << "Energy :" << energy << std::endl;
        std::cout << "Speechiness: " << speechiness << std::endl;
        std::cout << "Acousticness: " << acousticness << std::endl;
        std::cout << "Instrumentalness: " << instrumentalness << std::endl;
        std::cout << "Valence: " << valence << std::endl;
        std::cout << "Tempo: " << tempo << std::endl;
    }
};

// this struct holds information about each recommended song
// it stores the track name, artist, and how similar it is to the search query (0.0 to 1.0)
struct SongResult {
    std::string trackName;
    std::string artist;
    float similarity;
    
    SongResult(const std::string& name, const std::string& art, float sim)
        : trackName(name), artist(art), similarity(sim) {}
};

/* 
Helper to associate names of songs artists with their artists and index in the larger vector for lookup by name.
Returns a map where the key is the song title and the value is a vector corresponding 
to each artist for that song and the songs index in the vector of all songs which is passed in
*/
std::unordered_map<std::string,std::vector<std::pair<std::string,int>>> getTrack_Artist(const std::vector<song_data>& d);

/* 
Helper for the loadData() function when going through each row of the csv.
Goes through the passed in row character by character to handle special names and characters.
This is necessary to correctly parse through elements in csv with , in them (which are enclosed in "")
*/
std::vector<std::string> parseRow(const std::string& line);

/* normalizes the duration and tempo for the songs in the song_data struct*/
void normalize(std::vector<song_data>& songs);

/* 
loads all data from the dataset.csv file of spotify song data 
returns a vector of all the songs data in structs.
MAIN HAS TO TAKE IN ARGC ARGV AND PASS ARGV[0] to loadData otherwise it will NOT
be able to find dataset.csv to load it
*/
std::vector<song_data> loadData(std::string exePath);
