/*
 * SPDX-FileCopyrightText: 2019-2025 Sébastien Helleu <flashcode@flashtux.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

#ifndef RELAY_CLI_UTIL_H
#define RELAY_CLI_UTIL_H

extern long long timeval_diff (struct timeval *tv1, struct timeval *tv2);
extern char *string_hex_dump (const char *data, size_t size,
                              int bytes_per_line,
                              const char *prefix, const char *suffix);
extern void display_hex_dump (const void *buffer, size_t size);

#endif /* RELAY_CLI_UTIL_H */
