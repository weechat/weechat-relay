/*
 * test-lib-message.cpp - test build of binary messages
 *
 * Copyright (C) 2019-2024 Sébastien Helleu <flashcode@flashtux.org>
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
#include <string.h>
#include <arpa/inet.h>
#include "tests/tests.h"
#include "lib/weechat-relay.h"
}

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
    struct t_weechat_relay_msg *msg;

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
 *   weechat_relay_msg_set_bytes
 */

TEST(LibMessage, SetBytes)
{
    struct t_weechat_relay_msg *msg;
    const char *str1 = "abc";
    const char *str2 = "def";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (NULL, 0, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, 0, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, 0, str1, 0));
    LONGS_EQUAL(0, weechat_relay_msg_set_bytes (msg, 0, str1,
                                                WEECHAT_RELAY_MSG_INITIAL_ALLOC + 1));

    LONGS_EQUAL(1, weechat_relay_msg_add_bytes (msg, str1, strlen (str1)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(str1, msg->data + 13, strlen (str1));

    LONGS_EQUAL(1, weechat_relay_msg_set_bytes (msg, 13, str2, strlen (str2)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(str2, msg->data + 13, strlen (str2));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_bytes
 */

TEST(LibMessage, AddBytes)
{
    struct t_weechat_relay_msg *msg;
    const char *str = "abc";
    char buffer[WEECHAT_RELAY_MSG_INITIAL_ALLOC];

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_bytes (NULL, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_add_bytes (msg, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_msg_add_bytes (msg, str, 0));

    LONGS_EQUAL(1, weechat_relay_msg_add_bytes (msg, str, strlen (str)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(16, msg->data_size);
    MEMCMP_EQUAL(str, msg->data + 13, strlen (str));

    memset (buffer, 123, sizeof (buffer));
    LONGS_EQUAL(1, weechat_relay_msg_add_bytes (msg, buffer, sizeof (buffer)));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC * 2, msg->data_alloc);
    LONGS_EQUAL(16 + sizeof (buffer), msg->data_size);
    MEMCMP_EQUAL(buffer, msg->data + 16, sizeof (buffer));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_type
 */

TEST(LibMessage, AddType)
{
    struct t_weechat_relay_msg *msg;
    int i, pos;
    const char *ptr_type;

    msg = weechat_relay_msg_new ("test");

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        pos = msg->data_size;
        ptr_type = weechat_relay_obj_types_str[i];
        LONGS_EQUAL(1, weechat_relay_msg_add_type (msg, (enum t_weechat_relay_obj_type)i));
        LONGS_EQUAL(pos + strlen (ptr_type), msg->data_size);
        MEMCMP_EQUAL(ptr_type, msg->data + pos, strlen (ptr_type));
    }

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_char
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddChar)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    char ch = 123;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_char (NULL, 1));

    obj.type = WEECHAT_RELAY_OBJ_TYPE_CHAR;
    obj.value_char = ch;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(17, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_CHAR],
                 msg->data + 13, 3);
    BYTES_EQUAL(msg->data[16], ch);

    weechat_relay_msg_free (msg);
}


/*
 * Tests functions:
 *   weechat_relay_msg_add_integer
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddInteger)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    int number1 = 123456;
    uint32_t value32_number1;
    int number2 = -112233;
    uint32_t value32_number2;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_integer (NULL, 1));

    obj.type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    obj.value_integer = number1;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(20, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 13, 3);
    value32_number1 = htonl ((uint32_t)number1);
    MEMCMP_EQUAL(&value32_number1, msg->data + 16, 4);

    obj.type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    obj.value_integer = number2;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(27, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 20, 3);
    value32_number2 = htonl ((uint32_t)number2);
    MEMCMP_EQUAL(&value32_number2, msg->data + 23, 4);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_long
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddLong)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    long number1 = 123456789012;
    const char *str_number1 = "123456789012";
    long number2 = -112233445566;
    const char *str_number2 = "-112233445566";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_long (NULL, 1));

    obj.type = WEECHAT_RELAY_OBJ_TYPE_LONG;
    obj.value_long = number1;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(29, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_LONG],
                 msg->data + 13, 3);
    BYTES_EQUAL(12, msg->data[16]);
    MEMCMP_EQUAL(str_number1, msg->data + 17, strlen (str_number1));

    obj.type = WEECHAT_RELAY_OBJ_TYPE_LONG;
    obj.value_long = number2;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(46, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_LONG],
                 msg->data + 29, 3);
    BYTES_EQUAL(13, msg->data[32]);
    MEMCMP_EQUAL(str_number2, msg->data + 33, strlen (str_number2));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_string
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddString)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    const char *str = "abcdef";
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_string (NULL, str));

    /* NULL string: length = -1, no content */
    obj.type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    obj.value_string = NULL;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(20, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 13, 3);
    value32 = htonl (-1);
    MEMCMP_EQUAL(&value32, msg->data + 16, 4);

    /* empty string: length = 0, no content */
    obj.type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    obj.value_string = strdup ("");
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(27, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 20, 3);
    value32 = htonl (0);
    MEMCMP_EQUAL(&value32, msg->data + 23, 4);
    free (obj.value_string);

    /* string: length = length of string, content = the string (without \0) */
    obj.type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    obj.value_string = strdup (str);
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(40, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 27, 3);
    value32 = htonl (strlen (str));
    MEMCMP_EQUAL(&value32, msg->data + 30, 4);
    MEMCMP_EQUAL(str, msg->data + 34, strlen (str));
    free (obj.value_string);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_buffer
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddBuffer)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    struct t_weechat_relay_obj_buffer *ptr_buf;
    const void *str = "abcdef";
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    obj.type = WEECHAT_RELAY_OBJ_TYPE_BUFFER;
    ptr_buf = &obj.value_buffer;

    LONGS_EQUAL(0, weechat_relay_msg_add_buffer (NULL, NULL));

    obj.value_buffer.buffer = NULL;
    obj.value_buffer.length = 0;
    LONGS_EQUAL(0, weechat_relay_msg_add_buffer (NULL, ptr_buf));

    obj.value_buffer.buffer = (void *)str;
    obj.value_buffer.length = 0;
    LONGS_EQUAL(0, weechat_relay_msg_add_buffer (NULL, ptr_buf));

    /* NULL buffer: length = -1, no content */
    ptr_buf->buffer = NULL;
    ptr_buf->length = 0;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(20, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_BUFFER],
                 msg->data + 13, 3);
    value32 = htonl (-1);
    MEMCMP_EQUAL(&value32, msg->data + 16, 4);

    /* empty buffer: length = 0, no content */
    ptr_buf->buffer = (void *)str;
    ptr_buf->length = 0;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(27, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_BUFFER],
                 msg->data + 20, 3);
    value32 = htonl (0);
    MEMCMP_EQUAL(&value32, msg->data + 23, 4);

    /* buffer: length = length of buffer, content = the buffer */
    ptr_buf->buffer = (void *)str;
    ptr_buf->length = 6;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(40, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_BUFFER],
                 msg->data + 27, 3);
    value32 = htonl (6);
    MEMCMP_EQUAL(&value32, msg->data + 30, 4);
    MEMCMP_EQUAL(str, msg->data + 34, 6);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_pointer
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddPointer)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    void *pointer = (void *)0x123456789abc;
    const char *str_pointer = "123456789abc";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_pointer (NULL, (void *)1));

    /* NULL pointer */
    obj.type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    obj.value_pointer = NULL;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(18, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_POINTER],
                 msg->data + 13, 3);
    BYTES_EQUAL(1, msg->data[16]);
    BYTES_EQUAL('0', msg->data[17]);

    /* other pointer */
    obj.type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    obj.value_pointer = pointer;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(34, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_POINTER],
                 msg->data + 18, 3);
    BYTES_EQUAL(12, msg->data[21]);
    MEMCMP_EQUAL(str_pointer, msg->data + 22, strlen (str_pointer));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_time
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddTime)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    time_t time = 123456789;
    const char *str_time = "123456789";

    msg = weechat_relay_msg_new ("test");

    LONGS_EQUAL(0, weechat_relay_msg_add_time (NULL, 123));

    obj.type = WEECHAT_RELAY_OBJ_TYPE_TIME;
    obj.value_time = time;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(26, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_TIME],
                 msg->data + 13, 3);
    BYTES_EQUAL(9, msg->data[16]);
    MEMCMP_EQUAL(str_time, msg->data + 17, strlen (str_time));

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_hashtable
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddHashtable)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    struct t_weechat_relay_obj_hashtable *ptr_hash;
    struct t_weechat_relay_obj obj_str1, obj_str2, obj_int1, obj_int2;
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    obj.type = WEECHAT_RELAY_OBJ_TYPE_HASHTABLE;
    ptr_hash = &obj.value_hashtable;

    obj_str1.type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    obj_str1.value_string = strdup ("string1");
    obj_str2.type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    obj_str2.value_string = strdup ("string2");

    obj_int1.type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    obj_int1.value_integer = 123;
    obj_int2.type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    obj_int2.value_integer = 456;

    ptr_hash->type_keys = WEECHAT_RELAY_OBJ_TYPE_ARRAY;
    ptr_hash->type_values = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_hash->count = 0;
    ptr_hash->keys = NULL;
    ptr_hash->values = NULL;

    LONGS_EQUAL(0, weechat_relay_msg_add_hashtable (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_hashtable (msg, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_hashtable (NULL, ptr_hash));

    /* wrong types */
    ptr_hash->type_keys = WEECHAT_RELAY_OBJ_TYPE_ARRAY;
    ptr_hash->type_values = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_hash->count = 0;
    ptr_hash->keys = NULL;
    ptr_hash->values = NULL;
    LONGS_EQUAL(0, weechat_relay_msg_add_hashtable (msg, ptr_hash));
    ptr_hash->type_keys = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_hash->type_values = WEECHAT_RELAY_OBJ_TYPE_ARRAY;
    ptr_hash->count = 0;
    ptr_hash->keys = NULL;
    ptr_hash->values = NULL;
    LONGS_EQUAL(0, weechat_relay_msg_add_hashtable (msg, ptr_hash));
    weechat_relay_msg_free (msg);

    /* empty hashtable */
    msg = weechat_relay_msg_new ("test");
    ptr_hash->type_keys = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_hash->type_values = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_hash->count = 0;
    ptr_hash->keys = NULL;
    ptr_hash->values = NULL;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(26, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_HASHTABLE],
                 msg->data + 13,
                 3);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 16,
                 3);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 19,
                 3);
    value32 = htonl (0);
    MEMCMP_EQUAL(&value32, msg->data + 22, 4);
    weechat_relay_msg_free (msg);

    /* hashtable with 2 keys */
    msg = weechat_relay_msg_new ("test");
    ptr_hash->type_keys = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_hash->type_values = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_hash->count = 2;
    ptr_hash->keys = (struct t_weechat_relay_obj **)malloc (2 * sizeof (*ptr_hash->keys));
    ptr_hash->values = (struct t_weechat_relay_obj **)malloc (2 * sizeof (*ptr_hash->values));
    ptr_hash->keys[0] = &obj_str1;
    ptr_hash->keys[1] = &obj_str2;
    ptr_hash->values[0] = &obj_int1;
    ptr_hash->values[1] = &obj_int2;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(56, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_HASHTABLE],
                 msg->data + 13,
                 3);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 16,
                 3);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 19,
                 3);
    /* count */
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 22, 4);
    /* item 1: "string1" -> 123 */
    value32 = htonl (7);
    MEMCMP_EQUAL(&value32, msg->data + 26, 4);
    MEMCMP_EQUAL("string1", msg->data + 30, 7);
    value32 = htonl (123);
    MEMCMP_EQUAL(&value32, msg->data + 37, 4);
    /* item 2: "string2" -> 456 */
    value32 = htonl (7);
    MEMCMP_EQUAL(&value32, msg->data + 41, 4);
    MEMCMP_EQUAL("string2", msg->data + 45, 7);
    value32 = htonl (456);
    MEMCMP_EQUAL(&value32, msg->data + 52, 4);
    free (ptr_hash->keys);
    free (ptr_hash->values);
    weechat_relay_msg_free (msg);

    free (obj_str1.value_string);
    free (obj_str2.value_string);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_hdata
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddHdata)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    struct t_weechat_relay_obj_hdata *ptr_hdata;
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    obj.type = WEECHAT_RELAY_OBJ_TYPE_HDATA;
    ptr_hdata = &obj.value_hdata;

    ptr_hdata->hpath = NULL;
    ptr_hdata->num_hpaths = 0;
    ptr_hdata->hpaths = NULL;
    ptr_hdata->keys = NULL;
    ptr_hdata->num_keys = 0;
    ptr_hdata->keys_names = NULL;
    ptr_hdata->keys_types = NULL;
    ptr_hdata->count = 0;
    ptr_hdata->ppath = NULL;
    ptr_hdata->values = NULL;

    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (NULL, ptr_hdata));

    /* missing data in hdata */
    ptr_hdata->hpath = strdup ("path1/path2/path3");
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->num_hpaths = 3;
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->hpaths = (char **)malloc (3 * sizeof (*ptr_hdata->hpaths));
    ptr_hdata->hpaths[0] = strdup ("path1");
    ptr_hdata->hpaths[1] = strdup ("path2");
    ptr_hdata->hpaths[2] = strdup ("path3");
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->keys = strdup ("key1:str,key2:int");
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->num_keys = 2;
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->keys_names = (char **)malloc (2 * sizeof (*ptr_hdata->keys_names));
    ptr_hdata->keys_names[0] = strdup ("key1");
    ptr_hdata->keys_names[1] = strdup ("key2");
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->keys_types = (enum t_weechat_relay_obj_type *)malloc (2 * sizeof (*ptr_hdata->keys_types));
    ptr_hdata->keys_types[0] = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_hdata->keys_types[1] = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->count = 2;
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->ppath = (struct t_weechat_relay_obj ***)malloc (2 * sizeof (*ptr_hdata->ppath));
    ptr_hdata->ppath[0] = (struct t_weechat_relay_obj **)malloc (3 * sizeof (**ptr_hdata->ppath));
    ptr_hdata->ppath[0][0] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->ppath));
    ptr_hdata->ppath[0][0]->type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    ptr_hdata->ppath[0][0]->value_pointer = (const void *)0x123;
    ptr_hdata->ppath[0][1] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->ppath));
    ptr_hdata->ppath[0][1]->type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    ptr_hdata->ppath[0][1]->value_pointer = (const void *)0x456;
    ptr_hdata->ppath[0][2] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->ppath));
    ptr_hdata->ppath[0][2]->type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    ptr_hdata->ppath[0][2]->value_pointer = (const void *)0x789;
    ptr_hdata->ppath[1] = (struct t_weechat_relay_obj **)malloc (3 * sizeof (**ptr_hdata->ppath));
    ptr_hdata->ppath[1][0] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->ppath));
    ptr_hdata->ppath[1][0]->type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    ptr_hdata->ppath[1][0]->value_pointer = (const void *)0xaaa;
    ptr_hdata->ppath[1][1] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->ppath));
    ptr_hdata->ppath[1][1]->type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    ptr_hdata->ppath[1][1]->value_pointer = (const void *)0xbbb;
    ptr_hdata->ppath[1][2] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->ppath));
    ptr_hdata->ppath[1][2]->type = WEECHAT_RELAY_OBJ_TYPE_POINTER;
    ptr_hdata->ppath[1][2]->value_pointer = (const void *)0xccc;
    LONGS_EQUAL(0, weechat_relay_msg_add_hdata (msg, ptr_hdata));
    ptr_hdata->values = (struct t_weechat_relay_obj ***)malloc (2 * sizeof (*ptr_hdata->values));
    ptr_hdata->values[0] = (struct t_weechat_relay_obj **)malloc (2 * sizeof (**ptr_hdata->values));
    ptr_hdata->values[0][0] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->values));
    ptr_hdata->values[0][0]->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_hdata->values[0][0]->value_string = strdup ("string1");
    ptr_hdata->values[0][1] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->values));
    ptr_hdata->values[0][1]->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_hdata->values[0][1]->value_integer = 123;
    ptr_hdata->values[1] = (struct t_weechat_relay_obj **)malloc (2 * sizeof (**ptr_hdata->values));
    ptr_hdata->values[1][0] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->values));
    ptr_hdata->values[1][0]->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_hdata->values[1][0]->value_string = strdup ("string2");
    ptr_hdata->values[1][1] = (struct t_weechat_relay_obj *)malloc (sizeof (***ptr_hdata->values));
    ptr_hdata->values[1][1]->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_hdata->values[1][1]->value_integer = 456;

    /* hdata OK */
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(116, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_HDATA],
                 msg->data + 13,
                 3);
    value32 = htonl (17);
    MEMCMP_EQUAL(&value32, msg->data + 16, 4);
    MEMCMP_EQUAL("path1/path2/path3", msg->data + 20, 17);
    value32 = htonl (17);
    MEMCMP_EQUAL(&value32, msg->data + 37, 4);
    MEMCMP_EQUAL("key1:str,key2:int", msg->data + 41, 17);
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 58, 4);
    /* obj1 */
    LONGS_EQUAL(3, msg->data[62]);
    MEMCMP_EQUAL("123", msg->data + 63, 3);
    LONGS_EQUAL(3, msg->data[66]);
    MEMCMP_EQUAL("456", msg->data + 67, 3);
    LONGS_EQUAL(3, msg->data[70]);
    MEMCMP_EQUAL("789", msg->data + 71, 3);
    value32 = htonl (7);
    MEMCMP_EQUAL(&value32, msg->data + 74, 4);
    MEMCMP_EQUAL("string1", msg->data + 78, 7);
    value32 = htonl (123);
    MEMCMP_EQUAL(&value32, msg->data + 85, 4);
    /* obj2 */
    LONGS_EQUAL(3, msg->data[89]);
    MEMCMP_EQUAL("aaa", msg->data + 90, 3);
    LONGS_EQUAL(3, msg->data[93]);
    MEMCMP_EQUAL("bbb", msg->data + 94, 3);
    LONGS_EQUAL(3, msg->data[97]);
    MEMCMP_EQUAL("ccc", msg->data + 98, 3);
    value32 = htonl (7);
    MEMCMP_EQUAL(&value32, msg->data + 101, 4);
    MEMCMP_EQUAL("string2", msg->data + 105, 7);
    value32 = htonl (456);
    MEMCMP_EQUAL(&value32, msg->data + 112, 4);

    free (ptr_hdata->hpath);
    free (ptr_hdata->hpaths[0]);
    free (ptr_hdata->hpaths[1]);
    free (ptr_hdata->hpaths[2]);
    free (ptr_hdata->hpaths);
    free (ptr_hdata->keys);
    free (ptr_hdata->keys_names[0]);
    free (ptr_hdata->keys_names[1]);
    free (ptr_hdata->keys_names);
    free (ptr_hdata->keys_types);
    free (ptr_hdata->ppath[0][0]);
    free (ptr_hdata->ppath[0][1]);
    free (ptr_hdata->ppath[0][2]);
    free (ptr_hdata->ppath[1][0]);
    free (ptr_hdata->ppath[1][1]);
    free (ptr_hdata->ppath[1][2]);
    free (ptr_hdata->ppath[0]);
    free (ptr_hdata->ppath[1]);
    free (ptr_hdata->ppath);
    free (ptr_hdata->values[0][0]->value_string);
    free (ptr_hdata->values[1][0]->value_string);
    free (ptr_hdata->values[0][0]);
    free (ptr_hdata->values[0][1]);
    free (ptr_hdata->values[1][0]);
    free (ptr_hdata->values[1][1]);
    free (ptr_hdata->values[0]);
    free (ptr_hdata->values[1]);
    free (ptr_hdata->values);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_info
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddInfo)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    struct t_weechat_relay_obj_info *ptr_info;
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    obj.type = WEECHAT_RELAY_OBJ_TYPE_INFO;
    ptr_info = &obj.value_info;

    ptr_info->name = NULL;
    ptr_info->value = NULL;

    LONGS_EQUAL(0, weechat_relay_msg_add_info (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_info (msg, NULL));

    /* NULL name and value */
    ptr_info->name = NULL;
    ptr_info->value = NULL;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(24, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INFO],
                 msg->data + 13,
                 3);
    value32 = htonl (-1);
    MEMCMP_EQUAL(&value32, msg->data + 16, 4);
    value32 = htonl (-1);
    MEMCMP_EQUAL(&value32, msg->data + 20, 4);

    /* empty name and value */
    ptr_info->name = strdup ("");
    ptr_info->value = strdup ("");
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(35, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INFO],
                 msg->data + 24,
                 3);
    value32 = htonl (0);
    MEMCMP_EQUAL(&value32, msg->data + 27, 4);
    value32 = htonl (0);
    MEMCMP_EQUAL(&value32, msg->data + 31, 4);
    free (ptr_info->name);
    free (ptr_info->value);

    /* name and value */
    ptr_info->name = strdup ("name");
    ptr_info->value = strdup ("value");
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(55, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INFO],
                 msg->data + 35,
                 3);
    value32 = htonl (4);
    MEMCMP_EQUAL(&value32, msg->data + 38, 4);
    MEMCMP_EQUAL("name", msg->data + 42, 4);
    value32 = htonl (5);
    MEMCMP_EQUAL(&value32, msg->data + 46, 4);
    MEMCMP_EQUAL("value", msg->data + 50, 5);
    free (ptr_info->name);
    free (ptr_info->value);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_infolist
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddInfolist)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    struct t_weechat_relay_obj_infolist *ptr_infolist;
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    obj.type = WEECHAT_RELAY_OBJ_TYPE_INFOLIST;
    ptr_infolist = &obj.value_infolist;

    ptr_infolist->name = NULL;
    ptr_infolist->count = -1;
    ptr_infolist->items = NULL;

    LONGS_EQUAL(0, weechat_relay_msg_add_infolist (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_infolist (msg, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_infolist (NULL, ptr_infolist));

    /* missing data in infolist */
    ptr_infolist->name = NULL;
    ptr_infolist->count = -1;
    ptr_infolist->items = NULL;
    LONGS_EQUAL(0, weechat_relay_msg_add_infolist (msg, ptr_infolist));
    ptr_infolist->name = strdup ("test");
    LONGS_EQUAL(0, weechat_relay_msg_add_infolist (msg, ptr_infolist));
    ptr_infolist->count = 2;
    ptr_infolist->items = (struct t_weechat_relay_obj_infolist_item **)malloc (2 * sizeof (*ptr_infolist->items));
    ptr_infolist->items[0] = (struct t_weechat_relay_obj_infolist_item *)malloc (sizeof (*ptr_infolist->items[0]));
    ptr_infolist->items[0]->count = 2;
    ptr_infolist->items[0]->variables = (struct t_weechat_relay_obj_infolist_var **)malloc (2 * sizeof (*ptr_infolist->items[0]->variables));
    ptr_infolist->items[0]->variables[0] = (struct t_weechat_relay_obj_infolist_var *)malloc (sizeof (*ptr_infolist->items[0]->variables[0]));
    ptr_infolist->items[0]->variables[0]->name = strdup ("var1");
    ptr_infolist->items[0]->variables[0]->value = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_infolist->items[0]->variables[0]->value));
    ptr_infolist->items[0]->variables[0]->value->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_infolist->items[0]->variables[0]->value->value_string = strdup ("item1_value");
    ptr_infolist->items[0]->variables[1] = (struct t_weechat_relay_obj_infolist_var *)malloc (sizeof (*ptr_infolist->items[0]->variables[1]));
    ptr_infolist->items[0]->variables[1]->name = strdup ("var2");
    ptr_infolist->items[0]->variables[1]->value = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_infolist->items[0]->variables[1]->value));
    ptr_infolist->items[0]->variables[1]->value->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_infolist->items[0]->variables[1]->value->value_integer = 123;
    ptr_infolist->items[1] = (struct t_weechat_relay_obj_infolist_item *)malloc (sizeof (*ptr_infolist->items[1]));
    ptr_infolist->items[1]->count = 2;
    ptr_infolist->items[1]->variables = (struct t_weechat_relay_obj_infolist_var **)malloc (2 * sizeof (*ptr_infolist->items[1]->variables));
    ptr_infolist->items[1]->variables[0] = (struct t_weechat_relay_obj_infolist_var *)malloc (sizeof (*ptr_infolist->items[1]->variables[0]));
    ptr_infolist->items[1]->variables[0]->name = strdup ("var1");
    ptr_infolist->items[1]->variables[0]->value = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_infolist->items[0]->variables[0]->value));
    ptr_infolist->items[1]->variables[0]->value->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_infolist->items[1]->variables[0]->value->value_string = strdup ("item2_value");
    ptr_infolist->items[1]->variables[1] = (struct t_weechat_relay_obj_infolist_var *)malloc (sizeof (*ptr_infolist->items[1]->variables[1]));
    ptr_infolist->items[1]->variables[1]->name = strdup ("var2");
    ptr_infolist->items[1]->variables[1]->value = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_infolist->items[0]->variables[1]->value));
    ptr_infolist->items[1]->variables[1]->value->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_infolist->items[1]->variables[1]->value->value_integer = 456;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(118, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INFOLIST],
                 msg->data + 13,
                 3);
    /* name */
    value32 = htonl (4);
    MEMCMP_EQUAL(&value32, msg->data + 16, 4);
    MEMCMP_EQUAL("test", msg->data + 20, 4);
    /* count */
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 24, 4);
    /* item 1: count */
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 28, 4);
    /* item 1: var 1 */
    value32 = htonl (4);
    MEMCMP_EQUAL(&value32, msg->data + 32, 4);
    MEMCMP_EQUAL("var1", msg->data + 36, 4);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 40, 3);
    value32 = htonl (11);
    MEMCMP_EQUAL(&value32, msg->data + 43, 4);
    MEMCMP_EQUAL("item1_value", msg->data + 47, 11);
    /* item 1: var 2 */
    value32 = htonl (4);
    MEMCMP_EQUAL(&value32, msg->data + 58, 4);
    MEMCMP_EQUAL("var2", msg->data + 62, 4);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 66, 3);
    value32 = htonl (123);
    MEMCMP_EQUAL(&value32, msg->data + 69, 4);
    /* item 2: count */
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 73, 4);
    /* item 2: var 1 */
    value32 = htonl (4);
    MEMCMP_EQUAL(&value32, msg->data + 77, 4);
    MEMCMP_EQUAL("var1", msg->data + 81, 4);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 85, 3);
    value32 = htonl (11);
    MEMCMP_EQUAL(&value32, msg->data + 88, 4);
    MEMCMP_EQUAL("item2_value", msg->data + 92, 11);
    /* item 2: var 2 */
    value32 = htonl (4);
    MEMCMP_EQUAL(&value32, msg->data + 103, 4);
    MEMCMP_EQUAL("var2", msg->data + 107, 4);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 111, 3);
    value32 = htonl (456);
    MEMCMP_EQUAL(&value32, msg->data + 114, 4);

    free (ptr_infolist->name);
    free (ptr_infolist->items[0]->variables[0]->name);
    free (ptr_infolist->items[0]->variables[0]->value->value_string);
    free (ptr_infolist->items[0]->variables[0]->value);
    free (ptr_infolist->items[0]->variables[1]->name);
    free (ptr_infolist->items[0]->variables[1]->value);
    free (ptr_infolist->items[0]->variables[0]);
    free (ptr_infolist->items[0]->variables[1]);
    free (ptr_infolist->items[0]->variables);
    free (ptr_infolist->items[1]->variables[0]->name);
    free (ptr_infolist->items[1]->variables[0]->value->value_string);
    free (ptr_infolist->items[1]->variables[0]->value);
    free (ptr_infolist->items[1]->variables[1]->name);
    free (ptr_infolist->items[1]->variables[1]->value);
    free (ptr_infolist->items[1]->variables[0]);
    free (ptr_infolist->items[1]->variables[1]);
    free (ptr_infolist->items[1]->variables);
    free (ptr_infolist->items[0]);
    free (ptr_infolist->items[1]);
    free (ptr_infolist->items);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_add_array
 *   weechat_relay_msg_add_object
 */

TEST(LibMessage, AddArray)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj obj;
    struct t_weechat_relay_obj_array *ptr_array;
    uint32_t value32;

    msg = weechat_relay_msg_new ("test");

    obj.type = WEECHAT_RELAY_OBJ_TYPE_ARRAY;
    ptr_array = &obj.value_array;

    ptr_array->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_array->count = -1;
    ptr_array->values = NULL;

    LONGS_EQUAL(0, weechat_relay_msg_add_array (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_array (msg, NULL));
    LONGS_EQUAL(0, weechat_relay_msg_add_array (NULL, ptr_array));

    /* missing data in array */
    ptr_array->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_array->count = -1;
    ptr_array->values = NULL;
    LONGS_EQUAL(0, weechat_relay_msg_add_array (msg, ptr_array));

    /* array of 2 strings */
    ptr_array->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_array->count = 2;
    ptr_array->values = (struct t_weechat_relay_obj **)malloc (2 * sizeof (*ptr_array->values));
    ptr_array->values[0] = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_array->values[0]));
    ptr_array->values[0]->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_array->values[0]->value_string = strdup ("value1");
    ptr_array->values[1] = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_array->values[0]));
    ptr_array->values[1]->type = WEECHAT_RELAY_OBJ_TYPE_STRING;
    ptr_array->values[1]->value_string = strdup ("value2");
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(43, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_ARRAY],
                 msg->data + 13, 3);
    /* type */
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_STRING],
                 msg->data + 16, 3);
    /* count */
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 19, 4);
    /* item 1: "value1" */
    value32 = htonl (6);
    MEMCMP_EQUAL(&value32, msg->data + 23, 4);
    MEMCMP_EQUAL("value1", msg->data + 27, 6);
    /* item 2: "value2" */
    value32 = htonl (6);
    MEMCMP_EQUAL(&value32, msg->data + 33, 4);
    MEMCMP_EQUAL("value2", msg->data + 37, 6);

    free (ptr_array->values[0]->value_string);
    free (ptr_array->values[1]->value_string);
    free (ptr_array->values[0]);
    free (ptr_array->values[1]);
    free (ptr_array->values);

    /* array of 2 integers */
    ptr_array->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_array->count = 2;
    ptr_array->values = (struct t_weechat_relay_obj **)malloc (2 * sizeof (*ptr_array->values));
    ptr_array->values[0] = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_array->values[0]));
    ptr_array->values[0]->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_array->values[0]->value_integer = 123;
    ptr_array->values[1] = (struct t_weechat_relay_obj *)malloc (sizeof (*ptr_array->values[0]));
    ptr_array->values[1]->type = WEECHAT_RELAY_OBJ_TYPE_INTEGER;
    ptr_array->values[1]->value_integer = 456;
    LONGS_EQUAL(1, weechat_relay_msg_add_object (msg, &obj));
    LONGS_EQUAL(WEECHAT_RELAY_MSG_INITIAL_ALLOC, msg->data_alloc);
    LONGS_EQUAL(61, msg->data_size);
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_ARRAY],
                 msg->data + 43, 3);
    /* type */
    MEMCMP_EQUAL(weechat_relay_obj_types_str[WEECHAT_RELAY_OBJ_TYPE_INTEGER],
                 msg->data + 46, 3);
    /* count */
    value32 = htonl (2);
    MEMCMP_EQUAL(&value32, msg->data + 49, 4);
    /* item 1: 123 */
    value32 = htonl (123);
    MEMCMP_EQUAL(&value32, msg->data + 53, 4);
    /* item 2: 456 */
    value32 = htonl (456);
    MEMCMP_EQUAL(&value32, msg->data + 57, 4);

    free (ptr_array->values[0]);
    free (ptr_array->values[1]);
    free (ptr_array->values);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_compress_zlib
 */

TEST(LibMessage, CompressZlib)
{
    struct t_weechat_relay_msg *msg;
    void *buffer;
    size_t size;

    POINTERS_EQUAL(NULL, weechat_relay_msg_compress_zlib (NULL, 0, NULL));
    POINTERS_EQUAL(NULL, weechat_relay_msg_compress_zlib (NULL, 0, &size));

    MESSAGE_BUILD_FAKE(msg);

    /* initial message size */
    LONGS_EQUAL(4186, msg->data_size);

    /* compression level 0 (no compression) */
    buffer = weechat_relay_msg_compress_zlib (msg, 0, &size);
    LONGS_EQUAL(4197, size);
    free (buffer);

    /* compression level 1 (highest speed) */
    buffer = weechat_relay_msg_compress_zlib (msg, 1, &size);
    LONGS_EQUAL(1798, size);
    free (buffer);

    /* compression level 9 (highest compression) */
    buffer = weechat_relay_msg_compress_zlib (msg, 9, &size);
    LONGS_EQUAL(1701, size);
    free (buffer);

    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_msg_compress_zstd
 */

TEST(LibMessage, CompressZstd)
{
    struct t_weechat_relay_msg *msg;
    void *buffer;
    size_t size;

    POINTERS_EQUAL(NULL, weechat_relay_msg_compress_zstd (NULL, 0, NULL));
    POINTERS_EQUAL(NULL, weechat_relay_msg_compress_zstd (NULL, 0, &size));

    MESSAGE_BUILD_FAKE(msg);

    /* initial message size */
    LONGS_EQUAL(4186, msg->data_size);

    /* compression level 1 (highest speed) */
    buffer = weechat_relay_msg_compress_zstd (msg, 1, &size);
    CHECK(buffer);
    CHECK(size > 0);
    CHECK(size < 4186);
    free (buffer);

    /* compression level 9 (highest compression) */
    buffer = weechat_relay_msg_compress_zstd (msg, 19, &size);
    CHECK(buffer);
    CHECK(size > 0);
    CHECK(size < 4186);
    free (buffer);

    weechat_relay_msg_free (msg);
}
