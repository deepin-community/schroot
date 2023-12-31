/* Copyright © 2005-2009,2012  Roger Leigh <rleigh@codelibre.net>
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

#ifndef SBUILD_CHROOT_FACET_USERDATA_H
#define SBUILD_CHROOT_FACET_USERDATA_H

#include <sbuild/sbuild-chroot-facet.h>
#include <sbuild/sbuild-custom-error.h>
#include <sbuild/sbuild-types.h>

namespace sbuild
{

  /**
   * Chroot support for extensible user metadata.
   *
   * This facet contains user-specific configuration, both additional
   * keys in schroot.conf, and also set from the command-line.
   */
  class chroot_facet_userdata : public chroot_facet
  {
  public:
    /// Error codes.
    enum error_code
      {
        ENV_AMBIGUOUS,  ///< Environment variable name is ambiguous.
        KEY_AMBIGUOUS,  ///< Configuration key name is ambiguous.
        KEY_DISALLOWED, ///< Configuration key is not allowed to be modified.
        KEYNAME_INVALID ///< Invalid name for configuration key.
      };

    /// Exception type.
    typedef custom_error<error_code> error;

    /// A shared_ptr to a chroot facet object.
    typedef std::shared_ptr<chroot_facet_userdata> ptr;

    /// A shared_ptr to a const chroot facet object.
    typedef std::shared_ptr<const chroot_facet_userdata> const_ptr;

  private:
    /// The constructor.
    chroot_facet_userdata ();

  public:
    /// The destructor.
    virtual ~chroot_facet_userdata ();

    /**
     * Create a chroot facet.
     *
     * @returns a shared_ptr to the new chroot facet.
     */
    static ptr
    create ();

    virtual chroot_facet::ptr
    clone () const;

    virtual std::string const&
    get_name () const;

    virtual void
    setup_env (chroot const& chroot,
               environment&  env) const;

    virtual chroot::session_flags
    get_session_flags (chroot const& chroot) const;

    virtual void
    get_details (chroot const&  chroot,
                 format_detail& detail) const;

    virtual void
    get_keyfile (chroot const& chroot,
                 keyfile&      keyfile) const;

    virtual void
    set_keyfile (chroot&        chroot,
                 keyfile const& keyfile,
                 string_list&   used_keys);

    /**
     * Get user data as a map of key-value pairs.
     *
     * @returns a reference to a string map.
     */
    string_map const&
    get_data () const;

    /**
     * Get the value of a single user data key.
     *
     * @param key the key to search for.
     * @param value the string to store the key's value in.  Only
     * modified if the key is found.
     * @returns true if found, false if not found.
     */
    bool
    get_data (std::string const& key,
              std::string&       value) const;

    /**
     * Set user data from a string map.  Note that this method does
     * not perform permissions checking.
     *
     * @param data the user data to set.
     */
    void
    set_data (string_map const& data);

    /**
     * Set a single key-value pair.  Note that this method does not
     * perform permissions checking.
     *
     * @param key the key to set.
     * @param value the value of the key.
     */
    void
    set_data (std::string const& key,
              std::string const& value);

    /**
     * Set a single key-value pair.  Note that this method does not
     * perform permissions checking or key name validation.
     *
     * @param key the key to set.
     * @param value the value of the key.
     */
    void
    set_system_data (std::string const& key,
                     std::string const& value);

    /**
     * Remove a single key.  If present, the specified key is removed.
     *
     * @param key the key to remove.
     */
    void
    remove_data (std::string const& key);

    /**
     * Get the set of keys allowed to be modified by a user.
     *
     * @returns a string set of keys.
     */
    string_set const&
    get_user_modifiable_keys () const;

    /**
     * Set the set of keys allowed to be modified by a user.
     *
     * @param keys a string set of keys.
     */
    void
    set_user_modifiable_keys (string_set const& keys);

    /**
     * Get the set of keys allowed to be modified by root.
     *
     * @returns a string set of keys.
     */
    string_set const&
    get_root_modifiable_keys () const;

    /**
     * Set the set of keys allowed to be modified by root.
     *
     * @param keys a string set of keys.
     */
    void
    set_root_modifiable_keys (string_set const& keys);

    /**
     * Set data for the current user.  Only keys set using
     * set_user_modifiable_keys() are permitted to be set, otherwise
     * an exception will be thrown.
     *
     * @param string_map a map of key-value pairs.
     */
    void
    set_user_data(string_map const&  data);

    /**
     * Set data for root.  Only keys set using
     * set_user_modifiable_keys() and set_root_modifiable_keys() are
     * permitted to be set, otherwise an exception will be thrown.
     *
     * @param data a map of key-value pairs.
     */
    void
    set_root_data(string_map const&  data);

    /**
     * Set data without user or root checks.
     *
     * @param string_map a map of key-value pairs.
     */
    void
    set_system_data(string_map const&  data);

  private:
    /**
     * Generic function for setting data for any user.
     *
     * @param data a map of key-value pairs.
     * @param allowed_keys the keys which may be used.
     * @param root whether or not the user is the root user.
     */
    void
    set_data(string_map const&  data,
             string_set const&  allowed_keys,
             bool               root);

    /// Mapping between user keys and values.
    string_map userdata;
    /// Environment checking.
    string_set env;
    /// Keys modifiable by users.
    string_set user_modifiable_keys;
    /// Keys modifiable by root.
    string_set root_modifiable_keys;
  };

}

#endif /* SBUILD_CHROOT_FACET_USERDATA_H */

/*
 * Local Variables:
 * mode:C++
 * End:
 */
