#include <ncurses.h>

#include "../../../src/contents.hh"
#include "../../../src/key_aliases.hh"
#include "../../../src/show_message.hh"
#include "../../vick-move/src/move.hh"

struct insert_c : public change {
    std::string after,before;
    std::string diff;
    unsigned long y,x;
    insert_c(unsigned long y, std::string after, std::string before)
        : after(after), before(before), y(y)
    {
        //derive diff
        bool stilleq = true;
        for(unsigned int b = 0; b < before.length(); b++) {
            for(unsigned int a = 0; a < after.length(); a++) {
                if(b == before.length()) {
                    diff += after.substr(b);
                    return;
                }
                if(before[b] != after[a] && stilleq) {
                    stilleq = false;
                    x = b;
                }
                if(!stilleq) {
                }
            }
        }
    }
    virtual bool is_overriding() override { return true; }
    virtual void undo(contents& contents) override {
        contents.y = y;
        contents.x = x;
        contents.cont[contents.y] = before;
    }
    virtual void redo(contents& contents) override {
        contents.y = y;
        contents.x = x;
        contents.cont[contents.y] = after;
    }
    virtual std::shared_ptr<change> regenerate(const contents& contents) const override {
        return std::make_shared<insert_c>(contents.y, contents.x, diff, contents.cont[contents.y]);
    }
    insert_c(unsigned long y, unsigned long x, std::string diff, std::string before)
        : before(before), diff(diff), y(y), x(x) {
        //derive after
    }
};

boost::optional< std::shared_ptr<change> > enter_insert_mode(contents& contents, boost::optional<int>) {
    std::string before = contents.cont[contents.y];
    auto y = contents.y;
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
    showing_message = false;
    if(contents.cont[contents.y] == before) return boost::none;
    return boost::optional< std::shared_ptr<change> >
        (std::make_shared<insert_c>(y, contents.cont[contents.y], before));
}

boost::optional< std::shared_ptr<change> > enter_replace_mode(contents& contents, boost::optional<int>) {
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
    showing_message = false;
    return boost::none;
}

boost::optional< std::shared_ptr<change> > enter_append_mode(contents& contents, boost::optional<int> b) {
    contents.x++;
    return enter_insert_mode(contents, b);
}
