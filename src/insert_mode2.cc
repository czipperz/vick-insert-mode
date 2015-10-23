#include <ncurses.h>

#include "../../../src/contents.hh"
#include "../../../src/key_aliases.hh"
#include "../../../src/show_message.hh"
#include "../../vick-move/src/move.hh"

struct cons3_c : public change {
    std::shared_ptr< change > a, b, c;
    cons3_c(change* a, change* b, change* c) : a(a), b(b), c(c) {}

    virtual bool is_overriding() override {
        return a->is_overriding() || b->is_overriding() || c->is_overriding();
    }
    virtual void undo(contents& contents) override {
        a->undo(contents);
        b->undo(contents);
        c->undo(contents);
    }
    virtual void redo(contents& contents) override {
        c->redo(contents);
        b->redo(contents);
        a->redo(contents);
    }
    virtual std::shared_ptr< change >
    regenerate(const contents& contents) const override {
        return std::make_shared< cons3_c >(contents, *a, *b, *c);
    }
    cons3_c(const contents& contents, const change& a, const change& b,
            const change& c)
        : a(a.regenerate(contents)), b(b.regenerate(contents)),
          c(c.regenerate(contents)) {}
};

struct insert_c : public change {
    unsigned long y, x;
    insert_c() {}

    virtual bool is_overriding() override { return true; }
    virtual void undo(contents& contents) override {
        contents.y = y;
        contents.x = x;
    }
    virtual void redo(contents& contents) override {}
    virtual std::shared_ptr< change >
    regenerate(const contents& contents) const override {}
};

boost::optional< std::shared_ptr< change > >
enter_insert_mode(contents& contents, boost::optional< int >) {
    char ch;
    show_message("--INSERT--");
    contents.is_inserting = true;
    if (contents.refresh) {
        print_contents(contents);
        show_message("--INSERT--");
    }
    while ((ch = getch()) != _escape) {
        auto event = global_insert_map.find(ch);
        if (event != global_insert_map.end()) {
            event->second(contents, 0);
        } else if (contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
            contents.x = contents.cont[contents.y].size();
        } else {
            contents.cont[contents.y][contents.x] = ch;
            contents.x++;
        }
        if (contents.refresh) {
            print_contents(contents);
            show_message("--INSERT--");
        }
    }
    contents.is_inserting = false;
    showing_message = false;
    return boost::none;
}
