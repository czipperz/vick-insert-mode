#include <ncurses.h>

#include "../../../test/catch.hpp"
#include "../src/join.hh"

TEST_CASE("join_two_lines") {
    contents contents;
    contents.push_back("hi man");
    contents.push_back("bye man");

    initscr();
    auto change = join_two_lines(contents);
    CHECK(contents.cont[0] == "hi man" "bye man");
    CHECK(contents.cont.size() == 1);

    CHECK(change);
    change.get()->undo(contents);

    CHECK(contents.cont[0] == "hi man");
    CHECK(contents.cont[1] == "bye man");
    CHECK(contents.cont.size() == 2);
    endwin();
}

TEST_CASE("join_two_lines: join lines testing") {
    contents contents;
    contents.push_back("hello");
    contents.push_back("goodbye");
    contents.push_back("wow");
    contents.push_back("vick");
    contents.y = 1;

    initscr();
    auto first = join_two_lines(contents);
    auto second = join_two_lines(contents);
    CHECK(contents.cont[1] == "goodbye" "wow" "vick");
    CHECK(contents.cont.size() == 2);

    CHECK(second);
    if(!second) goto end;
    second.get()->undo(contents);
    CHECK(contents.cont[1] == "goodbye" "wow");
    CHECK(contents.cont[2] == "vick");
    CHECK(contents.cont.size() == 3);

    CHECK(first);
    if(!first) goto end;
    first.get()->undo(contents);
    CHECK(contents.cont[1] == "goodbye");
    CHECK(contents.cont[2] == "wow");
    CHECK(contents.cont[3] == "vick");

end:
    endwin();
}

TEST_CASE("join_two_lines: undo testing") {
    contents contents;
    contents.push_back("hello");
    contents.push_back("goodbye");
    contents.push_back("wow");
    contents.push_back("vick");

    initscr();
    contents.y = 1;
    auto first = join_two_lines(contents);
    contents.y = 1;
    auto second = join_two_lines(contents);

    CHECK(contents.cont[0] == "hello");
    CHECK(contents.cont[1] == "goodbye" "wow" "vick");
    CHECK(contents.cont.size() == 2);

    CHECK(second);
    if(!second) goto end;
    second.get()->undo(contents);
    CHECK(contents.cont[0] == "hello");
    CHECK(contents.cont[1] == "goodbye" "wow");
    CHECK(contents.cont[2] == "vick");
    CHECK(contents.cont.size() == 3);

    {
        contents.y = 0;
        auto third = join_two_lines(contents);
        CHECK(contents.cont[0] == "hello" "goodbye" "wow");
        CHECK(contents.cont[1] == "vick");
        CHECK(contents.cont.size() == 2);

        CHECK(third);
        if(!third) goto end;
        third.get()->undo(contents);
        CHECK(contents.cont[0] == "hello");
        CHECK(contents.cont[1] == "goodbye" "wow");
        CHECK(contents.cont[2] == "vick");
        CHECK(contents.cont.size() == 3);
    }

    CHECK(first);
    if(!first) goto end;
    first.get()->undo(contents);
    CHECK(contents.cont[0] == "hello");
    CHECK(contents.cont[1] == "goodbye");
    CHECK(contents.cont[2] == "wow");
    CHECK(contents.cont[3] == "vick");
    CHECK(contents.cont.size() == 4);

end:
    endwin();
}
