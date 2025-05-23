<!--
SPDX-FileCopyrightText: 2019-2025 Sébastien Helleu <flashcode@flashtux.org>

SPDX-License-Identifier: GPL-3.0-or-later
-->

# WeeChat Relay

[![CI](https://github.com/weechat/weechat-relay/workflows/CI/badge.svg)](https://github.com/weechat/weechat-relay/actions)
[![Code coverage](https://codecov.io/gh/weechat/weechat-relay/branch/master/graph/badge.svg)](https://codecov.io/gh/weechat/weechat-relay)
[![REUSE status](https://api.reuse.software/badge/github.com/weechat/weechat-relay)](https://api.reuse.software/info/github.com/weechat/weechat-relay)

WeeChat Relay is a C library to communicate between WeeChat and a client using the "weechat" relay protocol.

> [!IMPORTANT]
> This library is under development, there is no stable version yet.\
**DO NOT USE IN PRODUCTION!**

Homepage: [https://weechat.org/](https://weechat.org/)

## Features

- Communication between WeeChat and a client:
  - from client to WeeChat: build and send commands, decode binary messages received from WeeChat
  - from WeeChat to client: build and send binary messages.
- Command-line interface to the WeeChat Relay library (`weechat-relay-cli`), which can be used for tests, debug or in shell scripts.

## Install

### Dependencies

The following packages are **required**:

- CMake
- GnuTLS
- GNU Readline (for `weechat-relay-cli`)
- libzstd
- zlib

### Compile

Installation in system directories (requires _root_ privileges):

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

Installation in custom directory (for example your home):

```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/directory ..
make
make install
```

### Run tests

The following packages are **required** to compile tests:

- libcpputest-dev
- C++ compiler

Tests must be enabled when compiling WeeChat Relay:

```bash
cmake -DBUILD_TESTS=ON ..
```

They can be launched after compilation from the build directory:

```bash
ctest -V
```

## Command-line interface

The binary `weechat-relay-cli` can be used to debug communication between a client and WeeChat relay.

For help:

```bash
weechat-relay-cli --help
```

Example:

```text
$ weechat-relay-cli -dd -s -p 12345 localhost
Connected to 127.0.0.1 (SSL)
weechat-relay> init password=my_secret_password
<-- init password=my_secret_password (33 bytes)
      69 6E 69 74 20 70 61 73 73 77 6F 72 64 3D 6D 79   i n i t   p a s s w o r d = m y
      5F 73 65 63 72 65 74 5F 70 61 73 73 77 6F 72 64   _ s e c r e t _ p a s s w o r d
      0A                                                .
weechat-relay> info version
<-- info version (13 bytes)
      69 6E 66 6F 20 76 65 72 73 69 6F 6E 0A            i n f o   v e r s i o n .
--> 34 bytes received
      00 00 00 22 00 FF FF FF FF 69 6E 66 00 00 00 07   . . . " . . . . . i n f . . . .
      76 65 72 73 69 6F 6E 00 00 00 07 32 2E 39 2D 64   v e r s i o n . . . . 2 . 9 - d
      65 76                                             e v
    message (length: 34 bytes, data: 29 bytes, no compression):
      id: NULL
      info: "version" -> "2.9-dev"
weechat-relay> quit
<-- quit (5 bytes)
      71 75 69 74 0A                                    q u i t .
```

## Copyright

<!-- REUSE-IgnoreStart -->
Copyright © 2019-2025 [Sébastien Helleu](https://github.com/flashcode)

This file is part of WeeChat Relay.

WeeChat Relay is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

WeeChat Relay is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WeeChat Relay.  If not, see <https://www.gnu.org/licenses/>.
<!-- REUSE-IgnoreEnd -->
