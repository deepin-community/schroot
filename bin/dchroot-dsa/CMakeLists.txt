# Copyright © 2004-2013  Roger Leigh <rleigh@codelibre.net>
#
# schroot is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# schroot is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.
#
#####################################################################

set(dchroot_dsa_sources
    dchroot-dsa-session.h
    dchroot-dsa-session.cc
    dchroot-dsa-options.h
    dchroot-dsa-options.cc
    dchroot-dsa-main.h
    dchroot-dsa-main.cc
    dchroot-dsa.cc)

include_directories(${PROJECT_BINARY_DIR}/bin ${PROJECT_SOURCE_DIR}/bin)

if(BUILD_DCHROOT_DSA)
  add_executable(dchroot-dsa ${dchroot_dsa_sources})
  target_link_libraries(dchroot-dsa sbuild schroot-base schroot-common dchroot-common)

  install(TARGETS dchroot-dsa RUNTIME
           DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_EXECUTE
                      WORLD_READ WORLD_EXECUTE
                      SETUID)
endif(BUILD_DCHROOT_DSA)
