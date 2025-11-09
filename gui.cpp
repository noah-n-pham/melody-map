#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include "data_parse.h"
#include "rNN.h"
using namespace std;

// helper function to find euclidean distance between two songs
double calculateDistance(const song_data& a, const song_data& b) {
    double sum = 0.0;
    sum += (a.duration - b.duration) * (a.duration - b.duration);
    sum += (a.energy - b.energy) * (a.energy - b.energy);
    sum += (a.speechiness - b.speechiness) * (a.speechiness - b.speechiness);
    sum += (a.acousticness - b.acousticness) * (a.acousticness - b.acousticness);
    sum += (a.instrumentalness - b.instrumentalness) * (a.instrumentalness - b.instrumentalness);
    sum += (a.valence - b.valence) * (a.valence - b.valence);
    sum += (a.tempo - b.tempo) * (a.tempo - b.tempo);
    return sqrt(sum);
}

// helper function to find the index of a song given its name and artist
// returns -1 if not found
int findSongIndex(const string& songName, const string& artistName,
                  const unordered_map<string, vector<pair<string, int>>>& trackArtistMap) {
    // try to find the song
    auto it = trackArtistMap.find(songName);
    if (it == trackArtistMap.end()) {
        return -1; // song not found
    }
    
    // if artist name is provided, search for exact match
    if (!artistName.empty()) {
        for (const auto& artistPair : it->second) {
            if (artistPair.first == artistName) {
                return artistPair.second; // found exact match!
            }
        }
        // artist not found for this song
        cout << "Artist '" << artistName << "' not found for song '" << songName 
             << "', using first match instead." << endl;
    }
    
    // if no artist specified or no match found, return first version
    return it->second[0].second;
}

// khoi will implement the K-Nearest Neighbors algorithm here
vector<SongResult> kNearestNeighbors(int k, int index,
                                     const vector<song_data>& allSongs
                                    ){
    song_data querySong = allSongs[index];
    cout << "Found song: " << querySong.track << " by " << querySong.artist << endl;
    
    // loop through every song and calculate how far it is from the query song
    vector<pair<double, int>> distances;
    for (int i = 0; i < allSongs.size(); i++) {
        // skip the query song itself (including any duplicate entries with same name)
        if (i == index || allSongs[i].track == querySong.track) continue;
        double dist = calculateDistance(querySong, allSongs[i]);
        distances.push_back(make_pair(dist, i));
    }
    
    // sort all the distances from smallest to largest to find the nearest neighbors
    sort(distances.begin(), distances.end());
    
    // take the k nearest songs and convert them to SongResult format so it is ready to display
    vector<SongResult> results;
    for (int i = 0; i < k && i < distances.size(); i++) {
        float similarity = 1.0 / (1.0 + distances[i].first);
        results.push_back(SongResult(
            allSongs[distances[i].second].track,
            allSongs[distances[i].second].artist,
            similarity
        ));
    }
    
    return results;
}


// marcelo will implement the radius nearest neighbors algorithm
vector<SongResult> radiusNearestNeighbors(const string& songName, const string& artistName, int k,
                                          const vector<song_data>& allSongs,
                                          const unordered_map<string, vector<pair<string, int>>>& trackArtistMap);

class MelodyMapUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    vector<song_data> allSongs;
    unordered_map<string, vector<pair<string, int>>> trackArtistMap;
    
    // main ui boxes
    sf::RectangleShape searchBox;
    sf::RectangleShape dropdownBox;
    sf::RectangleShape searchButton;
    sf::RectangleShape resultsPanel;
    
    // autocomplete stuff
    sf::RectangleShape suggestionsBox;
    vector<sf::RectangleShape> suggestionBoxes;
    vector<sf::Text> suggestionTexts;
    
    // text labels
    sf::Text titleText;
    sf::Text searchLabel;
    sf::Text inputText;
    sf::Text algorithmLabel;
    sf::Text dropdownText;
    sf::Text buttonText;
    vector<sf::Text> resultTexts;
    
    // state variables
    string userInput;
    string selectedSongName;      
    string selectedArtistName;    
    string selectedAlgorithm;
    bool dropdownOpen;
    bool isSearching;
    bool searchBoxFocused;
    vector<SongResult> results;
    sf::Clock cursorClock;
    bool showCursor;
    pair<string, string> searchResults;

    // autocomplete state
    bool showSuggestions;
    vector<pair<string, string>> currentSuggestions;
    int selectedSuggestionIndex;
    const int MAX_SUGGESTIONS = 8;
    
    const unsigned int WINDOW_WIDTH = 1000;
    const unsigned int WINDOW_HEIGHT = 800;
    
    // used to prevent double-clicking on suggestions
    sf::Clock clickClock;
    const float CLICK_DELAY = 0.15f;

    
public:
    MelodyMapUI(const string& exePath) : 
        titleText(font),
        searchLabel(font),
        inputText(font),
        algorithmLabel(font),
        dropdownText(font),
        buttonText(font) {
        
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Melody Map - Song Recommender");
        window.setFramerateLimit(60);
        
        // load all the songs from the csv
        cout << "Loading Spotify dataset..." << endl;
        try {
            allSongs = loadData(exePath);
            trackArtistMap = getTrack_Artist(allSongs);
            cout << "Successfully loaded " << allSongs.size() << " songs!" << endl;
        } catch (const exception& e) {
            cerr << "ERROR: Failed to load dataset! " << e.what() << endl;
        }
        
        // try to find a font that works
        bool fontLoaded = false;
        vector<string> fontPaths = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",  // macOS Arial
            "/System/Library/Fonts/Helvetica.ttc",           // macOS Helvetica fallback
            "C:/Windows/Fonts/arial.ttf",                    // Windows Arial
        };
        
        for (const auto& path : fontPaths) {
            if (font.openFromFile(path)) {
                fontLoaded = true;
                cout << "Successfully loaded font from: " << path << endl;
                break;
            }
        }
        
        if (!fontLoaded) {
            cerr << "ERROR: Could not load any font!" << endl;
        }
        
        // set default values
        selectedAlgorithm = "K-Nearest Neighbors";
        dropdownOpen = false;
        isSearching = false;
        searchBoxFocused = true;
        showCursor = true;
        showSuggestions = false;
        selectedSuggestionIndex = -1;
        
        initializeUI();
    }
    
    // set up all the visual elements
    void initializeUI() {
        titleText.setString("Melody Map");
        titleText.setCharacterSize(48u);
        titleText.setFillColor(sf::Color(30u, 215u, 96u));
        titleText.setPosition({static_cast<float>(WINDOW_WIDTH) / 2.f - 150.f, 30.f});
        
        searchLabel.setString("Enter Song Name:");
        searchLabel.setCharacterSize(20u);
        searchLabel.setFillColor(sf::Color::White);
        searchLabel.setPosition({50.f, 120.f});
        
        searchBox.setSize({500.f, 40.f});
        searchBox.setPosition({50.f, 150.f});
        searchBox.setFillColor(sf::Color(50u, 50u, 50u));
        searchBox.setOutlineColor(sf::Color(100u, 100u, 100u));
        searchBox.setOutlineThickness(2.f);
        
        inputText.setCharacterSize(18u);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition({60.f, 158.f});
        
        algorithmLabel.setString("Select Algorithm:");
        algorithmLabel.setCharacterSize(20u);
        algorithmLabel.setFillColor(sf::Color::White);
        algorithmLabel.setPosition({600.f, 120.f});
        
        dropdownBox.setSize({300.f, 40.f});
        dropdownBox.setPosition({600.f, 150.f});
        dropdownBox.setFillColor(sf::Color(50u, 50u, 50u));
        dropdownBox.setOutlineColor(sf::Color(100u, 100u, 100u));
        dropdownBox.setOutlineThickness(2.f);
        
        dropdownText.setString(selectedAlgorithm);
        dropdownText.setCharacterSize(18u);
        dropdownText.setFillColor(sf::Color::White);
        dropdownText.setPosition({610.f, 158.f});
        
        searchButton.setSize({150.f, 50.f});
        searchButton.setPosition({static_cast<float>(WINDOW_WIDTH) / 2.f - 75.f, 220.f});
        searchButton.setFillColor(sf::Color(30u, 215u, 96u));
        
        buttonText.setString("Search");
        buttonText.setCharacterSize(24u);
        buttonText.setFillColor(sf::Color::Black);
        buttonText.setPosition({static_cast<float>(WINDOW_WIDTH) / 2.f - 40.f, 230.f});
        
        resultsPanel.setSize({900.f, 450.f});
        resultsPanel.setPosition({50.f, 300.f});
        resultsPanel.setFillColor(sf::Color(30u, 30u, 30u));
        resultsPanel.setOutlineColor(sf::Color(100u, 100u, 100u));
        resultsPanel.setOutlineThickness(2.f);
        
        suggestionsBox.setFillColor(sf::Color(40u, 40u, 40u));
        suggestionsBox.setOutlineColor(sf::Color(100u, 100u, 100u));
        suggestionsBox.setOutlineThickness(2.f);
    }
    
    // figure out what songs match what the user typed
    void updateSuggestions() {
        currentSuggestions.clear();
        selectedSuggestionIndex = -1;
        
        // need at least 2 characters before we start suggesting
        if (userInput.empty() || userInput.length() < 2) {
            showSuggestions = false;
            return;
        }
        
        // make everything lowercase so we can search better
        string lowerInput = userInput;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        
        // go through all the songs and find matches
        for (const auto& [trackName, artistList] : trackArtistMap) {
            string lowerTrack = trackName;
            transform(lowerTrack.begin(), lowerTrack.end(), lowerTrack.begin(), ::tolower);
            
            // if the song name has what we typed in it, add it to suggestions
            if (lowerTrack.find(lowerInput) != string::npos) {
                // UPDATED: Add all versions of the song (different artists)
                for (const auto& artistPair : artistList) {
                    currentSuggestions.push_back({trackName, artistPair.first});
                    
                    if (currentSuggestions.size() >= MAX_SUGGESTIONS) break;
                }
                
                if (currentSuggestions.size() >= MAX_SUGGESTIONS) break;
            }
        }
        
        showSuggestions = !currentSuggestions.empty();
    }
    
    // when someone picks a suggestion, fill it into the search box
    void selectSuggestion(int index) {
        if (index >= 0 && index < currentSuggestions.size()) {
            // UPDATED: Store both song name and artist
            selectedSongName = currentSuggestions[index].first;
            selectedArtistName = currentSuggestions[index].second;
            userInput = selectedSongName + " - " + selectedArtistName;
            searchResults = make_pair(currentSuggestions[index].first, currentSuggestions[index].second);
            inputText.setString(userInput);
            showSuggestions = false;
            selectedSuggestionIndex = -1;
        }
    }
    
    // handle typing, clicking, arrow keys, etc
    void handleInput(const sf::Event& event) {
        // when someone types something
        if (event.is<sf::Event::TextEntered>() && searchBoxFocused && !dropdownOpen) {
            const auto& textEvent = *event.getIf<sf::Event::TextEntered>();
            
            if (textEvent.unicode == 8 && !userInput.empty()) {
                // backspace - delete last character
                userInput.pop_back();
                inputText.setString(userInput);
                updateSuggestions();
            } else if (textEvent.unicode == 13) {
                // enter key - either pick a suggestion or search
                if (showSuggestions && selectedSuggestionIndex >= 0) {
                    selectSuggestion(selectedSuggestionIndex);
                } else if (!userInput.empty()) {
                    performSearch(trackArtistMap);
                }
            } else if (textEvent.unicode >= 32 && textEvent.unicode < 128) {
                // regular character like a letter or number
                userInput += static_cast<char>(textEvent.unicode);
                inputText.setString(userInput);
                updateSuggestions();
            }
        }
        
        // arrow keys to navigate suggestions
        if (event.is<sf::Event::KeyPressed>() && showSuggestions && searchBoxFocused) {
            const auto& keyEvent = *event.getIf<sf::Event::KeyPressed>();
            
            if (keyEvent.code == sf::Keyboard::Key::Down) {
                selectedSuggestionIndex = min(selectedSuggestionIndex + 1, 
                                             static_cast<int>(currentSuggestions.size()) - 1);
            } else if (keyEvent.code == sf::Keyboard::Key::Up) {
                selectedSuggestionIndex = max(selectedSuggestionIndex - 1, -1);
            } else if (keyEvent.code == sf::Keyboard::Key::Escape) {
                // escape key closes suggestions
                showSuggestions = false;
                selectedSuggestionIndex = -1;
            }
        }
        
        // mouse clicks
        if (event.is<sf::Event::MouseButtonPressed>()) {
            // only process if enough time has passed since last click
            if (clickClock.getElapsedTime().asSeconds() < CLICK_DELAY) {
                return;
            }
            
            sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
            bool clickedAnywhere = false;
            
            // did they click the search box?
            if (searchBox.getGlobalBounds().contains(mousePos)) {
                searchBoxFocused = true;
                if (!userInput.empty()) {
                    updateSuggestions();
                }
                clickedAnywhere = true;
            }
            
            // did they click the algorithm dropdown?
            if (dropdownBox.getGlobalBounds().contains(mousePos)) {
                dropdownOpen = !dropdownOpen;
                showSuggestions = false;
                clickedAnywhere = true;
            }
            
            // did they click the search button?
            if (searchButton.getGlobalBounds().contains(mousePos) && !userInput.empty()) {
                performSearch(trackArtistMap);
                clickedAnywhere = true;
            }
            
            // did they click on one of the suggestions?
            if (showSuggestions) {
                bool clickedSuggestion = false;
                for (size_t i = 0; i < suggestionBoxes.size(); ++i) {
                    if (suggestionBoxes[i].getGlobalBounds().contains(mousePos)) {
                        selectSuggestion(i);
                        clickedSuggestion = true;
                        clickedAnywhere = true;
                        clickClock.restart();
                        break;
                    }
                }
            }
            
            // if they clicked somewhere else, close everything
            if (!clickedAnywhere) {
                searchBoxFocused = false;
                showSuggestions = false;
            }
        }
    }
    
    // run the search algorithm
    void performSearch(const unordered_map<string, vector<pair<string, int>>>& trackArtistMap) {
        isSearching = true;
        results.clear();
        showSuggestions = false;
        
        int queryIndex = findSongIndex(searchResults.first, searchResults.second, trackArtistMap);
        if (queryIndex == -1) {
        cout << "Song not found in database." << endl;
        return;
        }    
        
        if (selectedAlgorithm == "K-Nearest Neighbors") {
            results = kNearestNeighbors(10,queryIndex, allSongs);
        } else {
            results = rNN(allSongs,allSongs[queryIndex],0.200);
        }
        
        updateResultsDisplay();
        isSearching = false;
        
        // Clear the selected song/artist for next search
        selectedSongName.clear();
        selectedArtistName.clear();
    }
    
    // format the search results so they look nice
    void updateResultsDisplay() {
        resultTexts.clear();
        
        sf::Text header(font);
        header.setString("Top Recommendations:");
        header.setCharacterSize(24u);
        header.setFillColor(sf::Color(30u, 215u, 96u));
        header.setPosition({70.f, 320.f});
        resultTexts.push_back(header);
        
        float yPos = 370.f;
        for (size_t i = 0; i < results.size(); ++i) {
            sf::Text result(font);
            
            string resultStr = to_string(i + 1) + ". " + 
                              results[i].trackName + " - " + 
                              results[i].artist + " (" + 
                              to_string(static_cast<int>(results[i].similarity * 100)) + "% match)";
            
            result.setString(resultStr);
            result.setCharacterSize(16u);
            result.setFillColor(sf::Color::White);
            result.setPosition({70.f, yPos});
            resultTexts.push_back(result);
            
            yPos += 35.f;
        }
    }
    
    // draw the algorithm selection dropdown
    void drawDropdownOptions() {
        vector<string> options = {"K-Nearest Neighbors", "Radius Nearest Neighbors"};
        
        for (size_t i = 0; i < options.size(); ++i) {
            sf::RectangleShape optionBox;
            optionBox.setSize({300.f, 40.f});
            optionBox.setPosition({600.f, 190.f + static_cast<float>(i) * 40.f});
            optionBox.setFillColor(sf::Color(70u, 70u, 70u));
            optionBox.setOutlineColor(sf::Color(100u, 100u, 100u));
            optionBox.setOutlineThickness(1.f);
            
            sf::Text optionText(font);
            optionText.setString(options[i]);
            optionText.setCharacterSize(18u);
            optionText.setFillColor(sf::Color::White);
            optionText.setPosition({610.f, 198.f + static_cast<float>(i) * 40.f});
            
            sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
            if (optionBox.getGlobalBounds().contains(mousePos)) {
                optionBox.setFillColor(sf::Color(100u, 100u, 100u));
                
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    selectedAlgorithm = options[i];
                    dropdownText.setString(selectedAlgorithm);
                    dropdownOpen = false;
                }
            }
            
            window.draw(optionBox);
            window.draw(optionText);
        }
    }
    
    // draw the autocomplete suggestions dropdown
    void drawSuggestions() {
        if (!showSuggestions || currentSuggestions.empty()) return;
        
        suggestionBoxes.clear();
        suggestionTexts.clear();
        
        float suggestionHeight = 35.f;
        float totalHeight = suggestionHeight * currentSuggestions.size();
        
        // draw the container for all suggestions
        suggestionsBox.setSize({500.f, totalHeight});
        suggestionsBox.setPosition({50.f, 192.f});
        window.draw(suggestionsBox);
        
        // figure out where the mouse is
        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        
        // draw each individual suggestion
        for (size_t i = 0; i < currentSuggestions.size(); ++i) {
            sf::RectangleShape suggestionBox;
            suggestionBox.setSize({500.f, suggestionHeight});
            suggestionBox.setPosition({50.f, 192.f + static_cast<float>(i) * suggestionHeight});
            
            // highlight it if mouse is over it or if its selected with arrow keys
            bool isHovered = suggestionBox.getGlobalBounds().contains(mousePos);
            bool isSelected = (i == selectedSuggestionIndex);
            
            if (isSelected || isHovered) {
                suggestionBox.setFillColor(sf::Color(70u, 70u, 70u));
            } else {
                suggestionBox.setFillColor(sf::Color(45u, 45u, 45u));
            }
            
            suggestionBoxes.push_back(suggestionBox);
            window.draw(suggestionBox);
            
            // draw the text showing the song and artist
            sf::Text suggestionText(font);
            string displayText = currentSuggestions[i].first + " - " + currentSuggestions[i].second;
            
            // cut it off if its too long
            if (displayText.length() > 60) {
                displayText = displayText.substr(0, 57) + "...";
            }
            
            suggestionText.setString(displayText);
            suggestionText.setCharacterSize(16u);
            suggestionText.setFillColor(sf::Color::White);
            suggestionText.setPosition({60.f, 198.f + static_cast<float>(i) * suggestionHeight});
            
            suggestionTexts.push_back(suggestionText);
            window.draw(suggestionText);
        }
    }
    
    // main loop that keeps everything running
    void run() {
        while (window.isOpen()) {
            while (const optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
                
                handleInput(*event);
            }
            
            // make the cursor blink
            if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
                showCursor = !showCursor;
                cursorClock.restart();
            }
            
            // change the search box color based on if its selected
            if (searchBoxFocused) {
                searchBox.setOutlineColor(sf::Color(30u, 215u, 96u));
            } else {
                searchBox.setOutlineColor(sf::Color(100u, 100u, 100u));
            }
            
            // clear the screen and start drawing
            window.clear(sf::Color(18u, 18u, 18u));
            
            window.draw(titleText);
            window.draw(searchLabel);
            window.draw(searchBox);
            window.draw(inputText);
            
            // draw the blinking cursor
            if (searchBoxFocused && showCursor) {
                sf::RectangleShape cursor;
                cursor.setSize({2.f, 20.f});
                auto bounds = inputText.getGlobalBounds();
                float cursorX = bounds.position.x + bounds.size.x + 2.f;
                cursor.setPosition({cursorX, 160.f});
                cursor.setFillColor(sf::Color::White);
                window.draw(cursor);
            }
            
            window.draw(algorithmLabel);
            window.draw(dropdownBox);
            window.draw(dropdownText);
            window.draw(searchButton);
            window.draw(buttonText);
            window.draw(resultsPanel);
            
            // draw the algorithm dropdown if its open
            if (dropdownOpen) {
                drawDropdownOptions();
            }
            
            // draw autocomplete suggestions if there are any
            if (showSuggestions && !dropdownOpen) {
                drawSuggestions();
            }
            
            // draw the search results
            for (const auto& text : resultTexts) {
                window.draw(text);
            }
            
            window.display();
        }
    }
};

// main entry point - creates the UI and runs it
int main(int argc, char* argv[]) {
    // create and run the UI (data loading happens in the constructor)
    MelodyMapUI app(argv[0]);
    app.run();
    return 0;
}