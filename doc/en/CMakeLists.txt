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

if(BUILD_MAN)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay-cli.1
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o weechat-relay-cli.1 ${CMAKE_CURRENT_SOURCE_DIR}/weechat-relay-cli.1.en.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/weechat-relay-cli.1.en.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building weechat-relay-cli.1 (en)"
  )
  add_custom_target(doc-man-en ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay-cli.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay-cli.1 DESTINATION ${MANDIR}/man1)
endif()

if(BUILD_DOC)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/weechat_relay_api.en.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o weechat_relay_api.en.html ${CMAKE_CURRENT_SOURCE_DIR}/weechat_relay_api.en.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/weechat_relay_api.en.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building weechat_relay_api.en.html"
  )
  add_custom_target(doc-relay-api-en ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/weechat_relay_api.en.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/weechat_relay_api.en.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})
endif()
