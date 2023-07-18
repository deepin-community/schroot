/* Copyright © 2006-2008  Roger Leigh <rleigh@debian.org>
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

#include <sbuild/sbuild-chroot-zfs-snapshot.h>
#include <sbuild/sbuild-chroot-facet-mountable.h>
#include <sbuild/sbuild-i18n.h>
#include <sbuild/sbuild-util.h>

#include "test-helpers.h"
#include "test-sbuild-chroot.h"

#include <algorithm>
#include <set>

#include <cppunit/extensions/HelperMacros.h>

using namespace CppUnit;

using sbuild::_;

class chroot_zfs_snapshot : public sbuild::chroot_zfs_snapshot
{
public:
  chroot_zfs_snapshot():
    sbuild::chroot_zfs_snapshot()
  {}

  virtual ~chroot_zfs_snapshot()
  {}
};

class test_chroot_zfs_snapshot : public test_chroot_base<chroot_zfs_snapshot>
{
  CPPUNIT_TEST_SUITE(test_chroot_zfs_snapshot);
  CPPUNIT_TEST(test_snapshot_name);
  CPPUNIT_TEST(test_snapshot_options);
  CPPUNIT_TEST(test_chroot_type);
  CPPUNIT_TEST(test_setup_env);
  CPPUNIT_TEST(test_setup_env_session);
  CPPUNIT_TEST(test_setup_env_source);
  CPPUNIT_TEST(test_setup_env_session_source);
  CPPUNIT_TEST(test_setup_keyfile);
  CPPUNIT_TEST(test_setup_keyfile_session);
  CPPUNIT_TEST(test_setup_keyfile_source);
  CPPUNIT_TEST(test_setup_keyfile_session_source);
  CPPUNIT_TEST(test_session_flags);
  CPPUNIT_TEST(test_print_details);
  CPPUNIT_TEST(test_print_config);
  CPPUNIT_TEST(test_run_setup_scripts);
  CPPUNIT_TEST_SUITE_END();

public:
  test_chroot_zfs_snapshot():
    test_chroot_base<chroot_zfs_snapshot>()
  {}

  void setUp()
  {
    test_chroot_base<chroot_zfs_snapshot>::setUp();
    CPPUNIT_ASSERT(chroot);
    CPPUNIT_ASSERT(session);
    CPPUNIT_ASSERT(source);
    CPPUNIT_ASSERT(session_source);
  }

  virtual void setup_chroot_props (sbuild::chroot::ptr& chroot)
  {
    test_chroot_base<chroot_zfs_snapshot>::setup_chroot_props(chroot);

    std::shared_ptr<sbuild::chroot_zfs_snapshot> c = std::dynamic_pointer_cast<sbuild::chroot_zfs_snapshot>(chroot);

    c->set_dataset("schroot-pool/testdev");
    c->set_snapshot_options("-o checksum=off");

    sbuild::chroot_facet_mountable::ptr pmnt(chroot->get_facet<sbuild::chroot_facet_mountable>());
    CPPUNIT_ASSERT(pmnt);

    pmnt->set_mount_options("-t jfs -o quota,rw");
    pmnt->set_location("/squeeze");
  }

  void
  test_snapshot_name()
  {
    std::shared_ptr<sbuild::chroot_zfs_snapshot> c = std::dynamic_pointer_cast<sbuild::chroot_zfs_snapshot>(chroot);
    CPPUNIT_ASSERT(c);
    c->set_snapshot_name("some-snapshot-name");
    CPPUNIT_ASSERT(c->get_snapshot_name() == "some-snapshot-name");
  }

  void
  test_snapshot_options()
  {
    std::shared_ptr<sbuild::chroot_zfs_snapshot> c = std::dynamic_pointer_cast<sbuild::chroot_zfs_snapshot>(chroot);
    CPPUNIT_ASSERT(c);
    c->set_snapshot_options("-o opt1,opt2");
    CPPUNIT_ASSERT(c->get_snapshot_options() == "-o opt1,opt2");
  }

  void test_chroot_type()
  {
    CPPUNIT_ASSERT(chroot->get_chroot_type() == "zfs-snapshot");
  }

  void setup_env_gen(sbuild::environment &expected)
  {
    setup_env_chroot(expected);
    expected.add("CHROOT_LOCATION",       "/squeeze");
    expected.add("CHROOT_MOUNT_LOCATION", "/mnt/mount-location");
    expected.add("CHROOT_PATH",           "/mnt/mount-location/squeeze");
    expected.add("CHROOT_ZFS_DATASET",         "schroot-pool/testdev");
    expected.add("CHROOT_MOUNT_OPTIONS",  "-t jfs -o quota,rw");
  }

  void test_setup_env()
  {
    sbuild::environment expected;
    setup_env_gen(expected);
    expected.add("CHROOT_TYPE",           "zfs-snapshot");
    expected.add("CHROOT_ZFS_SNAPSHOT_OPTIONS", "-o checksum=off");
    expected.add("CHROOT_SESSION_CLONE",  "true");
    expected.add("CHROOT_SESSION_CREATE", "true");
    expected.add("CHROOT_SESSION_PURGE",  "false");
    expected.add("CHROOT_SESSION_SOURCE", "false");

    test_chroot_base<chroot_zfs_snapshot>::test_setup_env(chroot, expected);
  }

  void test_setup_env_session()
  {
    std::shared_ptr<sbuild::chroot_zfs_snapshot> c = std::dynamic_pointer_cast<sbuild::chroot_zfs_snapshot>(chroot);

    sbuild::environment expected;
    setup_env_gen(expected);
    expected.add("CHROOT_TYPE",           "zfs-snapshot");
    expected.add("SESSION_ID",            "test-session-name");
    expected.add("CHROOT_ALIAS",          "test-session-name");
    expected.add("CHROOT_DESCRIPTION",     chroot->get_description() + ' ' + _("(session chroot)"));
    expected.add("CHROOT_MOUNT_DEVICE",   "schroot-pool/testdev/schroot-test-session-name");
    expected.add("CHROOT_ZFS_SNAPSHOT_NAME",    "schroot-pool/testdev@test-session-name");
    expected.add("CHROOT_ZFS_CLONE_NAME",   "schroot-pool/testdev/schroot-test-session-name");
    expected.add("CHROOT_ZFS_SNAPSHOT_OPTIONS", "-o checksum=off");
    expected.add("CHROOT_SESSION_CLONE",  "false");
    expected.add("CHROOT_SESSION_CREATE", "false");
    expected.add("CHROOT_SESSION_PURGE",  "true");
    expected.add("CHROOT_SESSION_SOURCE", "false");

    test_chroot_base<chroot_zfs_snapshot>::test_setup_env(session, expected);
  }

  void test_setup_env_source()
  {
    sbuild::environment expected;
    setup_env_gen(expected);
    expected.add("CHROOT_TYPE",           "zfs-snapshot");
    expected.add("CHROOT_NAME",           "test-name");
    expected.add("CHROOT_DESCRIPTION",     chroot->get_description() + ' ' + _("(source chroot)"));
    expected.add("CHROOT_SESSION_CLONE",  "false");
    expected.add("CHROOT_SESSION_CREATE", "true");
    expected.add("CHROOT_SESSION_PURGE",  "false");
    expected.add("CHROOT_SESSION_SOURCE", "false");

    test_chroot_base<chroot_zfs_snapshot>::test_setup_env(source, expected);
  }

  void test_setup_env_session_source()
  {
    sbuild::environment expected;
    setup_env_gen(expected);
    expected.add("CHROOT_TYPE",           "zfs-snapshot");
    expected.add("CHROOT_NAME",           "test-name");
    expected.add("SESSION_ID",            "test-session-name");
    expected.add("CHROOT_DESCRIPTION",     chroot->get_description() + ' ' + _("(source chroot) (session chroot)"));
    expected.add("CHROOT_ALIAS",          "test-session-name");
    expected.add("CHROOT_MOUNT_DEVICE",   "schroot-pool/testdev");
    expected.add("CHROOT_SESSION_CLONE",  "false");
    expected.add("CHROOT_SESSION_CREATE", "false");
    expected.add("CHROOT_SESSION_PURGE",  "true");
    expected.add("CHROOT_SESSION_SOURCE", "true");

    test_chroot_base<chroot_zfs_snapshot>::test_setup_env(session_source, expected);
  }

  void setup_keyfile_zfs(sbuild::keyfile &expected, std::string group)
  {
    expected.set_value(group, "location", "/squeeze");
    expected.set_value(group, "mount-options", "-t jfs -o quota,rw");
  }

  void test_setup_keyfile()
  {
    sbuild::keyfile expected;
    std::string group = chroot->get_name();
    setup_keyfile_chroot(expected, group);
    setup_keyfile_source(expected, group);
    setup_keyfile_zfs(expected, group);
    expected.set_value(group, "type", "zfs-snapshot");
    expected.set_value(group, "zfs-snapshot-options", "-o checksum=off");
    expected.set_value(group, "zfs-dataset-name", "schroot-pool/testdev");

    test_chroot_base<chroot_zfs_snapshot>::test_setup_keyfile
      (chroot,expected, chroot->get_name());
  }

  void test_setup_keyfile_session()
  {
    sbuild::keyfile expected;
    const std::string group(session->get_name());
    setup_keyfile_session(expected, group);
    setup_keyfile_zfs(expected, group);
    expected.set_value(group, "type", "zfs-snapshot");
    expected.set_value(group, "name", "test-session-name");
    expected.set_value(group, "selected-name", "test-session-name");
    expected.set_value(group, "description", chroot->get_description() + ' ' + _("(session chroot)"));
    expected.set_value(group, "aliases", "");
    expected.set_value(group, "zfs-snapshot-name", "schroot-pool/testdev@test-session-name");
    expected.set_value(group, "zfs-clone-name", "schroot-pool/testdev/schroot-test-session-name");
    expected.set_value(group, "mount-device", "schroot-pool/testdev/schroot-test-session-name");
    expected.set_value(group, "mount-location", "/mnt/mount-location");

    test_chroot_base<chroot_zfs_snapshot>::test_setup_keyfile
      (session, expected, group);
  }

  void test_setup_keyfile_source()
  {
    sbuild::keyfile expected;
    const std::string group(source->get_name());
    setup_keyfile_chroot(expected, group);
    setup_keyfile_zfs(expected, group);
    expected.set_value(group, "type", "zfs-snapshot");
    expected.set_value(group, "description", chroot->get_description() + ' ' + _("(source chroot)"));
    expected.set_value(group, "aliases", "test-name-source,test-alias-1-source,test-alias-2-source");
    expected.set_value(group, "zfs-snapshot-options", "-o checksum=off");
    expected.set_value(group, "zfs-dataset-name", "schroot-pool/testdev");
    setup_keyfile_source_clone(expected, group);

    test_chroot_base<chroot_zfs_snapshot>::test_setup_keyfile
      (source, expected, group);
  }

  void test_setup_keyfile_session_source()
  {
    sbuild::keyfile expected;
    const std::string group(source->get_name());
    setup_keyfile_chroot(expected, group);
    setup_keyfile_zfs(expected, group);
    expected.set_value(group, "type", "zfs-snapshot");
    expected.set_value(group, "mount-device", "schroot-pool/testdev");
    expected.set_value(group, "mount-location", "/mnt/mount-location");
    expected.set_value(group, "zfs-clone-name", "schroot-pool/testdev/schroot-test-session-name");
    expected.set_value(group, "zfs-snapshot-name", "schroot-pool/testdev@test-session-name");
    setup_keyfile_session_source_clone(expected, group);

    test_chroot_base<chroot_zfs_snapshot>::test_setup_keyfile
      (session_source, expected, group);
  }

  void test_session_flags()
  {
    CPPUNIT_ASSERT(chroot->get_session_flags() ==
                   (sbuild::chroot::SESSION_CREATE |
                    sbuild::chroot::SESSION_CLONE));

    CPPUNIT_ASSERT(session->get_session_flags() ==
                   (sbuild::chroot::SESSION_PURGE));

    /// @todo: Should return NOFLAGS?  This depends upon if source
    /// chroots need transforming into sessions as well (which should
    /// probably happen and be tested for independently).
    CPPUNIT_ASSERT(source->get_session_flags() ==
                   (sbuild::chroot::SESSION_CREATE));
  }

  void test_print_details()
  {
    std::ostringstream os;
    os << chroot;
    // TODO: Compare output.
    CPPUNIT_ASSERT(!os.str().empty());
  }

  void test_print_config()
  {
    std::ostringstream os;
    sbuild::keyfile config;
    config << chroot;
    os << config;
    // TODO: Compare output.
    CPPUNIT_ASSERT(!os.str().empty());
  }

  void test_run_setup_scripts()
  {
    CPPUNIT_ASSERT(chroot->get_run_setup_scripts());
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(test_chroot_zfs_snapshot);
