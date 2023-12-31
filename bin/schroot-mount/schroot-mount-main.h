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

#ifndef SCHROOT_MOUNT_MAIN_H
#define SCHROOT_MOUNT_MAIN_H

#include <schroot-base/schroot-base-main.h>

#include <schroot-mount/schroot-mount-options.h>

#include <sbuild/sbuild-custom-error.h>

/**
 * schroot-mount program components
 */
namespace schroot_mount
{

  /**
   * Frontend for schroot-mount.  This class is used to "run" schroot-mount.
   */
  class main : public schroot_base::main
  {
  public:
    /// Error codes.
    enum error_code
      {
        CHILD_FORK, ///< Failed to fork child.
        CHILD_WAIT, ///< Wait for child failed.
        CHROOT,     ///< Failed to change root.
        DUP,        ///< Failed to duplicate file descriptor.
        EXEC,       ///< Failed to execute.
        PIPE,       ///< Failed to create pipe.
        POLL,       ///< Failed to poll file descriptor.
        READ,       ///< Failed to read file descriptor.
        REALPATH    ///< Failed to resolve path.
      };

    /// Exception type.
    typedef sbuild::custom_error<error_code> error;

    /**
     * The constructor.
     *
     * @param options the command-line options to use.
     */
    main (options::ptr& options);

    /// The destructor.
    virtual ~main ();

  private:
    /**
     * Mount filesystems.
     */
    virtual void
    action_mount ();

    /**
     * Make a bind mount use private mount propagation (Linux-specific).
     *
     * @param options the mount options
     * @param mountpoint the mountpiont to make private
     */
    void
    make_mount_private (const std::string& options,
                        const std::string& mountpoint);

    /**
     * Run the command specified by file (an absolute pathname), using
     * command and env as the argv and environment, respectively.
     *
     * @param file the program to execute.
     * @param command the arguments to pass to the executable.
     * @param env the environment.
     * @returns the return value of the execve system call on failure.
     */
    int
    run_child(std::string const& file,
              sbuild::string_list const& command,
              sbuild::environment const& env);

    /**
     * Ensure that the mountpoint is a valid absolute path. The calling process
     * must be chrooted before calling this function to avoid resolving
     * absolute or relative symlinks pointing outside the chroot.  An exception
     * will be thrown if it is not possible to resolve the path.
     *
     * @param mountpoint the mountpoint to check,
     * @returns the validated path.
     */

    std::string
    resolve_path_chrooted (std::string const& mountpoint);

    /**
     * Ensure that the chroot base path and the mountpoint is a valid absolute
     * path inside the chroot.  This function calls chroot on the resolved
     * chroot base path before attempting to resolve the mountpoint path inside
     * of the chroot to avoid absolute or relative symlinks pointing outside
     * the chroot causing filesystems to be mounted on the host.  An exception
     * will be thrown if it is not possible to resolve the chroot base path or
     * the mountpoint path.
     *
     * @param mountpoint the mountpoint to check,
     * @returns the validated path.
     */
    std::string
    resolve_path (std::string const& mountpoint);

    /**
     * Wait for a child process to complete, and check its exit status.
     *
     * An error will be thrown on failure.
     *
     * @param pid the pid to wait for.
     * @param child_status the place to store the child exit status.
     */
    void
    wait_for_child (pid_t pid,
                    int&  child_status);

  protected:
    /**
     * Run the program.
     *
     * @returns 0 on success, 1 on failure or the exit status of the
     * chroot command.
     */
    virtual int
    run_impl ();

  private:
    /// The program options.
    options::ptr opts;
  };

}

#endif /* SCHROOT_MOUNT_MAIN_H */

/*
 * Local Variables:
 * mode:C++
 * End:
 */
