#include <ncurses.h>

#include "../../../src/contents.hh"
#include "../../../src/key_aliases.hh"
#include "../../vick-move/src/move.hh"

void enter_insert_mode(contents& contents, boost::optional<int>) {
    char ch;
    while((ch = getch()) != _escape) {
        contents.cont[contents.y].insert(contents.x, 1, ch);
        mvf(contents);
    }
}

void enter_replace_insert_mode(contents& contents, boost::optional<int>) {
    char ch;
    while((ch = getch()) != _escape) {
        if(contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
        } else {
            contents.cont[contents.y][contents.x] = ch;
        }
    }
}
