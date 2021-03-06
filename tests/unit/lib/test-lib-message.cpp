/*
 * test-lib-message.cpp - test build of binary messages
 *
 * Copyright (C) 2019-2021 Sébastien Helleu <flashcode@flashtux.org>
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
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "lib/weechat-relay.h"
}

#define LOREM_IPSUM_1024 "Lorem ipsum dolor sit amet, consectetur adipiscing" \
    " elit. Fusce auctor ac leo ut maximus. Curabitur vestibulum facilisis n" \
    "eque, vitae sodales elit pulvinar ac. Mauris suscipit pharetra metus eu" \
    " hendrerit. Proin viverra ligula ut nibh malesuada, vel vehicula leo pu" \
    "lvinar. Nullam tellus dolor, posuere sed orci in, pretium fermentum ant" \
    "e. Donec a quam vulputate, fermentum nisi nec, convallis sapien. Vestib" \
    "ulum malesuada dui eget iaculis sagittis. Praesent egestas non ex quis " \
    "blandit. Maecenas quis leo nunc. Integer eget tincidunt sapien, id lobo" \
    "rtis libero. Aliquam posuere turpis in libero luctus pharetra. Vestibul" \
    "um dui augue, volutpat ultricies laoreet in, varius sodales ante. Ut ne" \
    "c urna non lacus bibendum scelerisque. Nullam convallis aliquet lectus " \
    "interdum volutpat. Phasellus lacus tortor, elementum hendrerit lobortis" \
    " ac, commodo id augue. Morbi imperdiet interdum consequat. Mauris purus" \
    " lectus, ultrices sed velit et, pretium rhoncus erat. Pellentesque pell" \
    "entesque efficitur nisl quis sodales. Nam hendreri."

TEST_GROUP(LibMessage)
{
};

/*
 * Tests functions:
 *   weechat_relay_msg_init
 *   weechat_relay_msg_free
 */

TEST(LibMessage, InitFree)
{
    struct t_weechat_relay_msg_buf *msg;

    msg = weechat_relay_msg_new (NULL);
    CHECK(msg);
    POINTERS_EQUAL(NULL, msg->id);
    CHECK(msg->data);
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(9, msg->data_size);
    weechat_relay_msg_free (msg);

    msg = weechat_relay_msg_new ("test");
    CHECK(msg);
    STRCMP_EQUAL("test", msg->id);
    CHECK(msg->data);
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(13, msg->data_size);
    weechat_relay_msg_free (msg);

    weechat_relay_msg_free (NULL);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_bytes
 */

TEST(LibMessage, AddBytes)
{
    struct t_weechat_relay_msg_buf *msg;
    const char *str = "abc";
    char buffer[WEECHAT_RELAY_MSG_INITIAL_ALLOC];

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_bytes (NULL, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_add_bytes (msg, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_add_bytes (msg, str, 0));

    LONGS_EQUAL(1, weechat_relay_msg_add_bytes (msg, str, strlen (str)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(msg->data + 13, str, strlen (str));

    memset (buffer, 123, sizeof (buffer));
    LONGS_EQUAL(1, weechat_relay_msg_add_bytes (msg, buffer, sizeof (buffer)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC * 2, msg->data_alloc);
    LONGS_EQUAL(16 + sizeof (buffer), msg->data_size);
    MEMCMP_EQUAL(msg->data + 16, buffer, sizeof (buffer));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_set_bytes
 */

TEST(LibMessage, SetBytes)
{
    struct t_weechat_relay_msg_buf *msg;
    const char *str1 = "abc";
    const char *str2 = "def";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (NULL, 0, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, 0, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, 0, str1, 0));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, -1, str1, 1));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, 0, str1,
                                                WEECHAT_RELAY_MSG_INITIAL_ALLOC + 1));

    LONGS_EQUAL(1, weechat_relay_msg_add_bytes (msg, str1, strlen (str1)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(msg->data + 13, str1, strlen (str1));

    LONGS_EQUAL(1, weechat_relay_msg_set_bytes (msg, 13, str2, strlen (str2)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(msg->data + 13, str2, strlen (str2));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_type
 */

TEST(LibMessage, AddType)
{
    struct t_weechat_relay_msg_buf *msg;
    const char *type = "int";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_type (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_type (msg, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_type (msg, ""));

    LONGS_EQUAL(1, weechat_relay_msg_add_type (msg, type));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(msg->data + 13, type, strlen (type));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_char
 */

TEST(LibMessage, AddChar)
{
    struct t_weechat_relay_msg_buf *msg;
    char ch = 123;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_char (NULL, 1));

    LONGS_EQUAL(1, weechat_relay_msg_add_char (msg, ch));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(14, msg->data_size);
    BYTES_EQUAL(msg->data[13], ch);

    weechat_relay_msg_free (msg);
}


/*
 * Tests functions:
 *   weechat_relay_msg_add_int
 */

TEST(LibMessage, AddInt)
{
    struct t_weechat_relay_msg_buf *msg;
    int number1 = 123456;
    uint32_t value32_number1;
    int number2 = -112233;
    uint32_t value32_number2;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_int (NULL, 1));

    LONGS_EQUAL(1, weechat_relay_msg_add_int (msg, number1));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(17, msg->data_size);
    value32_number1 = htonl ((uint32_t)number1);
    MEMCMP_EQUAL(msg->data + 13, &value32_number1, 4);

    LONGS_EQUAL(1, weechat_relay_msg_add_int (msg, number2));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(21, msg->data_size);
    value32_number2 = htonl ((uint32_t)number2);
    MEMCMP_EQUAL(msg->data + 17, &value32_number2, 4);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_long
 */

TEST(LibMessage, AddLong)
{
    struct t_weechat_relay_msg_buf *msg;
    long number1 = 123456789012;
    const char *str_number1 = "123456789012";
    long number2 = -112233445566;
    const char *str_number2 = "-112233445566";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_long (NULL, 1));

    LONGS_EQUAL(1, weechat_relay_msg_add_long (msg, number1));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(26, msg->data_size);
    BYTES_EQUAL(12, msg->data[13]);
    MEMCMP_EQUAL(msg->data + 14, str_number1, strlen (str_number1));

    LONGS_EQUAL(1, weechat_relay_msg_add_long (msg, number2));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(40, msg->data_size);
    BYTES_EQUAL(13, msg->data[26]);
    MEMCMP_EQUAL(msg->data + 27, str_number2, strlen (str_number2));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_string
 */

TEST(LibMessage, AddString)
{
    struct t_weechat_relay_msg_buf *msg;
    const char *str = "abcdef";
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_string (NULL, str));

    /* NULL string: length = -1, no content */
    LONGS_EQUAL(1, weechat_relay_msg_add_string (msg, NULL));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(17, msg->data_size);
    value32 = htonl (-1);
    MEMCMP_EQUAL(msg->data + 13, &value32, 4);

    /* empty string: length = 0, no content */
    LONGS_EQUAL(1, weechat_relay_msg_add_string (msg, ""));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(21, msg->data_size);
    value32 = htonl (0);
    MEMCMP_EQUAL(msg->data + 17, &value32, 4);

    /* string: length = length of string, content = the string (without \0) */
    LONGS_EQUAL(1, weechat_relay_msg_add_string (msg, str));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(31, msg->data_size);
    value32 = htonl (strlen (str));
    MEMCMP_EQUAL(msg->data + 21, &value32, 4);
    MEMCMP_EQUAL(msg->data + 25, str, strlen (str));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_buffer
 */

TEST(LibMessage, AddBuffer)
{
    struct t_weechat_relay_msg_buf *msg;
    const void *str = "abcdef";
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_buffer (NULL, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_add_buffer (NULL, str, 0));

    /* NULL buffer: length = -1, no content */
    LONGS_EQUAL(1, weechat_relay_msg_add_buffer (msg, NULL, 0));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(17, msg->data_size);
    value32 = htonl (-1);
    MEMCMP_EQUAL(msg->data + 13, &value32, 4);

    /* empty buffer: length = 0, no content */
    LONGS_EQUAL(1, weechat_relay_msg_add_buffer (msg, "", 0));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(21, msg->data_size);
    value32 = htonl (0);
    MEMCMP_EQUAL(msg->data + 17, &value32, 4);

    /* buffer: length = length of buffer, content = the buffer */
    LONGS_EQUAL(1, weechat_relay_msg_add_buffer (msg, str, 6));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(31, msg->data_size);
    value32 = htonl (6);
    MEMCMP_EQUAL(msg->data + 21, &value32, 4);
    MEMCMP_EQUAL(msg->data + 25, str, 6);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_pointer
 */

TEST(LibMessage, AddPointer)
{
    struct t_weechat_relay_msg_buf *msg;
    void *pointer = (void *)0x123456789abc;
    const char *str_pointer = "123456789abc";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_pointer (NULL, (void *)1));

    /* NULL pointer */
    LONGS_EQUAL(1, weechat_relay_msg_add_pointer (msg, NULL));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(15, msg->data_size);
    BYTES_EQUAL(1, msg->data[13]);
    BYTES_EQUAL('0', msg->data[14]);

    LONGS_EQUAL(1, weechat_relay_msg_add_pointer (msg, pointer));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(28, msg->data_size);
    BYTES_EQUAL(12, msg->data[15]);
    MEMCMP_EQUAL(msg->data + 16, str_pointer, strlen (str_pointer));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_time
 */

TEST(LibMessage, AddTime)
{
    struct t_weechat_relay_msg_buf *msg;
    time_t time = 123456789;
    const char *str_time = "123456789";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_time (NULL, 123));

    LONGS_EQUAL(1, weechat_relay_msg_add_time (msg, time));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(23, msg->data_size);
    BYTES_EQUAL(9, msg->data[13]);
    MEMCMP_EQUAL(msg->data + 14, str_time, strlen (str_time));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_compress
 */

TEST(LibMessage, Compress)
{
    struct t_weechat_relay_msg_buf *msg;
    void *buffer;
    size_t size;

    POINTERS_EQUAL(NULL, weechat_relay_msg_compress (NULL, 0, NULL));
    POINTERS_EQUAL(NULL, weechat_relay_msg_compress (NULL, 0, &size));

    msg = weechat_relay_msg_new ("test");

    weechat_relay_msg_add_int (msg, 123456);
    weechat_relay_msg_add_long (msg, 1234567890L);
    weechat_relay_msg_add_string (msg, LOREM_IPSUM_1024);

    /* initial message size */
    LONGS_EQUAL(1056, msg->data_size);

    /* compression level 0 (no compression) */
    buffer = weechat_relay_msg_compress (msg, 0, &size);
    CHECK(size >= 1056);
    free (buffer);

    /* compression level 1 (highest speed) */
    buffer = weechat_relay_msg_compress (msg, 1, &size);
    CHECK(size < 1056);
    free (buffer);

    /* compression level 9 (highest compression) */
    buffer = weechat_relay_msg_compress (msg, 9, &size);
    CHECK(size < 1056);
    free (buffer);

    weechat_relay_msg_free (msg);
}
