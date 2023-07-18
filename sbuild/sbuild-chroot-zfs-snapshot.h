/* Copyright © 2005-2008  Roger Leigh <rleigh@debian.org>
 * Copyright © 2019       Steve Langasek <vorlon@debian.org>
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

#ifndef SBUILD_CHROOT_ZFS_SNAPSHOT_H
#define SBUILD_CHROOT_ZFS_SNAPSHOT_H

#include <sbuild/sbuild-chroot.h>

namespace sbuild
{

  /**
   * A chroot stored on a ZFS dataset.
   *
   * A clone dataset will be created and mounted on demand.
   */
  class chroot_zfs_snapshot : public chroot
  {
  protected:
    /// The constructor.
    chroot_zfs_snapshot ();

    /// The copy constructor.
    chroot_zfs_snapshot (const chroot_zfs_snapshot& rhs);

    friend class chroot;

  public:
    /// The destructor.
    virtual ~chroot_zfs_snapshot ();

    virtual chroot::ptr
    clone () const;

    virtual chroot::ptr
    clone_session (std::string const& session_id,
                   std::string const& alias,
                   std::string const& user,
                   bool               root) const;

    virtual chroot::ptr
    clone_source () const;

    /**
     * Get the ZFS source dataset name.  This is used by "zfs clone".
     *
     * @returns the dataset name.
     */
    std::string const&
    get_dataset () const;

    /**
     * Set the ZFS source dataset name.  This is used by "zfs clone".
     *
     * @param dataset the source dataset name.
     */
    void
    set_dataset (std::string const& dataset);

    /**
     * Get the ZFS clone name.  This is used by "zfs clone".
     *
     * @returns the clone name.
     */
    std::string const&
    get_clone_name () const;

    /**
     * Set the clone name.  This is used by "zfs clone".
     *
     * @param clone_name the clone name.
     */
    void
    set_clone_name (std::string const& clone_name);

    /**
     * Get the ZFS snapshot name.  This is used by "zfs snapshot".
     *
     * @returns the snapshot name.
     */
    std::string const&
    get_snapshot_name () const;

    /**
     * Set the snapshot name.  This is used by "zfs snapshot".
     *
     * @param snapshot_name the snapshot name.
     */
    void
    set_snapshot_name (std::string const& snapshot_name);

    /**
     * Get the ZFS snapshot options.  These are used by "zfs snapshot".
     *
     * @returns the options.
     */
    std::string const&
    get_snapshot_options () const;

    /**
     * Set the ZFS snapshot options.  These are used by "zfs snapshot".
     *
     * @param snapshot_options the options.
     */
    void
    set_snapshot_options (std::string const& snapshot_options);

    virtual std::string const&
    get_chroot_type () const;

    virtual std::string
    get_path () const;

    virtual void
    setup_env (chroot const& chroot,
               environment&  env) const;

    virtual session_flags
    get_session_flags (chroot const& chroot) const;

  protected:
    virtual void
    setup_lock (chroot::setup_type type,
                bool               lock,
                int                status);

    virtual void
    get_details (chroot const& chroot,
                 format_detail& detail) const;

    virtual void
    get_keyfile (chroot const& chroot,
                 keyfile& keyfile) const;

    virtual void
    set_keyfile (chroot&        chroot,
                 keyfile const& keyfile,
                 string_list&   used_keys);

  private:
    /// ZFS source dataset
    std::string dataset;
    /// ZFS clone name for "zfs clone"
    std::string clone_name;
    /// ZFS snapshot name for "zfs snapshot"
    std::string snapshot_name;
    /// ZFS snapshot options for "zfs snapshot"
    std::string snapshot_options;
  };

}

#endif /* SBUILD_CHROOT_ZFS_SNAPSHOT_H */

/*
 * Local Variables:
 * mode:C++
 * End:
 */
