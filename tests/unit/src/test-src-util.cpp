/*
 * test-src-network.cpp - test utility functions in command-line interface
 *
 * Copyright (C) 2019-2020 Sébastien Helleu <flashcode@flashtux.org>
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

#include "CppUTest/TestHarness.h"

extern "C"
{
#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#endif
#include <string.h>
#include "src/util.h"
}

#define WEE_HEX_DUMP(__result, __data, __size, __bytes_per_line,        \
                     __prefix, __suffix)                                \
    str = string_hex_dump (__data, __size, __bytes_per_line,            \
                           __prefix, __suffix);                         \
    STRCMP_EQUAL(__result, str);                                        \
    free (str);

TEST_GROUP(SrcUtil)
{
};

/*
 * Tests functions:
 *    string_hex_dump
 */

TEST(SrcUtil, StringHexDump)
{
    const char *noel_utf8 = "no\xc3\xabl";  /* noël */
    const char *noel_iso = "no\xebl";
    char *str;

    POINTERS_EQUAL(NULL, string_hex_dump (NULL, 0, 0, NULL, NULL));
    POINTERS_EQUAL(NULL, string_hex_dump ("abc", 0, 0, NULL, NULL));
    POINTERS_EQUAL(NULL, string_hex_dump ("abc", 3, 0, NULL, NULL));
    POINTERS_EQUAL(NULL, string_hex_dump ("abc", 0, 5, NULL, NULL));

    WEE_HEX_DUMP("61 62 63   a b c ", "abc", 3, 3, NULL, NULL);
    WEE_HEX_DUMP("61 62 63   a b c ", "abc", 3, 3, "", "");
    WEE_HEX_DUMP("(( 61 62 63   a b c ", "abc", 3, 3, "(( ", NULL);
    WEE_HEX_DUMP("61 62 63   a b c  ))", "abc", 3, 3, NULL, " ))");
    WEE_HEX_DUMP("(( 61 62 63   a b c  ))", "abc", 3, 3, "(( ", " ))");
    WEE_HEX_DUMP("61 62 63         a b c     ", "abc", 3, 5, NULL, NULL);
    WEE_HEX_DUMP("61 62 63                        a b c               ",
                 "abc", 3, 10, NULL, NULL);
    WEE_HEX_DUMP("61 62   a b \n"
                 "63      c   ",
                 "abc", 3, 2, NULL, NULL);
    WEE_HEX_DUMP("6E 6F C3 AB 6C   n o . . l ",
                 noel_utf8, strlen (noel_utf8), 5, NULL, NULL);
    WEE_HEX_DUMP("6E 6F   n o \n"
                 "C3 AB   . . \n"
                 "6C      l   ",
                 noel_utf8, strlen (noel_utf8), 2, NULL, NULL);
    WEE_HEX_DUMP("( 6E 6F   n o \n"
                 "( C3 AB   . . \n"
                 "( 6C      l   ",
                 noel_utf8, strlen (noel_utf8), 2, "( ", NULL);
    WEE_HEX_DUMP("( 6E 6F   n o  )\n"
                 "( C3 AB   . .  )\n"
                 "( 6C      l    )",
                 noel_utf8, strlen (noel_utf8), 2, "( ", " )");
    WEE_HEX_DUMP("6E 6F EB 6C      n o . l   ",
                 noel_iso, strlen (noel_iso), 5, NULL, NULL);
    WEE_HEX_DUMP("6E 6F   n o \n"
                 "EB 6C   . l ",
                 noel_iso, strlen (noel_iso), 2, NULL, NULL);
}
