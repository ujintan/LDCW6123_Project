/*
================================================================================
  MUSIC RECOMMENDATION ASSISTANT  (v2)
  LDCW6123 - Part 2: Interactive Program (C++)

  CONNECTION TO PART 1:
    Part 1 traced the innovation life cycle of DIGITAL MUSIC STREAMING (Spotify)
    using Brian Winston's model. The feature that made streaming succeed was
    smart music DISCOVERY - Spotify's "Discover Weekly" recommends songs by
    scoring how well each track matches a listener's taste. This program is a
    small working model of that idea: it scores a song library against the
    user's chosen genre, mood, and era, then RANKS the best matches.

  TECHNIQUES: switch-case (menu + genre), if-else (scoring, validation),
              loops and arrays (library, playlist, results).
================================================================================
*/

#include <iostream>
#include <string>
#include <limits>
#include <cctype>   // for tolower() - case-insensitive search

using namespace std;

// A single song in our library.
//   genre : 1=Pop 2=Rock 3=Hip-Hop 4=Electronic 5=Classical
//   energy: 1 (calm) to 5 (energetic)   year: release year
struct Song {
    string title;
    string artist;
    int    genre;
    int    energy;
    int    year;
};

const int LIBRARY_SIZE = 40;

Song library[LIBRARY_SIZE] = {
    // ---- Pop (1) ----
    {"Blinding Lights",      "The Weeknd",        1, 5, 2019},
    {"Someone You Loved",    "Lewis Capaldi",     1, 2, 2018},
    {"Shape of You",         "Ed Sheeran",        1, 4, 2017},
    {"Rolling in the Deep",  "Adele",             1, 3, 2010},
    {"Bad Guy",              "Billie Eilish",     1, 3, 2019},
    {"Uptown Funk",          "Mark Ronson",       1, 5, 2014},
    {"Levitating",           "Dua Lipa",          1, 4, 2020},
    {"Poker Face",           "Lady Gaga",         1, 4, 2008},
    // ---- Rock (2) ----
    {"Bohemian Rhapsody",    "Queen",             2, 4, 1975},
    {"Nothing Else Matters", "Metallica",         2, 2, 1991},
    {"Smells Like Teen Spirit","Nirvana",         2, 5, 1991},
    {"Believer",             "Imagine Dragons",   2, 5, 2017},
    {"Hotel California",     "Eagles",            2, 3, 1976},
    {"Back In Black",        "AC/DC",             2, 5, 1980},
    {"Numb",                 "Linkin Park",       2, 4, 2003},
    {"Sweet Child O' Mine",  "Guns N' Roses",     2, 4, 1987},
    // ---- Hip-Hop (3) ----
    {"SICKO MODE",           "Travis Scott",      3, 5, 2018},
    {"Lucid Dreams",         "Juice WRLD",        3, 2, 2018},
    {"Lose Yourself",        "Eminem",            3, 5, 2002},
    {"God's Plan",           "Drake",             3, 3, 2018},
    {"HUMBLE.",              "Kendrick Lamar",    3, 5, 2017},
    {"Sunflower",            "Post Malone",       3, 3, 2018},
    {"Old Town Road",        "Lil Nas X",         3, 4, 2019},
    {"Empire State of Mind", "Jay-Z",             3, 4, 2009},
    // ---- Electronic (4) ----
    {"Titanium",             "David Guetta",      4, 5, 2011},
    {"Faded",                "Alan Walker",       4, 3, 2015},
    {"Wake Me Up",           "Avicii",            4, 4, 2013},
    {"Strobe",               "deadmau5",          4, 2, 2009},
    {"Clarity",              "Zedd",              4, 4, 2012},
    {"Animals",              "Martin Garrix",     4, 5, 2013},
    {"Midnight City",        "M83",               4, 3, 2011},
    {"Scary Monsters and Nice Sprites","Skrillex",4, 5, 2010},
    // ---- Classical (5) ----
    {"Clair de Lune",        "Claude Debussy",    5, 1, 1905},
    {"Symphony No. 5",       "Beethoven",         5, 4, 1808},
    {"The Four Seasons",     "Vivaldi",           5, 3, 1725},
    {"Nocturne Op. 9 No.2",  "Chopin",            5, 1, 1832},
    {"Canon in D",           "Pachelbel",         5, 2, 1680},
    {"Eine kleine Nachtmusik","Mozart",           5, 3, 1787},
    {"Ride of the Valkyries","Wagner",            5, 5, 1856},
    {"Moonlight Sonata",     "Beethoven",         5, 1, 1801}
};

// Session playlist (stores library indexes) and listening statistics.
int  playlist[LIBRARY_SIZE];
int  playlistCount = 0;

// Listening statistics.
int  totalRecommendations = 0;
int  genrePickCount[6] = {0,0,0,0,0,0};   // index 1..5 used

// Safely read an integer (prevents crashes on letters/symbols).
bool readInt(int &value) {
    cin >> value;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

// Keep asking until the user enters a whole number within [minVal, maxVal].
// If allowBack is true, the user may type -1 to go back (returns -1).
int readIntRange(const string &prompt, int minVal, int maxVal, bool allowBack) {
    int value;
    while (true) {
        cout << prompt;
        if (allowBack) cout << " (or -1 to go back): ";
        else           cout << ": ";

        if (!readInt(value)) {
            cout << "  [!] That wasn't a number. Please try again.\n";
            continue;
        }
        if (allowBack && value == -1) return -1;
        if (value < minVal || value > maxVal) {
            cout << "  [!] Please enter a number between "
                 << minVal << " and " << maxVal << ".\n";
            continue;
        }
        return value;
    }
}

// Clear the screen (Windows: cls, Linux/Mac: clear).
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Pause until the user presses Enter.
void pause() {
    cout << "\nPress Enter to return to the menu...";
    cin.get();
}

// Convert a string to lower case (for case-insensitive search).
string toLower(string text) {
    for (int i = 0; i < (int)text.length(); i++)
        text[i] = tolower(text[i]);
    return text;
}

// Convert a genre number into its name (switch-case).
string genreName(int g) {
    switch (g) {
        case 1: return "Pop";
        case 2: return "Rock";
        case 3: return "Hip-Hop / Rap";
        case 4: return "Electronic / Dance";
        case 5: return "Classical";
        default:return "Unknown";
    }
}

// The welcome banner, shown at the top of every page.
void showWelcome() {
    cout << "================================================================\n";
    cout << "                MUSIC RECOMMENDATION ASSISTANT                  \n";
    cout << "        A mini 'Discover Weekly' - inspired by Spotify          \n";
    cout << "================================================================\n\n";
}

// Start a fresh page: clear the screen then re-print the banner.
void newPage() {
    clearScreen();
    showWelcome();
}

// Add one song to the playlist. Prevents duplicates.
bool addSongToPlaylist(int idx) {
    for (int i = 0; i < playlistCount; i++) {
        if (playlist[i] == idx) {
            cout << "    \"" << library[idx].title
                 << "\" is already in your playlist.\n";
            return false;
        }
    }
    playlist[playlistCount++] = idx;
    cout << "    Added \"" << library[idx].title << "\" to your playlist!\n";
    return true;
}

// Offer to add songs from a result list, one by one, until the user types 0.
void offerToAddSongs(int choices[], int count) {
    if (count == 0) return;
    cout << "\nWould you like to add any of these to your playlist?\n";
    while (true) {
        cout << "  Enter the number of a song to add (1";
        if (count > 1) cout << "-" << count;
        cout << "), or 0 when you're done: ";
        int pick;
        if (!readInt(pick)) {
            cout << "  [!] That wasn't a number. Please try again.\n";
            continue;
        }
        if (pick == 0) { cout << "    Done adding songs.\n"; break; }
        if (pick >= 1 && pick <= count) addSongToPlaylist(choices[pick - 1]);
        else cout << "  [!] Please enter a number between 1 and "
                  << count << ", or 0 to finish.\n";
    }
}

// Scores every song against the user's preferences, prints the ranked top 3,
// and stores those picks in topResults[] so the caller can add them.
// Scoring: +50 genre match, +0..40 mood closeness, +20 era match, +5 recency.
// Returns how many songs were placed in topResults[] (0 to 3).
int recommendEngine(int genre, int mood, int era, int topResults[]) {
    int score[LIBRARY_SIZE];

    // Step 1: score every song
    for (int i = 0; i < LIBRARY_SIZE; i++) {
        int s = 0;

        if (library[i].genre == genre) s += 50;   // genre match (if-else)
        else                           s += 0;

        int diff = library[i].energy - mood;      // mood closeness
        if (diff < 0) diff = -diff;
        s += (4 - diff) * 10;
        if (s < 0) s = 0;

        if (era == 1 && library[i].year < 2000) s += 20;              // era (if-else chain)
        else if (era == 2 && library[i].year >= 2000 && library[i].year <= 2009) s += 20;
        else if (era == 3 && library[i].year >= 2010) s += 20;

        if (library[i].year >= 2015) s += 5;       // recency bonus

        score[i] = s;
    }

    // Step 2: print the header
    cout << "\n----------------------------------------------------------------\n";
    cout << "   TOP MATCHES FOR: " << genreName(genre)
         << " | mood " << mood << "/5 | ";
    if (era == 1)      cout << "Classic (pre-2000)";
    else if (era == 2) cout << "2000s";
    else               cout << "2010s & newer";
    cout << "\n----------------------------------------------------------------\n";

    // Step 3: pick the 3 highest-scoring songs
    bool usedBefore[LIBRARY_SIZE] = {false};
    int shown = 0;

    for (int rank = 1; rank <= 3; rank++) {
        int best = -1, bestScore = -1;
        for (int i = 0; i < LIBRARY_SIZE; i++) {
            if (!usedBefore[i] && score[i] > bestScore) {
                bestScore = score[i];
                best = i;
            }
        }
        if (best == -1 || bestScore <= 0) break;
        usedBefore[best] = true;

        string matchLabel;                          // label (if-else)
        if (bestScore >= 90)      matchLabel = "PERFECT MATCH";
        else if (bestScore >= 70) matchLabel = "Great match";
        else if (bestScore >= 50) matchLabel = "Good match";
        else                      matchLabel = "Might also like";

        cout << "\n  #" << rank << "  " << library[best].title
             << "  -  " << library[best].artist << "\n";
        cout << "       Genre: " << genreName(library[best].genre)
             << " | Energy: " << library[best].energy << "/5"
             << " | Year: " << library[best].year << "\n";
        cout << "       [" << matchLabel << "  |  score: " << bestScore << "]\n";

        topResults[shown] = best;
        shown++;
    }

    if (shown == 0)
        cout << "\n  Sorry, no good matches found. Try different options.\n";

    totalRecommendations++;
    if (genre >= 1 && genre <= 5) genrePickCount[genre]++;

    return shown;
}

// FEATURE 1: get a recommendation, then offer to add songs. -1 goes back.
void getRecommendation() {
    cout << "\nChoose a genre:\n";
    cout << "  1. Pop\n  2. Rock\n  3. Hip-Hop / Rap\n";
    cout << "  4. Electronic / Dance\n  5. Classical\n";
    int genre = readIntRange("Your choice (1-5)", 1, 5, true);
    if (genre == -1) return;

    cout << "\nHow energetic do you feel? (1 = very chill ... 5 = very hyped)\n";
    int mood = readIntRange("Your energy level (1-5)", 1, 5, true);
    if (mood == -1) return;

    cout << "\nPick an era:\n";
    cout << "  1. Classic (before 2000)\n  2. 2000s\n  3. 2010s and newer\n";
    int era = readIntRange("Your choice (1-3)", 1, 3, true);
    if (era == -1) return;

    int topResults[3];
    int numResults = recommendEngine(genre, mood, era, topResults);

    offerToAddSongs(topResults, numResults);

    int rating = readIntRange("\nRate this recommendation (1-5)", 1, 5, true);
    if (rating == -1) return;
    if (rating >= 4 && rating <= 5) cout << "    Awesome! Enjoy the music.\n";
    else if (rating == 3)           cout << "    Thanks - we'll fine-tune next time.\n";
    else                            cout << "    Sorry about that - your feedback helps us improve.\n";
}

// FEATURE 2: show the session playlist.
void showPlaylist() {
    cout << "\n----------------------------------------------------------------\n";
    cout << "   YOUR SESSION PLAYLIST (" << playlistCount << " songs)\n";
    cout << "----------------------------------------------------------------\n";
    if (playlistCount == 0) {
        cout << "   Your playlist is empty. Get a recommendation and add songs!\n";
        return;
    }
    for (int i = 0; i < playlistCount; i++) {
        int idx = playlist[i];
        cout << "   " << (i + 1) << ". " << library[idx].title
             << " - " << library[idx].artist
             << " (" << genreName(library[idx].genre) << ", "
             << library[idx].year << ")\n";
    }
}

int main()
{
    return 0;
}
