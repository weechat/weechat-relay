#
# Copyright (C) 2019-2022 Sébastien Helleu <flashcode@flashtux.org>
#
# This file is part of WeeChat Relay, the extensible chat client.
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

enable_language(CXX)

include_directories(${CPPUTEST_INCLUDE_DIRS} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

# unit tests (library)
set(LIB_WEECHAT_RELAY_UNIT_TESTS_LIB_SRC
  unit/lib/test-lib-command.cpp
  unit/lib/test-lib-message.cpp
  unit/lib/test-lib-object.cpp
  unit/lib/test-lib-parse.cpp
  unit/lib/test-lib-session.cpp
  unit/src/test-src-cli.cpp
  unit/src/test-src-message.cpp
  unit/src/test-src-network.cpp
  unit/src/test-src-util.cpp
)
add_library(weechat_relay_unit_tests_lib STATIC ${LIB_WEECHAT_RELAY_UNIT_TESTS_LIB_SRC})

# binary to run tests
set(WEECHAT_RELAY_TESTS_SRC tests.cpp tests.h)
add_executable(tests ${WEECHAT_RELAY_TESTS_SRC})
target_link_libraries(tests
  weechat_relay_unit_tests_lib
  weechatrelay_static
  relay_cli_static
  ${CPPUTEST_LDFLAGS})
add_dependencies(tests
  weechat_relay_unit_tests_lib
  weechatrelay_static
  relay_cli_static)

# test for cmake (ctest)
add_test(NAME unit
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND tests -v)
