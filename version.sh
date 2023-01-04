#!/bin/sh
#
# Copyright (C) 2019-2023 Sébastien Helleu <flashcode@flashtux.org>
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

#
# Returns current stable or devel version of WeeChat Relay.
#
# Syntax:
#   version.sh stable|devel|devel-full|devel-major|devel-minor|devel-patch
#
#     stable         the stable version (e.g. 1.2.3)
#     stable-major   the stable version, major number (e.g. 1)
#     stable-minor   the stable version, minor number (e.g. 2)
#     stable-patch   the stable version, patch number (e.g. 3)
#     stable-number  the stable version, as hex number (e.g. 0x010203)
#
#     devel          the devel version (e.g. 1.3.0)
#     devel-full     the devel version, with suffix (e.g. 1.3.0-dev)
#     devel-major    the devel version, major number (e.g. 1)
#     devel-minor    the devel version, minor number (e.g. 3)
#     devel-patch    the devel version, patch number (e.g. 0)
#     devel-number   the devel version, as hex number (e.g. 0x010300)
#

WEECHAT_RELAY_STABLE_MAJOR="0"
WEECHAT_RELAY_STABLE_MINOR="0"
WEECHAT_RELAY_STABLE_PATCH="0"

WEECHAT_RELAY_DEVEL_MAJOR="1"
WEECHAT_RELAY_DEVEL_MINOR="0"
WEECHAT_RELAY_DEVEL_PATCH="0"
WEECHAT_RELAY_DEVEL_SUFFIX="-dev"

if [ $# -lt 1 ]; then
    echo >&2 "Syntax: $0 stable|stable-major|stable-minor|stable-patch|stable-number|devel|devel-full|devel-major|devel-minor|devel-patch|devel-number"
    exit 1
fi

case $1 in
    stable ) echo "${WEECHAT_RELAY_STABLE_MAJOR}.${WEECHAT_RELAY_STABLE_MINOR}.${WEECHAT_RELAY_STABLE_PATCH}" ;;
    stable-major ) echo "${WEECHAT_RELAY_STABLE_MAJOR}" ;;
    stable-minor ) echo "${WEECHAT_RELAY_STABLE_MINOR}" ;;
    stable-patch ) echo "${WEECHAT_RELAY_STABLE_PATCH}" ;;
    stable-number ) echo "0x$(printf '%02d' ${WEECHAT_RELAY_STABLE_MAJOR})$(printf '%02d' ${WEECHAT_RELAY_STABLE_MINOR})$(printf '%02d' ${WEECHAT_RELAY_STABLE_PATCH})" ;;
    devel ) echo "${WEECHAT_RELAY_DEVEL_MAJOR}.${WEECHAT_RELAY_DEVEL_MINOR}.${WEECHAT_RELAY_DEVEL_PATCH}" ;;
    devel-full ) echo "${WEECHAT_RELAY_DEVEL_MAJOR}.${WEECHAT_RELAY_DEVEL_MINOR}.${WEECHAT_RELAY_DEVEL_PATCH}${WEECHAT_RELAY_DEVEL_SUFFIX}" ;;
    devel-major ) echo "${WEECHAT_RELAY_DEVEL_MAJOR}" ;;
    devel-minor ) echo "${WEECHAT_RELAY_DEVEL_MINOR}" ;;
    devel-patch ) echo "${WEECHAT_RELAY_DEVEL_PATCH}" ;;
    devel-number ) echo "0x$(printf '%02d' ${WEECHAT_RELAY_DEVEL_MAJOR})$(printf '%02d' ${WEECHAT_RELAY_DEVEL_MINOR})$(printf '%02d' ${WEECHAT_RELAY_DEVEL_PATCH})" ;;
    * ) echo >&2 "ERROR: unknown version."
        exit 1 ;;
esac
