// this is for the radius nearest neighbors algorithm
#include <cmath> // for sqrt
#include "data_parse.h"

// does this here so it is not recalculted for every iteration
const double MAX_DIST = sqrt(7);

/* 
the actual implementation of the radius nearest neighbors algorithm
compares the squared distances between songs initially for efficiency
if the squared distance is within r^2 than it performs the sqrt to find the actual distance
*/
std::vector<SongResult> rNN(const std::vector<song_data>& allSongs, const song_data& search, double r);

// helper to calculate the similarity percentages
double getPercentSim(double distance);

// helper to calculate the distances between songs
double songDistanceSquare(const song_data& s1, const song_data& s2);