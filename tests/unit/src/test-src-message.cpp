/*
 * test-src-message.cpp - test message functions in command-line interface
 *
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

#include "CppUTest/TestHarness.h"

extern "C"
{
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <gnutls/gnutls.h>
#include "tests/tests.h"
#include "lib/weechat-relay.h"
#include "src/cli.h"
#include "src/message.h"

extern void relay_message_printf (const char *format, ...);
extern void relay_message_display_char (char value);
extern void relay_message_display_integer (int value);
extern void relay_message_display_long (long value);
extern void relay_message_display_string (const char *value);
extern void relay_message_display_buffer (struct t_weechat_relay_obj_buffer *value);
extern void relay_message_display_pointer (const void *value);
extern void relay_message_display_time (time_t value);
extern void relay_message_display_hashtable (struct t_weechat_relay_obj_hashtable *value);
extern void relay_message_display_hdata (struct t_weechat_relay_obj_hdata *value);
extern void relay_message_display_info (struct t_weechat_relay_obj_info *value);
extern void relay_message_display_infolist (struct t_weechat_relay_obj_infolist *value);
extern void relay_message_display_array (struct t_weechat_relay_obj_array *value);
extern void relay_message_display_object (struct t_weechat_relay_obj *obj);
}

TEST_GROUP(SrcMessage)
{
    void setup()
    {
        relay_cli_debug = 999999;
    }

    void teardown()
    {
        relay_cli_debug = 0;
    }
};

/*
 * Tests functions:
 *   relay_message_printf
 */

TEST(SrcMessage, Printf)
{
    relay_message_printf ("test %d", 123);
}

/*
 * Tests functions:
 *   relay_message_display_char
 */

TEST(SrcMessage, DisplayChar)
{
    relay_message_display_char ('\0');
    relay_message_display_char ('\1');
    relay_message_display_char ('A');
}

/*
 * Tests functions:
 *   relay_message_display_integer
 */

TEST(SrcMessage, DisplayInteger)
{
    relay_message_display_integer (0);
    relay_message_display_integer (123456);
    relay_message_display_integer (-123456);
}

/*
 * Tests functions:
 *   relay_message_display_long
 */

TEST(SrcMessage, DisplayLong)
{
    relay_message_display_long (0);
    relay_message_display_long (1234567890);
    relay_message_display_long (-1234567890);
}

/*
 * Tests functions:
 *   relay_message_display_string
 */

TEST(SrcMessage, DisplayString)
{
    relay_message_display_string (NULL);
    relay_message_display_string ("");
    relay_message_display_string ("abcdef");
}

/*
 * Tests functions:
 *   relay_message_display_buffer
 */

TEST(SrcMessage, DisplayBuffer)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_buffer_null[] = { MESSAGE_BUFFER_NULL };
    unsigned char msg_buffer_abc[] = { MESSAGE_BUFFER };

    parsed_msg = weechat_relay_parse_message (msg_buffer_null,
                                        sizeof (msg_buffer_null));
    CHECK(parsed_msg);
    relay_message_display_buffer (&parsed_msg->objects[0]->value_buffer);
    weechat_relay_parse_msg_free (parsed_msg);

    parsed_msg = weechat_relay_parse_message (msg_buffer_abc,
                                        sizeof (msg_buffer_abc));
    CHECK(parsed_msg);
    relay_message_display_buffer (&parsed_msg->objects[0]->value_buffer);
    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   relay_message_display_pointer
 */

TEST(SrcMessage, DisplayPointer)
{
    relay_message_display_pointer (NULL);
    relay_message_display_pointer ((const void *)0x123abc);
}

/*
 * Tests functions:
 *   relay_message_display_time
 */

TEST(SrcMessage, DisplayTime)
{
    relay_message_display_time (0);
    relay_message_display_time (1321993456);
}

/*
 * Tests functions:
 *   relay_message_display_hashtable
 */

TEST(SrcMessage, DisplayHashtable)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_hashtable[] = { MESSAGE_HASHTABLE };

    parsed_msg = weechat_relay_parse_message (msg_hashtable, sizeof (msg_hashtable));
    CHECK(parsed_msg);
    relay_message_display_hashtable (&parsed_msg->objects[0]->value_hashtable);
    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   relay_message_display_hdata
 */

TEST(SrcMessage, DisplayHdata)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_hdata[] = { MESSAGE_HDATA };

    parsed_msg = weechat_relay_parse_message (msg_hdata, sizeof (msg_hdata));
    CHECK(parsed_msg);
    relay_message_display_hdata (&parsed_msg->objects[0]->value_hdata);
    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   relay_message_display_info
 */

TEST(SrcMessage, DisplayInfo)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_info[] = { MESSAGE_INFO };

    parsed_msg = weechat_relay_parse_message (msg_info, sizeof (msg_info));
    CHECK(parsed_msg);
    relay_message_display_info (&parsed_msg->objects[0]->value_info);
    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   relay_message_display_infolist
 */

TEST(SrcMessage, DisplayInfolist)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_infolist[] = { MESSAGE_INFOLIST };

    parsed_msg = weechat_relay_parse_message (msg_infolist, sizeof (msg_infolist));
    CHECK(parsed_msg);
    relay_message_display_infolist (&parsed_msg->objects[0]->value_infolist);
    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   relay_message_display_array
 */

TEST(SrcMessage, DisplayArray)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_array[] = { MESSAGE_ARRAY };

    parsed_msg = weechat_relay_parse_message (msg_array, sizeof (msg_array));
    CHECK(parsed_msg);
    relay_message_display_array (&parsed_msg->objects[0]->value_array);
    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   relay_message_display_array
 */

TEST(SrcMessage, DisplayObject)
{
    unsigned char msg_invalid_size[] = { MESSAGE_INVALID_SIZE };
    unsigned char msg_invalid_id[] = { MESSAGE_INVALID_ID };
    unsigned char msg_invalid_compressed_data[] = { MESSAGE_INVALID_COMPRESSED_DATA };
    unsigned char msg_char[] = { MESSAGE_CHAR };
    unsigned char msg_integer[] = { MESSAGE_INTEGER };
    unsigned char msg_long[] = { MESSAGE_LONG };
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char msg_buffer[] = { MESSAGE_BUFFER };
    unsigned char msg_pointer[] = { MESSAGE_POINTER };
    unsigned char msg_time[] = { MESSAGE_TIME };
    unsigned char msg_hashtable[] = { MESSAGE_HASHTABLE };
    unsigned char msg_hdata[] = { MESSAGE_HDATA };
    unsigned char msg_info[] = { MESSAGE_INFO };
    unsigned char msg_infolist[] = { MESSAGE_INFOLIST };
    unsigned char msg_array[] = { MESSAGE_ARRAY };
    struct t_weechat_relay_msg *msg;
    void *ptr_compressed;
    size_t size;

    relay_message_display_object (NULL);

    relay_message_display (msg_invalid_size, sizeof (msg_invalid_size));
    relay_message_display (msg_invalid_id, sizeof (msg_invalid_id));
    relay_message_display (msg_invalid_compressed_data,
                           sizeof (msg_invalid_compressed_data));

    /* no compression, empty message */
    msg = weechat_relay_msg_new (NULL);
    relay_message_display (msg->data, msg->data_size);
    weechat_relay_msg_free (msg);

    /* no compression */
    MESSAGE_BUILD_FAKE(msg);
    relay_message_display (msg->data, msg->data_size);
    weechat_relay_msg_free (msg);

    /* zlib compression */
    MESSAGE_BUILD_FAKE(msg);
    ptr_compressed = weechat_relay_msg_compress_zlib (msg, 5, &size);
    CHECK(ptr_compressed);
    relay_message_display (ptr_compressed, size);

    /* zstd compression */
    MESSAGE_BUILD_FAKE(msg);
    ptr_compressed = weechat_relay_msg_compress_zstd (msg, 10, &size);
    CHECK(ptr_compressed);
    relay_message_display (ptr_compressed, size);

    /* test with every type supported */
    relay_message_display (msg_char, sizeof (msg_char));
    relay_message_display (msg_integer, sizeof (msg_integer));
    relay_message_display (msg_long, sizeof (msg_long));
    relay_message_display (msg_string, sizeof (msg_string));
    relay_message_display (msg_buffer, sizeof (msg_buffer));
    relay_message_display (msg_pointer, sizeof (msg_pointer));
    relay_message_display (msg_time, sizeof (msg_time));
    relay_message_display (msg_hashtable, sizeof (msg_hashtable));
    relay_message_display (msg_hdata, sizeof (msg_hdata));
    relay_message_display (msg_info, sizeof (msg_info));
    relay_message_display (msg_infolist, sizeof (msg_infolist));
    relay_message_display (msg_array, sizeof (msg_array));
}
