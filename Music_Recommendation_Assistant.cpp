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

int main()
{
    return 0;
}
