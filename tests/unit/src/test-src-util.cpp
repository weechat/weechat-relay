/*
 * test-src-network.cpp - test utility functions in command-line interface
 *
 * Copyright (C) 2019-2023 Sébastien Helleu <flashcode@flashtux.org>
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
 *    timeval_diff
 */

TEST(SrcUtil, TimevalDiff)
{
    struct timeval tv_zero = { 0, 0 };
    struct timeval tv1 = { 123456, 12000 };
    struct timeval tv2 = { 123456, 15000 };
    struct timeval tv3 = { 123457, 15000 };
    struct timeval tv4 = { 1640279046, 0 };  /* 2021-12-23 17:04:06 GMT */

    LONGS_EQUAL(0, timeval_diff (NULL, NULL));
    LONGS_EQUAL(0, timeval_diff (NULL, &tv1));
    LONGS_EQUAL(0, timeval_diff (&tv1, NULL));
    LONGS_EQUAL(3000, timeval_diff (&tv1, &tv2));
    LONGS_EQUAL(-3000, timeval_diff (&tv2, &tv1));
    LONGS_EQUAL(1003000, timeval_diff (&tv1, &tv3));
    LONGS_EQUAL(-1003000, timeval_diff (&tv3, &tv1));
    CHECK(1640279046 * 1000000LL == timeval_diff (&tv_zero, &tv4));
}

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

/*
 * Tests functions:
 *   display_hex_dump
 */

TEST(SrcUtil, DisplayHexDump)
{
    const char *string = "abcdefghijklmnopqrstuvwzyz";

    display_hex_dump (NULL, 0);
    display_hex_dump (string, 0);
    display_hex_dump (string, strlen (string));
}
