#ifndef HEADER_GUARD_JOIN_H
#define HEADER_GUARD_JOIN_H

#include "../../../src/contents.hh"

/*!
 * \file join.hh
 *
 * \brief Combines the line at ``contents.y`` with that at ``contents.y + 1``.
 */
boost::optional<std::shared_ptr<change> >
join_two_lines(contents& contents, boost::optional<int> unused = boost::none);

#endif
