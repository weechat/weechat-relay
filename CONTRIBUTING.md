<!--
SPDX-FileCopyrightText: 2019-2025 Sébastien Helleu <flashcode@flashtux.org>

SPDX-License-Identifier: GPL-3.0-or-later
-->

# Contributing to WeeChat Relay

## Reporting bugs

First, some basic things:

- Use only English to communicate with developers.
- Search in issues if the same problem or feature request has already been
  reported (a duplicate is waste of time for you and the developers!).
- If you can, please check if the problem has been fixed in development version
  (if you are using a stable release or old version).
- Report only one bug or feature request per issue.

### Security reports

Please **DO NOT** file a GitHub issue for security related problems, but send an
email to [security@weechat.org](mailto:security@weechat.org) instead.

### Required info

When reporting [issues](https://github.com/weechat/weechat-relay/issues) on GitHub,
please include:

- The **library version**: output of `weechat-relay-cli -v`
- The **version of the remote WeeChat**: the output of `/v` in WeeChat,
  for example: _WeeChat 2.5-dev (git: v2.4-67-g413221bc9)_. +
- Your **operating system**: its name and version (examples: Linux Debian Bookworm,
  FreeBSD 13.0, Windows/Cygwin 64-bit, Windows/Ubuntu 64-bit...).
- The **steps to reproduce**: if possible, please include a reproducible example:
  explain the steps which led you to the problem.
- The **gdb's backtrace** (only for a crash): if you can reproduce the crash
  (or if you have a core file), please include the backtrace from gdb.
- The **actual result**.
- The **expected result**: the correct result you are expecting.

## Feature requests

WeeChat Relay is under active development, so your idea may already have been
implemented, or scheduled for a future version (you can check in
[milestones](https://github.com/weechat/weechat-relay/milestones) on GitHub).

Pull requests on GitHub are welcome for minor new features.

For major new features, it's better to discuss about it in IRC
(server: `irc.libera.chat`, channel `#weechat`).

Before submitting any pull request, be sure you have read the
[coding rules](https://weechat.org/doc/weechat/dev/#coding_rules)
in WeeChat Developer's guide, which contains info about styles used, naming
convention and other useful info.
