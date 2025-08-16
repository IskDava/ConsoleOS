//todo: power button bind

#include <ncurses.h>
#include <locale.h>
#include <vector>
#include <string>
#include <map>
#include <any>

using namespace std;

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
    printw("%s", query.c_str()); // showing question
    for (int i = 0; i < variants.size(); i++) {
        // showing variants
        mvprintw(i+1, 1, "%s", variants[i].c_str());
    }

    printw("\nNavigate with arrows and press enter to submit\n");

    refresh();

    int chosen = 0;
    choose(chosen, 1, variants.size());

    bool chosing = true;
    while (chosing) {
        int input = getch();
        switch (input) {
            case KEY_UP:
                // if already chosen the first variant we climb down to the last one
                if (chosen == 0) chosen = variants.size() - 1;
                // else climb up a variant
                else chosen--;
                choose(chosen, 1, variants.size());
                break;
            case KEY_DOWN:
                // if already chose the last variant we climb all the way up
                if (chosen == variants.size() - 1) chosen = 0;
                // else climb down a variant
                else chosen++;
                choose(chosen, 1, variants.size());
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
            //TODO: add backspace and text cursor
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

map<string, map<string, string>> lines = {
    {"Greetings", {
        {"English", "Hello ~! I'm ConsoleOS.~ I am useless. Let's start!\n"},
        {"Russian", "Привет ~! Я ConsoleOS. Я бесполезный. Давайте начнем!\n"}
    }},
    {"QuitMsg", {
        {"English", "Press anything to quit..."},
        {"Russian", "Нажмите что-нибудь чтобы выйти..."}
    }}
};

void printbf(string format, vector<string> values) {
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

struct Guard {
    Guard() { init(); }
    ~Guard() { shut_down(); }
};

int main() {
    Guard guard;

    string lang = ask_variants("Select prefered language:\n", {"English", "Russian", "Spanish"});
    clear();

    string username = ask_input("How do I call you? ");
    clear();

    printbf(lines["Greetings"][lang], {username});
    refresh();

    printw("%s", lines["QuitMsg"][lang].c_str());

    refresh();
    
    getch();
}
