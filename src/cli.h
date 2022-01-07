/*
 * Copyright (C) 2019-2022 Sébastien Helleu <flashcode@flashtux.org>
 *
 * This file is part of WeeChat Relay.
 *
 * WeeChat Relay is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * WeeChat Relay is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WeeChat Relay.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef RELAY_CLI_CLI_H
#define RELAY_CLI_CLI_H

#define RELAY_CLI_COPYRIGHT_DATE   "(C) 2019-2022"
#define RELAY_CLI_WEB_SITE         "https://weechat.org/"
#define RELAY_CLI_LICENSE_TEXT \
    "WeeChat Relay is free software; you can redistribute it and/or modify\n" \
    "it under the terms of the GNU General Public License as published by\n" \
    "the Free Software Foundation; either version 3 of the License, or\n" \
    "(at your option) any later version.\n" \
    "\n" \
    "WeeChat Relay is distributed in the hope that it will be useful,\n" \
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
    "GNU General Public License for more details.\n" \
    "\n" \
    "You should have received a copy of the GNU General Public License\n" \
    "along with WeeChat Relay.  If not, see <https://www.gnu.org/licenses/>.\n"

#define RELAY_CLI_DEFAULT_PORT     "9000"
#define RELAY_CLI_MAX_COMMANDS     16
#define RELAY_CLI_MAX_COMMANDS_STR "16"

extern int relay_cli_debug;

extern int relay_cli_main (int argc, char *argv[]);

#endif /* RELAY_CLI_CLI_H */
