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

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(weechat-relay C)

# CMake options
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_SKIP_RPATH ON)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fsigned-char -Wall -Wextra -Werror-implicit-function-declaration")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char -Wall -Wextra -Werror-implicit-function-declaration")

# version
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/version.sh devel-full OUTPUT_VARIABLE VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/version.sh devel-major OUTPUT_VARIABLE VERSION_MAJOR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/version.sh devel-minor OUTPUT_VARIABLE VERSION_MINOR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/version.sh devel-patch OUTPUT_VARIABLE VERSION_PATCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/version.sh devel-number OUTPUT_VARIABLE VERSION_NUMBER OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT DEFINED LIBDIR)
  set(LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

if(NOT DEFINED SHAREDIR)
  set(SHAREDIR ${CMAKE_INSTALL_PREFIX}/share)
endif()

if(NOT DEFINED MANDIR)
  set(MANDIR ${SHAREDIR}/man)
endif()

if(NOT DEFINED LOCALEDIR)
  set(LOCALEDIR ${SHAREDIR}/locale)
endif()

if(DEFINED INCLUDEDIR)
  set(INCLUDEDIR ${INCLUDEDIR}/${PROJECT_NAME})
else()
  set(INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
endif()

option(BUILD_CLI     "Build the testing CLI program" ON)
option(BUILD_DOC     "Enable build of documentation" ON)
option(BUILD_MAN     "Enable build of man pages"     ON)
option(BUILD_TESTS   "Build tests"                   OFF)
option(CODE_COVERAGE "Enable code coverage"          OFF)

add_library(coverage_config INTERFACE)

if(CODE_COVERAGE)
  target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
  target_link_libraries(coverage_config INTERFACE --coverage)
endif()

configure_file(config.h.cmake config.h @ONLY)
add_definitions(-DHAVE_CONFIG_H)

configure_file(lib/weechat-relay.h.cmake lib/weechat-relay.h @ONLY)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(doc)

find_package(CppUTest)
if(BUILD_TESTS AND CPPUTEST_FOUND)
  enable_testing()
  add_subdirectory(tests)
endif()

# set the git version in "config-git.h"
add_custom_target(version_git ALL
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/git-version.sh" "${CMAKE_CURRENT_SOURCE_DIR}" "${VERSION}" "config-git.h"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(dist
  "${CMAKE_CURRENT_SOURCE_DIR}/tools/makedist.sh" "${VERSION}" "HEAD" "${CMAKE_CURRENT_BINARY_DIR}"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(includedir "\${prefix}/include")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/weechat-relay.pc.in ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay.pc DESTINATION ${LIBDIR}/pkgconfig)

# cygport file (used to build Cygwin packages)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/weechat-relay.cygport.in
  ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay-${VERSION}-1.cygport @ONLY)

# install some files (only on Cygwin)
if(CYGWIN)
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/Contributing.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/README.adoc
    DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})
endif()

# packages
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WeeChat relay protocol I/O")
set(CPACK_PACKAGE_VENDOR "Sébastien Helleu")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.adoc")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
