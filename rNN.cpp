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
    return (1-normalizedD) * 100; // as a percentage
}
vector<SongResult> rNN(const std::vector<song_data>& allSongs, const song_data& search, double r){
    const double rSquare = r*r;
    vector<SongResult> results;
    for ( auto& song : allSongs){

        // skip duplicates
        if (song.track == search.track && song.artist == search.artist){
            continue;
        }
        double diffDisSquare = songDistanceSquare(song,search);
        if (diffDisSquare < rSquare){

            results.emplace_back(song.track,song.artist,getPercentSim(sqrt(diffDisSquare)));
        }
    }
    
    // could be removed if we don't want the results in sorted order by similarity
    sort(results.begin(),results.end(),[](const SongResult& s1, const SongResult& s2){return s1.similarity > s2.similarity;});
    return results;
}

/* DEBUG ONLY
int main(int argc,char* argv[]){
    auto vec = loadData(argv[0]);
    cout << vec.size() << endl;
    auto results = rNN(vec,vec[67580],0.10);
    cout << results.size();
    return 0;
}
*/