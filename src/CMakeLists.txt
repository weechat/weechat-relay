#
# Copyright (C) 2019-2021 Sébastien Helleu <flashcode@flashtux.org>
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

  find_package(GnuTLS REQUIRED)
  find_package(Readline REQUIRED)

  list(APPEND LINK_LIBS ${READLINE_LIBRARIES})

  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # link with resolv lib on macOS
    list(APPEND LINK_LIBS "resolv")
  endif()

  include_directories(${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/lib ${READLINE_INCLUDE_DIRS})

  set(RELAY_LIB_CLI_SRC
    cli.c cli.h
    message.c message.h
    network.c network.h
    util.c util.h
  )
  add_library(relay_cli_static STATIC ${RELAY_LIB_CLI_SRC})
  add_dependencies(relay_cli_static weechatrelay_static)
  target_link_libraries(relay_cli_static ${LINK_LIBS} weechatrelay_static coverage_config)

  set(RELAY_CLI_MAIN_SRC main.c)
  set(EXECUTABLE weechat-relay-cli)
  add_executable(${EXECUTABLE} ${RELAY_CLI_MAIN_SRC})
  add_dependencies(${EXECUTABLE} relay_cli_static)
  target_link_libraries(${EXECUTABLE} ${LINK_LIBS} weechatrelay_static relay_cli_static)

  install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

endif()
