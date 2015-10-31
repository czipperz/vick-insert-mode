#include "../../../src/contents.hh"
#include "../../../src/concat_c.hh"
#include "insert_mode.hh"

struct open_line_c : public change {
    const int num_lines;
    const move_t y;
    open_line_c(size_t num_lines, move_t y) : num_lines(num_lines), y(y)
    {
        if(!num_lines) std::terminate();
    }
    virtual bool is_overriding()
    {
        return true;
    }
    virtual void undo(contents& contents)
    {
        if (num_lines < 0) {
            contents.cont.erase(contents.cont.begin() + y,
                                contents.cont.begin() + y - num_lines);
            // contents.y = y // + num_lines
                // ;
        } else {
            contents.cont.erase(contents.cont.begin() + y,
                                contents.cont.begin() + y + num_lines);
            // contents.y = y + num_lines;
        }
        contents.y = y;
        contents.x = 0;
    }
    virtual void redo(contents& contents)
    {
        if (num_lines < 0) {
            for (int x = 0; x > num_lines; x--)
                contents.cont.insert(contents.cont.begin() + y + x, "");
            contents.y = y // + num_lines
                ;
        } else {
            for (int x = 1; x <= num_lines; x++)
                contents.cont.insert(contents.cont.begin() + y + x, "");
            contents.y = y + num_lines;
        }
        contents.x = 0;
    }
    virtual std::shared_ptr<change> regenerate(const contents& contents) const
    {
        return std::make_shared<open_line_c>(num_lines, contents.y);
    }
};

boost::optional<std::shared_ptr<change> >
open_line_below(contents& contents, boost::optional<int> num)
{
    if (num && !num.get()) return boost::none;
    std::shared_ptr<change> open_line =
        std::make_shared<open_line_c>(num ? num.get() : 1, contents.y);
    open_line->redo(contents);
    auto insert = enter_insert_mode(contents);
    if (insert) {
        auto list{open_line, insert.get()};
        return boost::optional<std::shared_ptr<change> >(
            std::make_shared<concat_c>(list));
    } else {
        return open_line;
    }
}

boost::optional<std::shared_ptr<change> >
open_line_above(contents& contents, boost::optional<int> num)
{
    return open_line_below(contents, num ? -num.get() : -1);
}
