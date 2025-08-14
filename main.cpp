//todo: power button bind

#include <iostream>
#include <ncurses.h>
#include <locale.h>
#include <vector>
#include <string>
#include <map>

using namespace std;

void init() {
    // all preparations
    setlocale(LC_ALL, ""); // UTF-8 encoding
    initscr(); // Initialazing ncurses
    raw(); // Raw mode (no Ctrl+C)
    noecho(); // No writing what is inputed in the console
    keypad(stdscr, TRUE); // Listening to F1, F2 and e.t.c.
    curs_set(0); // Hiding cursor
}

void shut_down() {
    endwin(); // returning to normal mode
}

void choose(int variant_pos, int starting_pos, int all_variants) {
    mvprintw(starting_pos + variant_pos, 0, ">");
    for (int i = starting_pos; i < starting_pos + all_variants; i++) {
        if (i != starting_pos + variant_pos) mvprintw(i, 0, " ");
    }
}

string ask_lang() {
    vector<string> langvars = {
        "English",
        "Russian"
    };

    printw("Select language:\n");
    for (int i = 0; i < langvars.size(); i++) {
        mvprintw(i+1, 1, "%s", langvars[i].c_str());
    }

    printw("\nNavigate with arrows and press enter to submit\n");

    refresh(); // showing lang choice

    int chosen = 0;
    choose(chosen, 1, langvars.size());

    bool chosing = true;
    while (chosing) {
        int input = getch();
        switch (input) {
            case KEY_DOWN:
                if (chosen == 0) {
                    chosen = langvars.size() - 1;
                    choose(chosen, 1, langvars.size());
                } else {
                    choose(chosen--, 1, langvars.size());
                }
                break;
            case KEY_UP:
                chosen++;
                chosen %= langvars.size() - 1;
                choose(chosen, 1, langvars.size());
                break;
            case '\n':
            case KEY_ENTER:
                chosing = false;
                break;
        }
    }
    
    return langvars[chosen];
}

map<string, map<string, string>> lines = {
    {"Greetings", {
        {"English", "Hello World! I'm ConsoleOS. I am useless. Let's start!\n"},
        {"Russian", "Привет мир! Я ConsoleOS. Я бесполезный. Давайте начнем!\n"}
    }},
    {"QuitMsg", {
        {"English", "Press anything to quit..."},
        {"Russian", "Нажмите что-нибудь чтобы выйти..."}
    }}
};

struct Guard {
    Guard() { init(); }
    ~Guard() { shut_down(); }
};

int main() {
    Guard guard;

    string lang = ask_lang();
    clear();

    printw("%s", lines["Greetings"][lang].c_str());
    refresh();

    printw("%s", lines["QuitMsg"][lang].c_str());

    refresh();
    
    getch();
}
