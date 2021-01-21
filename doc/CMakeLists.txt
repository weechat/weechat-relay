#
# Copyright (C) 2019-2021 Sébastien Helleu <flashcode@flashtux.org>
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

if(BUILD_DOC OR BUILD_MAN)
  find_package(Asciidoctor)
  if(ASCIIDOCTOR_FOUND)
    set(ASCIIDOCTOR_ARGS -a experimental -a "prewrap!" -a icons=font -a revnumber="${VERSION}" -a sectanchors -a source-highlighter=prettify)
    add_subdirectory(en)
    add_subdirectory(fr)
  else()
    message(SEND_ERROR "Asciidoctor not found")
  endif()
endif()
