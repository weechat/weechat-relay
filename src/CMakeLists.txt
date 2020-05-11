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

if(BUILD_CLI)

  set(WEECHAT_RELAY_CLI_SRC cli.c network.c)

  set(EXECUTABLE weechat-relay-cli)

  include(FindPkgConfig)

  find_package(GnuTLS REQUIRED)
  find_package(ZLIB REQUIRED)

  pkg_check_modules(READLINE readline REQUIRED)
  list(APPEND LINK_LIBS ${READLINE_LDFLAGS})

  include_directories(${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/lib ${READLINE_INCLUDE_DIRS})
  add_executable(${EXECUTABLE} ${WEECHAT_RELAY_CLI_SRC})
  add_dependencies(${EXECUTABLE} weechatrelay_static)
  target_link_libraries(${EXECUTABLE} ${LINK_LIBS})

  install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

endif()
