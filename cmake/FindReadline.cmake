#
# Copyright (C) 2019-2020 Sébastien Helleu <flashcode@flashtux.org>
#
# This file is part of WeeChat Relay.
#
# WeeChat Relay is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# WeeChat Relay is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with WeeChat Relay.  If not, see <https://www.gnu.org/licenses/>.
#

# - Find CppUTest
# This module finds if CppUTest is installed and determines where the include
# files and libraries are. It also determines what the name of the library is.
# This code sets the following variables:
#
# CPPUTEST_FOUND        = CppUTest is installed
# CPPUTEST_INCLUDE_DIRS = CppUTest include directory
# CPPUTEST_LIBRARIES    = Link options to compile with CppUTest

include(FindPkgConfig)

pkg_check_modules(READLINE readline)

if(READLINE_FOUND)
  set(READLINE_LIBRARY ${READLINE_LDFLAGS})
else()
  find_path(READLINE_ROOT_DIR
    NAMES include/readline/readline.h
  )
  find_path(READLINE_INCLUDE_DIRS
    NAMES readline.h
    HINTS ${READLINE_ROOT_DIR}/include/readline
  )
  find_library(READLINE_LIBRARY
    NAMES readline
    HINTS ${READLINE_ROOT_DIR}/lib
  )
  if(READLINE_INCLUDE_DIRS AND READLINE_LIBRARY)
    set(READLINE_FOUND TRUE)
  endif()
endif()

mark_as_advanced(
  READLINE_INCLUDE_DIRS
  READLINE_LIBRARY
)
