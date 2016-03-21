#ifndef HEADER_GUARD_INSERT_MODE_H
#define HEADER_GUARD_INSERT_MODE_H

#include "../../../src/contents.hh"

namespace vick {
namespace insert_mode {

/*!
 * \file insert_mode.hh
 * \brief Basic commands that have to do with inserting text into the
 * buffer
 *
 * These commands all rely on `global_insert_map` to dictate what keys
 * do special things.
 *
 * \see enter_insert_mode
 * \see enter_replace_mode
 * \see enter_append_mode
 * \see global_insert_map
 */

/*!
 * \brief Enters insert mode and prompts for user input continually
 *
 * Keys pressed are governed against `global_insert_map` before being
 * inserted raw
 *
 * EVERYTHING that happens in insert mode counts as one edit
 *
 * \see global_insert_map
 */
boost::optional<std::shared_ptr<change> >
enter_insert_mode(contents&, boost::optional<int> = boost::none);

/*!
 * \brief Similar to insert mode but overrides text
 *
 * Enter replace mode, a subgenre of insert mode.  Instead of
 * inserting text, you replace the text at the cursor (or append text
 * if already replaced last character in line).
 *
 * Keys pressed are governed against `global_insert_map` before being
 * inserted raw
 *
 * EVERYTHING that happens in replace mode counts as one edit
 *
 * \see enter_insert_mode()
 * \see global_insert_map
 */
boost::optional<std::shared_ptr<change> >
enter_replace_mode(contents&, boost::optional<int> = boost::none);

/*!
 * \brief Moves forward a character then enters insert mode
 *
 * EVERYTHING that happens in insert mode counts as one edit
 *
 * \see enter_insert_mode()
 * \see global_insert_map
 */
boost::optional<std::shared_ptr<change> >
enter_append_mode(contents&, boost::optional<int> = boost::none);
}
}

#endif
