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
#   version.sh <name>
#
#   name is one of:
#
#     stable         the stable version (e.g. "1.2.3")
#     stable-major   the major version of stable ("1" for "1.2.3")
#     stable-minor   the minor version of stable ("2" for "1.2.3")
#     stable-patch   the patch version of stable ("3" for "1.2.3")
#     stable-number  the stable version as hex number ("0x010203" for "1.2.3")
#     devel          the devel with only digits/dots (e.g. "1.3.0")
#     devel-full     the full devel (e.g. "1.3.0-dev")
#     devel-major    the major version of devel ("1" for "1.3.0-dev")
#     devel-minor    the minor version of devel ("3" for "1.3.0-dev")
#     devel-patch    the patch version of devel ("0-dev" for "1.3.0-dev")
#     devel-number   the devel version as hex number ("0x010300" for "1.3.0-dev")
#

WEECHAT_RELAY_STABLE="0.0.0"
WEECHAT_RELAY_DEVEL="1.0.0-dev"

STABLE_MAJOR=$(echo "${WEECHAT_RELAY_STABLE}" | cut -d"." -f1)
STABLE_MINOR=$(echo "${WEECHAT_RELAY_STABLE}" | cut -d"." -f2)
STABLE_PATCH=$(echo "${WEECHAT_RELAY_STABLE}" | cut -d"." -f3-)
STABLE_PATCH_DIGITS=$(echo "${WEECHAT_RELAY_STABLE}" | cut -d"." -f3- | cut -d"-" -f1)

DEVEL_MAJOR=$(echo "${WEECHAT_RELAY_DEVEL}" | cut -d"." -f1)
DEVEL_MINOR=$(echo "${WEECHAT_RELAY_DEVEL}" | cut -d"." -f2)
DEVEL_PATCH=$(echo "${WEECHAT_RELAY_DEVEL}" | cut -d"." -f3-)
DEVEL_PATCH_DIGITS=$(echo "${WEECHAT_RELAY_DEVEL}" | cut -d"." -f3- | cut -d"-" -f1)

if [ $# -lt 1 ]; then
    echo >&2 "Syntax: $0 <name>"
    echo >&2 "name: stable, stable-major, stable-minor, stable-patch, stable-number,"
    echo >&2 "      devel, devel-full, devel-major, devel-minor, devel-patch, devel-number"
    exit 1
fi

case $1 in
    # stable
    stable ) echo "${WEECHAT_RELAY_STABLE}" ;;
    stable-major ) echo "${STABLE_MAJOR}" ;;
    stable-minor ) echo "${STABLE_MINOR}" ;;
    stable-patch ) echo "${STABLE_PATCH}" ;;
    stable-number ) printf "0x%02d%02d%02d\n" "${STABLE_MAJOR}" "${STABLE_MINOR}" "${STABLE_PATCH_DIGITS}" ;;
    # devel
    devel ) echo "${WEECHAT_RELAY_DEVEL}" | cut -d"-" -f1 ;;
    devel-full ) echo "${WEECHAT_RELAY_DEVEL}" ;;
    devel-major ) echo "${DEVEL_MAJOR}" ;;
    devel-minor ) echo "${DEVEL_MINOR}" ;;
    devel-patch ) echo "${DEVEL_PATCH}" ;;
    devel-number ) printf "0x%02d%02d%02d\n" "${DEVEL_MAJOR}" "${DEVEL_MINOR}" "${DEVEL_PATCH_DIGITS}" ;;
    # error
    * ) echo >&2 "ERROR: unknown version."
        exit 1 ;;
esac
