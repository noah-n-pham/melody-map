#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "data_parse.h"

using namespace std;

// this struct holds information about each recommended song
// it stores the track name, artist, and how similar it is to the search query (0.0 to 1.0)
struct SongResult {
    string trackName;
    string artist;
    float similarity;
    
    SongResult(const string& name, const string& art, float sim)
        : trackName(name), artist(art), similarity(sim) {}
};

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

// these are placeholder function declarations for the recommendation algorithms
// khoi will implement the K-Nearest Neighbors algorithm here
vector<SongResult> kNearestNeighbors(const string& songName, int k){
}


// marcelo will implement the Radius Nearest Neighbors algorithm here
vector<SongResult> radiusNearestNeighbors(const string& songName, int k);

class MelodyMapUI {
private:
    // main window and font for the entire application
    sf::RenderWindow window;
    sf::Font font;
    
    // these are the visual boxes on screen 
    sf::RectangleShape searchBox;       // the box where you type the song name
    sf::RectangleShape dropdownBox;     // the box that shows which algorithm is selected
    sf::RectangleShape searchButton;    // the green "Search" button
    sf::RectangleShape resultsPanel;    // the big panel at the bottom that shows results
    
    // all the text elements that display information to the user
    sf::Text titleText;         // "Melody Map" at the top
    sf::Text searchLabel;       // "Enter Song Name:" label
    sf::Text inputText;         // the actual textinput inna search box
    sf::Text algorithmLabel;    // "Select Algorithm:" label
    sf::Text dropdownText;      // shows which algorithm is currently selected
    sf::Text buttonText;        // "Search" text on the button
    vector<sf::Text> resultTexts; //all the recommended songs displayed in the results panel which icurrently have as a placeholder dummy 
    
    // these variables keep track of the current state of the UI
    string userInput;               // what the user has typed so far
    string selectedAlgorithm;       // which algorithm is currently selected (K-NN or Radius)
    bool dropdownOpen;              // whether the dropdown menu is currently open
    bool isSearching;               // whether we're currently running a search
    bool searchBoxFocused;          // whether the search box is active and ready for typing
    vector<SongResult> results;     // the list of recommended songs to display
    sf::Clock cursorClock;          // timer for making the cursor blink
    bool showCursor;                // controls whether the cursor is visible right now
    
    // window dimensions size on screen editable for half screen or popups
    const unsigned int WINDOW_WIDTH = 1000;
    const unsigned int WINDOW_HEIGHT = 800;
    
public:
    // constructor - sets up the window and initializes all the text elements
    MelodyMapUI() : 
        titleText(font),
        searchLabel(font),
        inputText(font),
        algorithmLabel(font),
        dropdownText(font),
        buttonText(font) {
        
        // create the window with our specified dimensions
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Melody Map - Song Recommender");
        window.setFramerateLimit(60); //60 fps limit, good rule of
        
        // font loading
        bool fontLoaded = false;
        vector<string> fontPaths = {
            "C:/Windows/Fonts/arial.ttf",       // standard Windows Arial location; add multiple fonts here in case u wanan swap em
        };
        
        for (const auto& path : fontPaths) {
            if (font.openFromFile(path)) {
                fontLoaded = true;
                cout << "Successfully loaded font from: " << path << endl;
                break;
            }
        }
        
        if (!fontLoaded) {
            cerr << "ERROR: Could not load any font! The UI will not display text correctly." << endl;
            cerr << "Please ensure you have a font file available." << endl;
        }
        
        // set up our initial stuff
        selectedAlgorithm = "K-Nearest Neighbors";  
        dropdownOpen = false;                        
        isSearching = false;                         
        searchBoxFocused = true;                     
        showCursor = true;                           
        
        initializeUI();
    }
    
    //this function positions and styles all the UI elements on the screen
    void initializeUI() {
        //set up the main title at the top - "Melody Map"
        titleText.setString("Melody Map");
        titleText.setCharacterSize(48u);
        titleText.setFillColor(sf::Color(30u, 215u, 96u)); // Spotify green - looks modern!
        titleText.setPosition({static_cast<float>(WINDOW_WIDTH) / 2.f - 150.f, 30.f});
        
        // label that says "Enter Song Name:"
        searchLabel.setString("Enter Song Name:");
        searchLabel.setCharacterSize(20u);
        searchLabel.setFillColor(sf::Color::White);
        searchLabel.setPosition({50.f, 120.f});
        
        // the actual input box where users type
        searchBox.setSize({500.f, 40.f});
        searchBox.setPosition({50.f, 150.f});
        searchBox.setFillColor(sf::Color(50u, 50u, 50u));      // Dark gray background
        searchBox.setOutlineColor(sf::Color(100u, 100u, 100u)); // Lighter gray border
        searchBox.setOutlineThickness(2.f);
        
        // the text that appears inside the search box (what you type)
        inputText.setCharacterSize(18u);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition({60.f, 158.f}); // Slightly indented from the box edge
        
        //label for the algorithm dropdown - "Select Algorithm:"
        algorithmLabel.setString("Select Algorithm:");
        algorithmLabel.setCharacterSize(20u);
        algorithmLabel.setFillColor(sf::Color::White);
        algorithmLabel.setPosition({600.f, 120.f});
        
        // the dropdown box that shows which algorithm is selected
        dropdownBox.setSize({300.f, 40.f});
        dropdownBox.setPosition({600.f, 150.f});
        dropdownBox.setFillColor(sf::Color(50u, 50u, 50u));
        dropdownBox.setOutlineColor(sf::Color(100u, 100u, 100u));
        dropdownBox.setOutlineThickness(2.f);
        
        // text inside the dropdown showing the current selection
        dropdownText.setString(selectedAlgorithm);
        dropdownText.setCharacterSize(18u);
        dropdownText.setFillColor(sf::Color::White);
        dropdownText.setPosition({610.f, 158.f});
        
        // the big green "Search" button
        searchButton.setSize({150.f, 50.f});
        searchButton.setPosition({static_cast<float>(WINDOW_WIDTH) / 2.f - 75.f, 220.f});
        searchButton.setFillColor(sf::Color(30u, 215u, 96u)); // Same Spotify green as title
        
        // "search" text on the button
        buttonText.setString("Search");
        buttonText.setCharacterSize(24u);
        buttonText.setFillColor(sf::Color::Black); // Black text on green button for good contrast
        buttonText.setPosition({static_cast<float>(WINDOW_WIDTH) / 2.f - 40.f, 230.f});
        
        // large panel at the bottom where we display all the song recommendations
        resultsPanel.setSize({900.f, 450.f});
        resultsPanel.setPosition({50.f, 300.f});
        resultsPanel.setFillColor(sf::Color(30u, 30u, 30u));    // Very dark gray
        resultsPanel.setOutlineColor(sf::Color(100u, 100u, 100u));
        resultsPanel.setOutlineThickness(2.f);
    }
    
    // this function handles all user input - keyboard typing and mouse clicks
    void handleInput(const sf::Event& event) {
        // check if the user typed something on the keyboard
        if (event.is<sf::Event::TextEntered>() && searchBoxFocused) {
            const auto& textEvent = *event.getIf<sf::Event::TextEntered>();
            
            if (textEvent.unicode == 8 && !userInput.empty()) {
                // backspace pressed - delete the last character
                userInput.pop_back();
            } else if (textEvent.unicode == 13) {
                // enter key pressed - run the search if there's input
                if (!userInput.empty()) {
                    performSearch();
                }
            } else if (textEvent.unicode >= 32 && textEvent.unicode < 128) {
                // normal printable character - add it to the input
                userInput += static_cast<char>(textEvent.unicode);
            }
            // update the displayed text with the new input
            inputText.setString(userInput);
        }
        
        if (event.is<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
            if (searchBox.getGlobalBounds().contains(mousePos)) {
                searchBoxFocused = true;
            } else {
            
                searchBoxFocused = false;
            }
            
            
            if (dropdownBox.getGlobalBounds().contains(mousePos)) {
                dropdownOpen = !dropdownOpen;
            }
            
            
            if (searchButton.getGlobalBounds().contains(mousePos) && !userInput.empty()) {
                performSearch();
            }
        }
    }
    
    // this function runs the selected algorithm and gets the song recommendations
    void performSearch() {
        isSearching = true;     
        results.clear();         
        
        // call the appropriate algorithm based on what's selected in the dropdown
        if (selectedAlgorithm == "K-Nearest Neighbors") {
            // Khoi's K-NN algorithm will be called here
            results = kNearestNeighbors(userInput, 10);
        } else {
            // Marcelo's Radius algorithm will be called here
            results = radiusNearestNeighbors(userInput, 10);
        }
        
        // update the UI to show the new results
        updateResultsDisplay();
        isSearching = false;     // Done searching
    }
    
    // this function takes the search results and formats them nicely for display
    void updateResultsDisplay() {
        resultTexts.clear(); // Clear out any old results
        
        // add a header that says "Top Recommendations:"
        sf::Text header(font);
        header.setString("Top Recommendations:");
        header.setCharacterSize(24u);
        header.setFillColor(sf::Color(30u, 215u, 96u)); // Green header to match theme
        header.setPosition({70.f, 320.f});
        resultTexts.push_back(header);
        
        // loop through each result and create a formatted text line for it
        float yPos = 370.f; // Starting Y position for the first result
        for (size_t i = 0; i < results.size(); ++i) {
            sf::Text result(font);
            
            // format: "1. Song Name - Artist Name (95% match)"
            string resultStr = to_string(i + 1) + ". " + 
                              results[i].trackName + " - " + 
                              results[i].artist + " (" + 
                              to_string(static_cast<int>(results[i].similarity * 100)) + "% match)";
            
            result.setString(resultStr);
            result.setCharacterSize(16u);
            result.setFillColor(sf::Color::White);
            result.setPosition({70.f, yPos});
            resultTexts.push_back(result);
            
            yPos += 35.f; // move down for the next result
        }
    }
    
    // this function draws the dropdown menu options when it's open
    void drawDropdownOptions() {
        // our two algorithm choices
        vector<string> options = {"K-Nearest Neighbors", "Radius Nearest Neighbors"};
        
        // ddraw a box for each option
        for (size_t i = 0; i < options.size(); ++i) {
            // create a box for this option
            sf::RectangleShape optionBox;
            optionBox.setSize({300.f, 40.f});
            optionBox.setPosition({600.f, 190.f + static_cast<float>(i) * 40.f});
            optionBox.setFillColor(sf::Color(70u, 70u, 70u));
            optionBox.setOutlineColor(sf::Color(100u, 100u, 100u));
            optionBox.setOutlineThickness(1.f);
            
            // add text showing the option name
            sf::Text optionText(font);
            optionText.setString(options[i]);
            optionText.setCharacterSize(18u);
            optionText.setFillColor(sf::Color::White);
            optionText.setPosition({610.f, 198.f + static_cast<float>(i) * 40.f});
            
            // check if the mouse is hovering over this option
            sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
            if (optionBox.getGlobalBounds().contains(mousePos)) {
                // highlight the box if we're hovering
                optionBox.setFillColor(sf::Color(100u, 100u, 100u));
                
                // if the user clicks while hovering, select this algorithm
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    selectedAlgorithm = options[i];
                    dropdownText.setString(selectedAlgorithm);
                    dropdownOpen = false; // close the dropdown after selection
                }
            }
            
            // draw the option box and text
            window.draw(optionBox);
            window.draw(optionText);
        }
    }
    
    // main application loop - keeps the window running and handles all drawing
    void run() {
        while (window.isOpen()) {
            // check for any events (mouse clicks, key presses, window close, etc.)
            while (const optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close(); // User clicked the X button
                }
                
                handleInput(*event);
            }
            
            // Make the cursor blink every half second
            if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
                showCursor = !showCursor;  
                cursorClock.restart();          
            }
            
            if (searchBoxFocused) {
                searchBox.setOutlineColor(sf::Color(30u, 215u, 96u)); // Green = ready to type!
            } else {
                searchBox.setOutlineColor(sf::Color(100u, 100u, 100u)); // Gray = not focused
            }
            
            // clear the window with a dark background color
            window.clear(sf::Color(18u, 18u, 18u));
            
            window.draw(titleText);
            window.draw(searchLabel);
            window.draw(searchBox);
            window.draw(inputText);
            
            if (searchBoxFocused && showCursor) {
                sf::RectangleShape cursor;
                cursor.setSize({2.f, 20.f});
                auto bounds = inputText.getGlobalBounds();
                float cursorX = bounds.position.x + bounds.size.x + 2.f;
                cursor.setPosition({cursorX, 160.f});
                cursor.setFillColor(sf::Color::White);
                window.draw(cursor);
            }
            
            // draw the rest of the UI
            window.draw(algorithmLabel);
            window.draw(dropdownBox);
            window.draw(dropdownText);
            window.draw(searchButton);
            window.draw(buttonText);
            window.draw(resultsPanel);
            
            // ff the dropdown menu is open, draw the options
            if (dropdownOpen) {
                drawDropdownOptions();
            }
            
            // draw all the search results
            for (const auto& text : resultTexts) {
                window.draw(text);
            }
            
            // display everything we just drew
            window.display();
        }
    }
};

//dummies below implementation

// Khoi's K-Nearest Neighbors algorithm
// This should load the Spotify dataset, calculate distances, and return the k closest songs
// vector<SongResult> kNearestNeighbors(const string& songName, int k) {
//     // TODO: Khoi - Replace this with your actual K-NN implementation
//     // For now, just returning some dummy data so we can test the UI
//     vector<SongResult> results;
//     results.push_back(SongResult("Similar Song 1", "Artist A", 0.95f));
//     results.push_back(SongResult("Similar Song 2", "Artist B", 0.89f));
//     results.push_back(SongResult("Similar Song 3", "Artist C", 0.84f));
//     results.push_back(SongResult("Similar Song 4", "Artist D", 0.80f));
//     results.push_back(SongResult("Similar Song 5", "Artist E", 0.76f));
//     return results;
// }

// marcelo's Radius Nearest Neighbors algorithm
// this should find all songs within a certain distance, then return the top k
vector<SongResult> radiusNearestNeighbors(const string& songName, int k) {
    // TODO: Marcelo - Replace this with your actual Radius algorithm
    // for now, just returning some dummy data so we can test the UI
    vector<SongResult> results;
    results.push_back(SongResult("Radius Song 1", "Artist X", 0.92f));
    results.push_back(SongResult("Radius Song 2", "Artist Y", 0.87f));
    results.push_back(SongResult("Radius Song 3", "Artist Z", 0.81f));
    results.push_back(SongResult("Radius Song 4", "Artist W", 0.78f));
    results.push_back(SongResult("Radius Song 5", "Artist V", 0.74f));
    return results;
}

// main entry point - creates the UI and runs it
int main() {
    MelodyMapUI app;
    app.run();
    return 0;
}