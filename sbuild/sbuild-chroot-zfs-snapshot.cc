/* Copyright © 2005-2009  Roger Leigh <rleigh@debian.org>
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

#include <config.h>

#include "sbuild-chroot-zfs-snapshot.h"
#include "sbuild-chroot-facet-session.h"
#include "sbuild-chroot-facet-session-clonable.h"
#include "sbuild-chroot-facet-source-clonable.h"
#include "sbuild-chroot-facet-source.h"
#include "sbuild-chroot-facet-mountable.h"
#include "sbuild-format-detail.h"

#include <cassert>
#include <cerrno>

#include <boost/format.hpp>

using std::endl;
using boost::format;
using namespace sbuild;

chroot_zfs_snapshot::chroot_zfs_snapshot ():
  chroot(),
  dataset(),
  clone_name(),
  snapshot_name(),
  snapshot_options()
{
  add_facet(chroot_facet_source_clonable::create());
  add_facet(chroot_facet_mountable::create());
}

chroot_zfs_snapshot::chroot_zfs_snapshot (const chroot_zfs_snapshot& rhs):
  chroot(rhs),
  dataset(rhs.dataset),
  clone_name(rhs.clone_name),
  snapshot_name(rhs.snapshot_name),
  snapshot_options(rhs.snapshot_options)
{
}

chroot_zfs_snapshot::~chroot_zfs_snapshot ()
{
}

sbuild::chroot::ptr
chroot_zfs_snapshot::clone () const
{
  return ptr(new chroot_zfs_snapshot(*this));
}

sbuild::chroot::ptr
chroot_zfs_snapshot::clone_session (std::string const& session_id,
                                    std::string const& alias,
                                    std::string const& user,
                                    bool               root) const
{
  chroot_facet_session_clonable::const_ptr psess
    (get_facet<chroot_facet_session_clonable>());
  assert(psess);

  ptr session(new chroot_zfs_snapshot(*this));
  psess->clone_session_setup(*this, session, session_id, alias, user, root);

  return session;
}

sbuild::chroot::ptr
chroot_zfs_snapshot::clone_source () const
{
  ptr clone(new chroot_zfs_snapshot(*this));

  chroot_facet_source_clonable::const_ptr psrc
    (get_facet<chroot_facet_source_clonable>());
  assert(psrc);

  psrc->clone_source_setup(*this, clone);

  return clone;
}

std::string const&
chroot_zfs_snapshot::get_dataset () const
{
  return this->dataset;
}

void
chroot_zfs_snapshot::set_dataset (std::string const& dataset)
{
  this->dataset = dataset;
}

std::string const&
chroot_zfs_snapshot::get_clone_name () const
{
  return this->clone_name;
}

void
chroot_zfs_snapshot::set_clone_name (std::string const& clone_name)
{
  this->clone_name = clone_name;

  chroot_facet_mountable::ptr pmnt
    (get_facet<chroot_facet_mountable>());
  if (pmnt)
    if (! get_facet<chroot_facet_source>())
      pmnt->set_mount_device(this->clone_name);
    else
      pmnt->set_mount_device(get_dataset());
}

std::string const&
chroot_zfs_snapshot::get_snapshot_name () const
{
  return this->snapshot_name;
}

void
chroot_zfs_snapshot::set_snapshot_name (std::string const& snapshot_name)
{
  this->snapshot_name = snapshot_name;
}

std::string const&
chroot_zfs_snapshot::get_snapshot_options () const
{
  return this->snapshot_options;
}

void
chroot_zfs_snapshot::set_snapshot_options (std::string const& snapshot_options)
{
  this->snapshot_options = snapshot_options;
}

std::string const&
chroot_zfs_snapshot::get_chroot_type () const
{
  static const std::string type("zfs-snapshot");

  return type;
}

std::string
chroot_zfs_snapshot::get_path () const
{
  chroot_facet_mountable::const_ptr pmnt
    (get_facet<chroot_facet_mountable>());

  std::string path(get_mount_location());

  if (pmnt)
    path += pmnt->get_location();

  return path;
}

void
chroot_zfs_snapshot::setup_env (chroot const& chroot,
                                environment&  env) const
{
  chroot::setup_env(chroot, env);

  env.add("CHROOT_ZFS_DATASET", get_dataset());

  // if this is a source chroot, avoid configuring snapshotting.
  if (chroot.get_facet<chroot_facet_source>())
    return;

  env.add("CHROOT_ZFS_SNAPSHOT_NAME", get_snapshot_name());
  env.add("CHROOT_ZFS_CLONE_NAME", get_clone_name());
  env.add("CHROOT_ZFS_SNAPSHOT_OPTIONS", get_snapshot_options());
}

void
chroot_zfs_snapshot::setup_lock (chroot::setup_type type,
                                 bool               lock,
                                 int                status)
{
  /* Create or unlink session information. */
  if ((type == SETUP_START && lock == true) ||
      (type == SETUP_STOP && lock == false && status == 0))
    {
      bool start = (type == SETUP_START);
      setup_session_info(start);
    }
}

sbuild::chroot::session_flags
chroot_zfs_snapshot::get_session_flags (chroot const& chroot) const
{
  session_flags flags = SESSION_NOFLAGS;

  if (get_facet<chroot_facet_session>())
    flags = flags | SESSION_PURGE;

  return flags;
}

void
chroot_zfs_snapshot::get_details (chroot const& chroot,
                                  format_detail& detail) const
{
  chroot::get_details(chroot, detail);

  if (!this->dataset.empty())
    detail.add(_("ZFS Source Dataset"), get_dataset());
  if (!this->snapshot_name.empty())
    detail.add(_("ZFS Snapshot Name"), get_snapshot_name());
  if (!this->clone_name.empty())
    detail.add(_("ZFS Clone Dataset"), get_clone_name());
  if (!this->snapshot_options.empty())
    detail.add(_("ZFS Snapshot Options"), get_snapshot_options());
}

void
chroot_zfs_snapshot::get_keyfile (chroot const& chroot,
                                  keyfile& keyfile) const
{
  chroot::get_keyfile(chroot, keyfile);

  bool session = static_cast<bool>(get_facet<chroot_facet_session>());

  if (session)
  {
    keyfile::set_object_value(*this,
                              &chroot_zfs_snapshot::get_snapshot_name,
                              keyfile, get_name(),
                              "zfs-snapshot-name");
    keyfile::set_object_value(*this,
                              &chroot_zfs_snapshot::get_clone_name,
                              keyfile, get_name(),
                              "zfs-clone-name");
  }

  if (!session)
  {
    keyfile::set_object_value(*this,
                              &chroot_zfs_snapshot::get_dataset,
                              keyfile, get_name(),
                              "zfs-dataset-name");
    keyfile::set_object_value(*this,
                              &chroot_zfs_snapshot::get_snapshot_options,
                              keyfile, get_name(),
                              "zfs-snapshot-options");
  }
}

void
chroot_zfs_snapshot::set_keyfile (chroot&        chroot,
                                  keyfile const& keyfile,
                                  string_list&   used_keys)
{
  chroot::set_keyfile(chroot, keyfile, used_keys);

  bool session = static_cast<bool>(get_facet<chroot_facet_session>());

  keyfile::get_object_value(*this, &chroot_zfs_snapshot::set_dataset,
                            keyfile, get_name(), "zfs-dataset",
                            session ?
                            keyfile::PRIORITY_DISALLOWED :
                            keyfile::PRIORITY_REQUIRED);
  used_keys.push_back("zfs-dataset");

  keyfile::get_object_value(*this, &chroot_zfs_snapshot::set_snapshot_name,
                            keyfile, get_name(), "zfs-snapshot-name",
                            session ?
                            keyfile::PRIORITY_REQUIRED :
                            keyfile::PRIORITY_DISALLOWED);
  used_keys.push_back("zfs-snapshot-name");

  keyfile::get_object_value(*this, &chroot_zfs_snapshot::set_clone_name,
                            keyfile, get_name(), "zfs-clone-name",
                            session ?
                            keyfile::PRIORITY_REQUIRED :
                            keyfile::PRIORITY_DISALLOWED);
  used_keys.push_back("zfs-clone-name");

  keyfile::get_object_value(*this, &chroot_zfs_snapshot::set_snapshot_options,
                            keyfile, get_name(), "zfs-snapshot-options",
                            session ?
                            keyfile::PRIORITY_DISALLOWED :
                            keyfile::PRIORITY_OPTIONAL); // Only needed for creating snapshot, not using snapshot
  used_keys.push_back("zfs-snapshot-options");
}
