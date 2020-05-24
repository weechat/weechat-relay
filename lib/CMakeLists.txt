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

set(WEECHAT_RELAY_SRC command.c message.c session.c)

find_package(ZLIB REQUIRED)

list(APPEND LINK_LIBS ${ZLIB_LIBRARY})

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(weechatrelay_static STATIC ${WEECHAT_RELAY_SRC})
add_library(weechatrelay_shared SHARED ${WEECHAT_RELAY_SRC})

target_link_libraries(weechatrelay_static ${LINK_LIBS} coverage_config)
target_link_libraries(weechatrelay_shared ${LINK_LIBS})

set_target_properties(weechatrelay_static PROPERTIES OUTPUT_NAME weechatrelay)
set_target_properties(weechatrelay_shared PROPERTIES OUTPUT_NAME weechatrelay)
set_target_properties(weechatrelay_shared PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/weechat-relay.h)

install(TARGETS weechatrelay_static DESTINATION ${LIBDIR})
install(TARGETS weechatrelay_shared DESTINATION ${LIBDIR} PUBLIC_HEADER DESTINATION ${INCLUDEDIR})
