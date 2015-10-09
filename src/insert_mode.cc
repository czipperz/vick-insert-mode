#include <ncurses.h>

#include "../../../src/contents.hh"
#include "../../../src/key_aliases.hh"
#include "../../../src/show_message.hh"
#include "../../vick-move/src/move.hh"

void enter_insert_mode(contents& contents, boost::optional<int>) {
    char ch;
    show_message("--INSERT--");
    contents.is_inserting = true;
    if(get_contents().refresh) {
        print_contents(get_contents());
        show_message("--INSERT--");
    }
    while((ch = getch()) != _escape) {
        if(contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
            contents.x = contents.cont[contents.y].size();
        } else {
            contents.cont[contents.y].insert(contents.x, 1, ch);
            contents.x++;
        }
        if(get_contents().refresh) {
            print_contents(get_contents());
            show_message("--INSERT--");
        }
    }
    contents.is_inserting = false;
}

void enter_replace_mode(contents& contents, boost::optional<int>) {
    char ch;
    show_message("--INSERT (REPLACE)--");
    contents.is_inserting = true;
    while((ch = getch()) != _escape) {
        if(contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
            contents.x = contents.cont[contents.y].size();
        } else {
            contents.cont[contents.y][contents.x] = ch;
            contents.x++;
        }
        if(get_contents().refresh) {
            print_contents(get_contents());
            show_message("--INSERT (REPLACE)--");
        }
    }
    contents.is_inserting = false;
}
