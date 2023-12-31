/* Copyright © 2005-2007  Roger Leigh <rleigh@codelibre.net>
 *
 * schroot is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * schroot is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 *********************************************************************/

#ifndef SBUILD_AUTH_PAM_MESSAGE_H
#define SBUILD_AUTH_PAM_MESSAGE_H

#include <string>

#include <security/pam_appl.h>

namespace sbuild
{

  /**
   * Authentication messages.
   *
   * When auth_pam needs to interact with the user, it does this by
   * sending a list of auth_pam_message objects to an auth_pam_conv
   * conversation object.  These messages tell the conversation object
   * how to display the message to the user, and if necessary, whether
   * or not to ask the user for some input.  They also store the
   * user's input, if required.
   */
  class auth_pam_message
  {
  public:
    /// Message type
    enum message_type
      {
        /// Display a prompt, with no echoing of user input.
        MESSAGE_PROMPT_NOECHO = PAM_PROMPT_ECHO_OFF,
        /// Display a prompt, echoing user input.
        MESSAGE_PROMPT_ECHO = PAM_PROMPT_ECHO_ON,
        /// Display an error message.
        MESSAGE_ERROR = PAM_ERROR_MSG,
        /// Display an informational message.
        MESSAGE_INFO = PAM_TEXT_INFO
      };

    /**
     * The constructor.
     *
     * @param type the type of message.
     * @param message the message to display.
     */
    auth_pam_message (message_type       type,
                      std::string const& message);

    /// The destructor.
    virtual ~auth_pam_message ();

    /// The type of message.
    message_type type;
    /// The message to display.
    std::string  message;
    /// The user's response (if any).
    std::string  response;
  };

}

#endif /* SBUILD_AUTH_PAM_MESSAGE_H */

/*
 * Local Variables:
 * mode:C++
 * End:
 */
