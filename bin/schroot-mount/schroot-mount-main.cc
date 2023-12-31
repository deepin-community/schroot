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

#include <config.h>

#include <sbuild/sbuild-mntstream.h>
#include <sbuild/sbuild-util.h>

#if defined (__linux__)
#include <sbuild/sbuild-regex.h>
#endif

#include "schroot-mount-main.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <locale>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <mntent.h>

using std::endl;
using boost::format;
using sbuild::_;
using sbuild::N_;
using namespace schroot_mount;

namespace
{

  typedef std::pair<main::error_code,const char *> emap;

  /**
   * This is a list of the supported error codes.  It's used to
   * construct the real error codes map.
   */
  emap init_errors[] =
    {
      emap(main::CHILD_FORK, N_("Failed to fork child")),
      emap(main::CHILD_WAIT, N_("Wait for child failed")),
      // TRANSLATORS: %1% = directory
      emap(main::CHROOT,     N_("Failed to change root to directory ‘%1%’")),
      emap(main::DUP,        N_("Failed to duplicate file descriptor")),
      // TRANSLATORS: %1% = command name
      emap(main::EXEC,       N_("Failed to execute “%1%”")),
      emap(main::PIPE,       N_("Failed to create pipe")),
      emap(main::POLL,       N_("Failed to poll file descriptor")),
      emap(main::READ,       N_("Failed to read file descriptor")),
      emap(main::REALPATH,   N_("Failed to resolve path “%1%”"))
    };

}

template<>
sbuild::error<main::error_code>::map_type
sbuild::error<main::error_code>::error_strings
(init_errors,
 init_errors + (sizeof(init_errors) / sizeof(init_errors[0])));

main::main (options::ptr& options):
  schroot_base::main("schroot-mount",
                     // TRANSLATORS: '...' is an ellipsis e.g. U+2026,
                     // and '-' is an em-dash.
                     _("[OPTION…] — mount filesystems"),
                     options,
                     false),
  opts(options)
{
}

main::~main ()
{
}

std::string
main::resolve_path_chrooted (std::string const& mountpoint)
{
  std::string directory(mountpoint);
  if (directory.empty() || directory[0] != '/')
    directory = std::string("/") + directory;

  // Canonicalise path to remove any symlinks.
  char *resolved_path = realpath(directory.c_str(), 0);
  if (resolved_path == 0)
    {
      // The path is either not present or is an invalid link.  If
      // it's not present, we'll create it later.  If it's a link,
      // bail out now.
      bool link = false;
      try
        {
          if (sbuild::stat(directory, true).is_link())
            link = true;
        }
      catch (...)
        {} // Does not exist, not a link

      if (link)
        throw error(directory, REALPATH, strerror(ENOTDIR));
      else
        {
          // Try validating the parent directory.
          sbuild::string_list dirs = sbuild::split_string(directory, "/");
          if (dirs.size() > 1) // Recurse if possible, otherwise continue
            {
              std::string saveddir = *dirs.rbegin();
              dirs.pop_back();

              std::string newpath(resolve_path_chrooted(sbuild::string_list_to_string(dirs, "/")));
              directory = newpath + "/" + saveddir;
            }
        }
    }
  else
    {
      directory = resolved_path;
      std::free(resolved_path);
    }

  return directory;
}

std::string
main::resolve_path (std::string const& mountpoint)
{
  std::string stdout_buf;
  int stdout_pipe[2];
  int exit_status = 0;
  pid_t pid;

  try
    {
      if (pipe(stdout_pipe) < 0)
        throw error(PIPE, strerror(errno));

      if ((pid = fork()) == -1)
        {
          throw error(CHILD_FORK, strerror(errno));
        }
      else if (pid == 0)
        {
          try
            {
              // Set up pipes for stdout
              if (dup2(stdout_pipe[1], STDOUT_FILENO) < 0)
                throw error(DUP, strerror(errno));

              close(stdout_pipe[0]);
              close(stdout_pipe[1]);

              char *resolved_path = realpath(opts->mountpoint.c_str(), 0);
              if (!resolved_path)
                throw error(opts->mountpoint, REALPATH, strerror(errno));

              std::string basepath(resolved_path);
              std::free(resolved_path);

              if (chroot(basepath.c_str()) < 0)
                throw error(basepath, CHROOT, strerror(errno));

              std::cout << basepath + resolve_path_chrooted(mountpoint);
              std::cout.flush();
              _exit(EXIT_SUCCESS);
            }
          catch (std::exception const& e)
            {
              sbuild::log_exception_error(e);
            }
          catch (...)
            {
              sbuild::log_error()
                << _("An unknown exception occurred") << std::endl;
            }
          _exit(EXIT_FAILURE);
        }

      // Log stdout
      close(stdout_pipe[1]);

      struct pollfd pollfd;
      pollfd.fd = stdout_pipe[0];
      pollfd.events = POLLIN;
      pollfd.revents = 0;

      char buffer[BUFSIZ];

      while (1)
        {
          int status;

          if ((status = poll(&pollfd, 1, -1)) < 0)
            throw error(POLL, strerror(errno));

          int outdata = 0;

          if (pollfd.revents & POLLIN)
            {
              if ((outdata = read(pollfd.fd, buffer, BUFSIZ)) < 0
                  && errno != EINTR)
                throw error(READ, strerror(errno));

              if (outdata)
                stdout_buf += std::string(&buffer[0], outdata);
            }

          if (outdata == 0) // pipe closed
            break;
        }

      close(stdout_pipe[0]);
      wait_for_child(pid, exit_status);
    }
  catch (error const& e)
    {
      close(stdout_pipe[0]);
      close(stdout_pipe[1]);
      throw;
    }

  if (exit_status)
    exit(exit_status);

  return stdout_buf;
}

void
main::action_mount ()
{
  // Check mounts.
  sbuild::mntstream mounts(opts->fstab);

  sbuild::mntstream::mntentry entry;

  while (mounts >> entry)
    {
      std::string directory = resolve_path(entry.directory);

      if (!boost::filesystem::exists(directory))
        {
          sbuild::log_debug(sbuild::DEBUG_INFO)
            << boost::format("Creating ‘%1%' in '%2%’")
            % entry.directory
            % opts->mountpoint
            << std::endl;

          if (!opts->dry_run)
            {
              try
                {
                  boost::filesystem::create_directories(directory);
                }
              catch (std::exception const& e)
                {
                  sbuild::log_exception_error(e);
                  exit(EXIT_FAILURE);
                }
              catch (...)
                {
                  sbuild::log_error()
                    << _("An unknown exception occurred") << std::endl;
                  exit(EXIT_FAILURE);
                }
            }
        }

      sbuild::log_debug(sbuild::DEBUG_INFO)
        << boost::format("Mounting ‘%1%’ on ‘%2%’")
        % entry.filesystem_name
        % directory
        << std::endl;

      if (!opts->dry_run)
        {
          sbuild::string_list command;
          command.push_back("/bin/mount");
          if (opts->verbose)
            command.push_back("-v");
          command.push_back("-t");
          command.push_back(entry.type);
          command.push_back("-o");
          command.push_back(entry.options);
          command.push_back(entry.filesystem_name);
          command.push_back(directory);

          int status = run_child(command[0], command, sbuild::environment());

          if (status)
            exit(status);
        }

        make_mount_private(entry.options, directory);
    }
}

void
main::make_mount_private (const std::string& options,
                          const std::string& mountpoint)
{
#if defined (__linux__)
  static sbuild::regex bind("(^|,)(|r)bind(,|$)");
  static sbuild::regex propagation("(^|,)(|r)(shared|slave|private|unbindable)(,|$)");

  if (regex_search(options, bind) && !regex_search(options, propagation))
    {
      static sbuild::regex rbind("(^|,)rbind(,|$)");
      bool recursive = regex_search(options, rbind);

      sbuild::log_debug(sbuild::DEBUG_INFO)
      << boost::format("Making ‘%1%’ mount point %2%private")
      % mountpoint
      % (recursive ? "recursively " : "")
      << std::endl;

      if (!opts->dry_run)
        {
          sbuild::string_list command;
          command.push_back("/bin/mount");
          if (opts->verbose)
            command.push_back("-v");
          command.push_back(recursive ? "--make-rprivate" : "--make-private");
          command.push_back(mountpoint);

          int status = run_child(command[0], command, sbuild::environment());

          if (status)
            exit(status);
        }
    }
#endif
}

int
main::run_child (std::string const& file,
                 sbuild::string_list const& command,
                 sbuild::environment const& env)
{
  int exit_status = 0;
  pid_t pid;

  if ((pid = fork()) == -1)
    {
      throw error(CHILD_FORK, strerror(errno));
    }
  else if (pid == 0)
    {
      try
        {
          sbuild::log_debug(sbuild::DEBUG_INFO)
            << "mount_main: executing "
            << sbuild::string_list_to_string(command, ", ")
            << std::endl;
          exec(file, command, env);
          error e(file, EXEC, strerror(errno));
          sbuild::log_exception_error(e);
        }
      catch (std::exception const& e)
        {
          sbuild::log_exception_error(e);
        }
      catch (...)
        {
          sbuild::log_error()
            << _("An unknown exception occurred") << std::endl;
        }
      _exit(EXIT_FAILURE);
    }
  else
    {
      wait_for_child(pid, exit_status);
    }

  if (exit_status)
    sbuild::log_debug(sbuild::DEBUG_INFO)
      << "mount_main: " << file
      << " failed with status " << exit_status
      << std::endl;
  else
    sbuild::log_debug(sbuild::DEBUG_INFO)
      << "mount_main: " << file
      << " succeeded"
      << std::endl;

  return exit_status;
}

void
main::wait_for_child (pid_t pid,
                      int&  child_status)
{
  child_status = EXIT_FAILURE; // Default exit status

  int status;

  while (1)
    {
      if (waitpid(pid, &status, 0) == -1)
        {
          if (errno == EINTR)
            continue; // Wait again.
          else
            throw error(CHILD_WAIT, strerror(errno));
        }
      else
        break;
    }

  if (WIFEXITED(status))
    child_status = WEXITSTATUS(status);
}

int
main::run_impl ()
{
  if (this->opts->action == options::ACTION_HELP)
    action_help(std::cerr);
  else if (this->opts->action == options::ACTION_VERSION)
    action_version(std::cerr);
  else if (this->opts->action == options::ACTION_MOUNT)
    action_mount();
  else
    assert(0); // Invalid action.

  return EXIT_SUCCESS;
}
