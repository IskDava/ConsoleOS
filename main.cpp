//todo: power button bind

#include <cstring>
#include <ncurses.h>
#include <locale.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
using namespace std;

ofstream maindiskwriter;
ifstream maindiskreader;

void init() {
    // all preparations of the terminal
    setlocale(LC_ALL, ""); // UTF-8 encoding
    initscr(); // Initialazing ncurses
    raw(); // Raw mode (no Ctrl+C)
    noecho(); // No writing what is inputed in the console
    keypad(stdscr, TRUE); // Listening to F1, F2 and e.t.c.
    curs_set(0); // Hiding cursor
}

void shut_down() {
    // all the preparations before shutting down (e.g. deleting cache, saving opened files)
    maindiskwriter.close();
    endwin(); // returning to normal mode
}

void choose(
    int variant_pos, // the variant to choose
    int starting_pos, // starting row where the fist variant is
    int all_variants_count // number of variants
) {
    // allows to choose the variant when asked
    mvprintw(starting_pos + variant_pos, 0, ">");
    for (int i = starting_pos; i < starting_pos + all_variants_count; i++) {
        // clearing chosed state from every other variant
        if (i != starting_pos + variant_pos) mvprintw(i, 0, " ");
    }
}

string ask_variants(string query, vector<string> variants) {
    // making a question where you have several variants and navigate whith arrows and enter
    int row, col;
    printw("%s", query.c_str()); // showing question
    getyx(stdscr, row, col);
    for (int i = row; i < row + variants.size(); i++) {
        // showing variants
        mvprintw(i+1, 1, "%s", variants[i-row].c_str());
    }
    row++;

    printw("\nNavigate with arrows and press enter to submit\n");

    refresh();

    int chosen = 0;
    choose(chosen, row, variants.size());

    bool chosing = true;
    while (chosing) {
        int input = getch();
        switch (input) {
            case KEY_UP:
                // if already chosen the first variant we climb down to the last one
                if (chosen == 0) chosen = variants.size() - 1;
                // else climb up a variant
                else chosen--;
                choose(chosen, row, variants.size());
                break;
            case KEY_DOWN:
                // if already chose the last variant we climb all the way up
                if (chosen == variants.size() - 1) chosen = 0;
                // else climb down a variant
                else chosen++;
                choose(chosen, row, variants.size());
                break;
            case '\n':
            case KEY_ENTER:
                // closing if user submits
                chosing = false;
                break;
        }
    }
    
    return variants[chosen];
}

string ask_input(string query) {
    // asking user input for answering the question
    printw("%s\n", query.c_str()); // writing the question
    printw("Press enter to submit\n");
    move(0, query.length());
    
    bool writing = true;
    string answer = "";

    while (writing) {
        int input = getch();
        switch (input) {
            case '\n':
            case KEY_ENTER:
                writing = false;
                break;
            case '\b':
            case KEY_BACKSPACE:
                if (answer.length() > 0) {
                    answer = answer.substr(0, answer.length()-1);
                    int y, x;
                    getyx(stdscr, y, x);
                    x--;
                    mvprintw(y, x, " ");
                    move(y, x);
                }
                break;
            default:
                // if it is a valid letter (via ASCII codes) then adding it to the answer and printing the letter
                if (32 <= input && input <= 255) { 
                    answer += (char)input;
                    printw("%c", (char)input);
                }
        }
    }

    return answer;
}

void printbf(string format, vector<string> values) {
    // printing with auto formating ~ with values
    for (int i = 0; i < values.size(); i++) {
        for (int j = 0; j < format.length(); j++) {
            if (format[j] == '~') {
                format = format.substr(0, j) + values[i] + format.substr(j+1, format.length());
                break;
            }
        }
    }
    printw("%s", format.c_str());
}

void init_disk() {
    maindiskwriter.open("disk.davad", ios::binary);
    maindiskreader.open("disk.davad", ios::binary);
    maindiskwriter << "consoleosdiskishealthy";
}
bool check_disk() {
    char sign[23] = {0};
    maindiskreader.read(sign, 22);
    if (strncmp(sign, "consoleosdiskishealthy", 22) == 0) {
        return true;
    }
    return false;
}

struct Guard {
    Guard() { init(); }
    ~Guard() { shut_down(); }
};

int main() {
    Guard guard;

    string username = ask_input("How do I call you? ");
    clear();

    printw("Initializing disk...\n");
    refresh();

    if (fs::exists("disk.davad")) {
        string answer = ask_variants("Disk already exists:", {"Boot from it", "Clear it"});
        if (answer == "Clear it") {
            init_disk();
        } else {
            if (check_disk()) printw("Disk is valid");
        }
    } else {
        init_disk();
    }

    clear();
    printbf("Hello ~! I'm ConsoleOS. Let's start!\n", {username});
    printw("Press anything to quit...");

    refresh();
    
    getch();
}
