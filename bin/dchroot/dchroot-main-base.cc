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

#include "dchroot-main.h"
#include "dchroot-session.h"

#include <cstdlib>
#include <iostream>
#include <locale>

#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include <boost/format.hpp>

#include <syslog.h>

using std::endl;
using boost::format;
using sbuild::_;
using schroot::options_base;
using namespace dchroot;

main_base::main_base (std::string const& program_name,
                      std::string const& program_usage,
                      schroot::options_base::ptr& options):
  schroot::main_base(program_name, program_usage, options, true)
{
}

main_base::~main_base ()
{
}

void
main_base::action_list ()
{
  this->config->print_chroot_list_simple(std::cout);
}
