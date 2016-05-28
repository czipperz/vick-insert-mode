/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ncurses.h>

#include "concat_c.hh"
#include "configuration.hh"
#include "contents.hh"
#include "file_contents.hh"
#include "../lib.hh"
#include "key_aliases.hh"
#include "show_message.hh"
#include "vick-move/lib.hh"

namespace vick {
namespace insert_mode {

struct insert_c : public change {
    const std::string track;
    const move_t y, x;
    insert_c(const std::string& track, move_t y, move_t x)
        : track(track)
        , y(y)
        , x(x) {}
    virtual bool is_overriding() const noexcept override {
        return true;
    }
    virtual void undo(contents& contents) override {
        contents.cont[y] = contents.cont[y].substr(0, x) +
                           contents.cont[y].substr(x + track.size());
        contents.y = y;
        contents.x = x;
        if (contents.x and contents.x >= contents.cont[y].size())
            contents.x = contents.cont[y].size() - 1;
    }
    virtual void redo(contents& contents) override {
        contents.cont[y] = contents.cont[y].substr(0, x) + track +
                           contents.cont[y].substr(x);
        contents.y = y;
        contents.x = x + track.size();
        if (contents.x >= contents.cont[y].size())
            contents.x = contents.cont[y].size() - 1;
    }
    virtual std::shared_ptr<change>
    regenerate(const contents& contents) const override {
        return std::make_shared<insert_c>(track, contents.y,
                                          contents.x);
    }
};

struct newline_c : public change {
    const std::string first, second;
    const move_t y;
    newline_c(const contents& contents)
        : first(contents.cont[contents.y].substr(0, contents.x))
        , second(contents.cont[contents.y].substr(contents.x))
        , y(contents.y) {}
    virtual bool is_overriding() const noexcept override {
        return true;
    }
    virtual void undo(contents& contents) override {
        contents.cont[y] = first + second;
        contents.cont.erase(contents.cont.begin() + y + 1);
        contents.y = y;
        contents.x = first.size();
    }
    virtual void redo(contents& contents) override {
        contents.cont[y] = first;
        contents.cont.insert(contents.cont.begin() + y + 1, second);
        contents.y = y + 1;
        contents.x = 0;
    }
    virtual std::shared_ptr<change>
    regenerate(const contents& contents) const override {
        return std::make_shared<newline_c>(contents);
    }
};

boost::optional<std::shared_ptr<change> >
enter_insert_mode(contents& contents, boost::optional<int> pref) {
    std::string track;
    auto x = contents.x;
    char ch;
    show_message("--INSERT--");
    contents.is_inserting = true;
    if (contents.refresh) {
        print_contents(contents);
        show_message("--INSERT--");
    }
    while ((ch = getch()) != QUIT_KEY) {
        if (ch == '\n') {
            std::vector<std::shared_ptr<change> > changes;
            changes.reserve(3);
            if (track.size())
                changes.push_back(
                    std::make_shared<insert_c>(track, contents.y, x));
            changes.push_back(std::make_shared<newline_c>(contents));
            changes.back()->redo(contents);
            auto recursed = enter_insert_mode(contents, pref);
            if (recursed)
                changes.push_back(recursed.get());
            return boost::optional<std::shared_ptr<change> >(
                std::make_shared<concat_c>(changes));
        }
        if (contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
            contents.x = contents.cont[contents.y].size();
            track += ch;
        } else {
            contents.cont[contents.y].insert(contents.x, 1, ch);
            contents.x++;
            track += ch;
        }
        if (contents.refresh) {
            print_contents(contents);
            show_message("--INSERT--");
        }
    }
    contents.is_inserting = false;
    showing_message = false;
    return boost::optional<std::shared_ptr<change> >(
        std::make_shared<insert_c>(track, contents.y, x));
}

struct replace_c : public change {
    const std::string o, n;
    const move_t y, x;
    replace_c(const std::string& o, const std::string& n, move_t y,
              move_t x)
        : o(o)
        , n(n)
        , y(y)
        , x(x) {}
    virtual bool is_overriding() const noexcept override {
        return true;
    }
    virtual void undo(contents& contents) override {
        contents.cont[y] = contents.cont[y].substr(0, x) + o +
                           contents.cont[y].substr(x + o.size());
    }
    virtual void redo(contents& contents) override {
        contents.cont[y] = contents.cont[y].substr(0, x) + n +
                           contents.cont[y].substr(x + n.size());
    }
    virtual std::shared_ptr<change>
    regenerate(const contents& contents) const override {
        return std::make_shared<replace_c>(contents.cont[contents.y]
                                               .substr(contents.x,
                                                       n.size()),
                                           n, contents.y, contents.x);
    }
};

boost::optional<std::shared_ptr<change> >
enter_replace_mode(contents& contents, boost::optional<int> pref) {
    std::string o, n;
    auto x = contents.x;
    char ch;
    show_message("--INSERT (REPLACE)--");
    contents.is_inserting = true;
    if (get_contents().refresh) {
        print_contents(get_contents());
        show_message("--INSERT (REPLACE)--");
    }
    while ((ch = getch()) != QUIT_KEY) {
        if (ch == '\n') {
            o = contents.cont[contents.y][contents.x];
            std::vector<std::shared_ptr<change> > changes;
            changes.reserve(3);
            if (o.size())
                changes.push_back(
                    std::make_shared<replace_c>(o, n, contents.y, x));
            changes.push_back(std::make_shared<newline_c>(contents));
            changes.back()->redo(contents);
            auto recursed = enter_replace_mode(contents, pref);
            if (recursed)
                changes.push_back(recursed.get());
            return boost::optional<std::shared_ptr<change> >(
                std::make_shared<concat_c>(changes));
        }
        if (contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
            contents.x = contents.cont[contents.y].size();
            n += ch;
        } else {
            o += contents.cont[contents.y][contents.x];
            n += ch;
            contents.cont[contents.y][contents.x] = ch;
            contents.x++;
        }
        if (get_contents().refresh) {
            print_contents(get_contents());
            show_message("--INSERT (REPLACE)--");
        }
    }
    contents.is_inserting = false;
    showing_message = false;
    return boost::optional<std::shared_ptr<change> >(
        std::make_shared<replace_c>(o, n, contents.y, x));
}

struct append_c : public change {
    const std::string track;
    const move_t y, x;
    append_c(const std::string& track, move_t y, move_t x)
        : track(track)
        , y(y)
        , x(x) {}
    virtual bool is_overriding() const noexcept override {
        return true;
    }
    virtual void undo(contents& contents) override {
        contents.cont[y] = contents.cont[y].substr(0, x) +
                           contents.cont[y].substr(x + track.size());
        contents.y = y;
        contents.x = x;
        if (contents.x and contents.x >= contents.cont[y].size())
            contents.x = contents.cont[y].size() - 1;
    }
    virtual void redo(contents& contents) override {
        contents.cont[y] = contents.cont[y].substr(0, x) + track +
                           contents.cont[y].substr(x);
        contents.y = y;
        contents.x = x + track.size();
        if (contents.x >= contents.cont[y].size())
            contents.x = contents.cont[y].size() - 1;
    }
    virtual std::shared_ptr<change>
    regenerate(const contents& contents) const override {
        return std::make_shared<append_c>(track, contents.y,
                                          contents.x + 1);
    }
};

boost::optional<std::shared_ptr<change> >
enter_append_mode(contents& contents, boost::optional<int> pref) {
    if (contents.cont[contents.y].empty())
        return enter_insert_mode(contents, pref);
    contents.x++;
    std::string track;
    auto x = contents.x;
    char ch;
    show_message("--INSERT--");
    contents.is_inserting = true;
    if (get_contents().refresh) {
        print_contents(get_contents());
        show_message("--INSERT--");
    }
    while ((ch = getch()) != QUIT_KEY) {
        if (ch == '\n') {
            std::vector<std::shared_ptr<change> > changes;
            changes.reserve(3);
            if (track.size())
                changes.push_back(
                    std::make_shared<insert_c>(track, contents.y, x));
            changes.push_back(std::make_shared<newline_c>(contents));
            changes.back()->redo(contents);
            auto recursed = enter_insert_mode(contents, pref);
            if (recursed)
                changes.push_back(recursed.get());
            return boost::optional<std::shared_ptr<change> >(
                std::make_shared<concat_c>(changes));
        }
        if (contents.x >= contents.cont[contents.y].size()) {
            contents.cont[contents.y].push_back(ch);
            contents.x = contents.cont[contents.y].size();
            track += ch;
        } else {
            contents.cont[contents.y].insert(contents.x, 1, ch);
            contents.x++;
            track += ch;
        }
        if (get_contents().refresh) {
            print_contents(get_contents());
            show_message("--INSERT--");
        }
    }
    contents.is_inserting = false;
    showing_message = false;
    // cancel out ++ from beginning
    if (contents.x != 0)
        contents.x--;
    return boost::optional<std::shared_ptr<change> >(
        std::make_shared<append_c>(track, contents.y, x));
}
}
}
