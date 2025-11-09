#include "rNN.h"
using namespace std;


double songDistanceSquare(const song_data& s1, const song_data& s2){
    return ( 
            ((s2.duration - s1.duration) * (s2.duration - s1.duration)) + 
            ((s2.energy - s1.energy) * (s2.energy - s1.energy)) + 
            ((s2.speechiness - s1.speechiness) * (s2.speechiness - s1.speechiness)) + 
            ((s2.acousticness - s1.acousticness) * (s2.acousticness - s1.acousticness)) + 
            ((s2.instrumentalness - s1.instrumentalness) * (s2.instrumentalness- s1.instrumentalness)) + 
            ((s2.valence - s1.valence) * (s2.valence- s1.valence)) + 
            ((s2.tempo - s1.tempo) * (s2.tempo - s1.tempo))
            );
}

double getPercentSim(double distance){
    
    /* 
    normalize distance
    max values for every thing are 1 so if all 7 categories r 1, max distance is sqrt(7)
    actual distance/max distance = normalized distance using min max normalization
    min = 0 (if it's the same song)
    max = sqrt(7) from above
    normalized = (value-min)/(max-min)
    but min is 0 so we get value/max as the normalized num
    */
    double normalizedD = distance/MAX_DIST;
    return (1-normalizedD); // as a percentage
}
vector<SongResult> rNN(const std::vector<song_data>& allSongs, const song_data& search, double r){
    const double rSquare = r*r;
    vector<SongResult> results;
    unordered_map<double,pair<string,string>> dupes;
    unordered_set<string> dupeNames;
    for ( auto& song : allSongs){

        // skip same track as search
        if (song.track == search.track && song.artist == search.artist){
            continue;
        }
        
        // skip if we've already added this song name to results
        if (seenSongs.find(song.track) != seenSongs.end()){
            continue;
        }
        
        double diffDisSquare = songDistanceSquare(song,search);
        if (diffDisSquare < rSquare){
            auto check = dupes.find(diffDisSquare);
            auto nameCheck = dupeNames.find(song.track);
            if (check != dupes.end() && check->second == make_pair(song.track,song.artist) || nameCheck != dupeNames.end()){ // skip duplicate results
                continue;
            }
            else {
                results.emplace_back(song.track,song.artist,getPercentSim(sqrt(diffDisSquare)));
                dupes[diffDisSquare] = make_pair(song.track,song.artist);
                dupeNames.insert(song.track);
            }
        }
    }
    
    // could be removed if we don't want the results in sorted order by similarity
    std::sort(results.begin(),results.end(),[](const SongResult& s1, const SongResult& s2){return s1.similarity > s2.similarity;});
    vector<SongResult> toRe;
    if (results.size() < 10){
        cout << "Less than 10 matches found";
    }
    else {
        for (int i = 0; i < 10; i++){
            cout << results[i].similarity;
            toRe.push_back(results[i]);
        }
    }
    return toRe;
}

/* DEBUG ONLY
int main(int argc, char* argv[]){
    auto vec = loadData(argv[0]);
    vec[10001].Print();
    auto results = rNN(vec,vec[101],0.105);
    cout << "Size of Results: " << results.size() << endl;
    for (int i = 0; i < 10; i++){
       cout << "Song: " << results[i].trackName 
            << " Artist: " << results[i].artist
            << " Similarity: " << results[i].similarity << endl;
    }
    return 0;
}
*/