#ifndef HEADER_GUARD_OPEN_LINE_H
#define HEADER_GUARD_OPEN_LINE_H

#include "../../../src/contents.hh"

/*!
 * \file open_line.hh
 * \brief Describes the open_line_below() and open_line_above() functions to insert newlines.
 */

/*!
 * \brief Makes a given number of newlines after the current line and enters insert mode
 */
boost::optional<std::shared_ptr<change> >
open_line_below(contents&, boost::optional<int> = 1);

/*!
 * \brief Makes a given number of newlines before the current line and enters insert mode
 */
boost::optional<std::shared_ptr<change> >
open_line_above(contents&, boost::optional<int> = 1);

#endif
