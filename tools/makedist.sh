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
# Build compressed tarballs for WeeChat Relay using git-archive.
#
# Syntax:  makedist.sh [<version> [<tree-ish> [<path>]]]
#
# Optional arguments:
#
#   version   WeeChat Relay version, for example 1.0.0 or 1.1.0-dev
#             defaults to current devel version (output of "version.sh devel-full")
#   tree-ish  git tree-ish, example: v0.1
#             defaults to "HEAD"
#   path      where to put packages
#             defaults to current directory
#

# check git repository
root_dir=$(git rev-parse --show-toplevel)
if [ -z "${root_dir}" ] || [ ! -d "${root_dir}/.git" ]; then
    echo "This script must be run from WeeChat Relay git repository."
    exit 1
fi

# default values
version="$("${root_dir}/version.sh" devel-full)"
treeish="HEAD"
outpath="$(pwd)"

if [ $# -ge 1 ]; then
    version=$1
fi
if [ $# -ge 2 ]; then
    treeish=$2
fi
if [ $# -ge 3 ]; then
    outpath=$(cd "$3" || exit 1; pwd)
fi

cd "${root_dir}" || exit 1

prefix="weechat-relay-${version}/"
file="${outpath}/weechat-relay-${version}.tar"

echo "Building package ${file}.gz"
git archive --prefix="${prefix}" "${treeish}" | gzip -c >"${file}.gz"

echo "Building package ${file}.xz"
git archive --prefix="${prefix}" "${treeish}" | xz -c >"${file}.xz"
