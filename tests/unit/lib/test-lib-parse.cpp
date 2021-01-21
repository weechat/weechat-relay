/*
 * test-lib-parse.cpp - test parsing of binary messages
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
#include "string.h"
#include "tests/tests.h"
#include "lib/weechat-relay.h"

extern const char *weechat_relay_obj_types_str[];

extern int weechat_relay_parse_search_type (const char *type);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_alloc (
    enum t_weechat_relay_obj_type type);
extern void weechat_relay_parse_obj_free (
    struct t_weechat_relay_obj *obj);
extern int weechat_relay_parse_read_bytes (
    struct t_weechat_relay_msg *msg, void *output, size_t count);
extern int weechat_relay_parse_read_type (
    struct t_weechat_relay_msg *msg,
    enum t_weechat_relay_obj_type *type);
extern int weechat_relay_parse_read_integer (
    struct t_weechat_relay_msg *msg, int *value);
extern int weechat_relay_parse_read_string (
    struct t_weechat_relay_msg *msg, char **string);
extern int weechat_relay_parse_read_buffer (
    struct t_weechat_relay_msg *msg, void **buffer, int *length);
extern int weechat_relay_parse_read_pointer (
    struct t_weechat_relay_msg *msg, const void **pointer);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_char (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_integer (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_long (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_string (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_buffer (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_pointer (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_time (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_hashtable (
    struct t_weechat_relay_msg *msg);
extern int weechat_relay_parse_hdata_split_hpath (
    const char *hpath, char ***hpaths, int *num_hpaths);
extern int weechat_relay_parse_hdata_split_keys (
    const char *keys, char ***keys_names,
    enum t_weechat_relay_obj_type **keys_types, int *num_keys);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_hdata (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_info (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_infolist (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_array (
    struct t_weechat_relay_msg *msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_read_object (
    struct t_weechat_relay_msg *msg,
    enum t_weechat_relay_obj_type type);
extern void *weechat_relay_parse_decompress (const void *data, size_t size,
                                             size_t initial_output_size,
                                             size_t *size_decompressed);
extern struct t_weechat_relay_msg *weechat_relay_parse_msg_alloc (
    const void *buffer, size_t size);
extern void weechat_relay_parse_msg_free (
    struct t_weechat_relay_msg *msg);
}

TEST_GROUP(LibParse)
{
};

/*
 * Tests functions:
 *   weechat_relay_parse_search_type
 */

TEST(LibParse, SearchType)
{
    int i;

    LONGS_EQUAL(-1, weechat_relay_parse_search_type (NULL));
    LONGS_EQUAL(-1, weechat_relay_parse_search_type (""));
    LONGS_EQUAL(-1, weechat_relay_parse_search_type ("xxx"));

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        LONGS_EQUAL(i,
                    weechat_relay_parse_search_type (
                        weechat_relay_obj_types_str[i]));
    }
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_alloc
 *   weechat_relay_parse_obj_free
 */

TEST(LibParse, ObjAllocFree)
{
    struct t_weechat_relay_obj *obj;
    int i;

    weechat_relay_parse_obj_free (NULL);

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        obj = weechat_relay_parse_obj_alloc ((t_weechat_relay_obj_type)i);
        CHECK(obj);
        LONGS_EQUAL(i, obj->type);
        weechat_relay_parse_obj_free (obj);
    }
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_bytes
 */

TEST(LibParse, ReadBytes)
{
    struct t_weechat_relay_msg *msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    char type[4], output[4];
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (NULL, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (NULL, output, 0));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (NULL, output, 3));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (msg, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (msg, NULL, 3));

    /* buffer too small */
    msg->buffer = type;
    msg->size = 2;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (msg, output, 3));

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        snprintf (type, sizeof (type), weechat_relay_obj_types_str[i]);
        msg->buffer = type;
        msg->size = 3;
        msg->position = 0;
        LONGS_EQUAL(1, weechat_relay_parse_read_bytes (msg, output, 3));
        MEMCMP_EQUAL(weechat_relay_obj_types_str[i], output, 3);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_type
 */

TEST(LibParse, ReadType)
{
    struct t_weechat_relay_msg *msg;
    enum t_weechat_relay_obj_type type;
    unsigned char msg_one_str[] = { MESSAGE_STRING };
    char str_type[4];
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_one_str, sizeof (msg_one_str));

    LONGS_EQUAL(0, weechat_relay_parse_read_type (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_type (NULL, &type));
    LONGS_EQUAL(0, weechat_relay_parse_read_type (msg, NULL));

    /* buffer too small */
    msg->buffer = str_type;
    msg->size = 2;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_type (msg, &type));

    /* invalid type */
    snprintf (str_type, sizeof (str_type), "XXX");
    msg->buffer = str_type;
    msg->size = 3;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_type (msg, &type));

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        type = (i == WEECHAT_RELAY_OBJ_TYPE_CHAR) ?
            WEECHAT_RELAY_OBJ_TYPE_STRING : WEECHAT_RELAY_OBJ_TYPE_CHAR;
        snprintf (str_type, sizeof (str_type), weechat_relay_obj_types_str[i]);
        msg->buffer = str_type;
        msg->size = 3;
        msg->position = 0;
        LONGS_EQUAL(1, weechat_relay_parse_read_type (msg, &type));
        LONGS_EQUAL(i, type);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_integer
 */

TEST(LibParse, ReadInteger)
{
    struct t_weechat_relay_msg *msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char int_123456[] = { 0x00, 0x01, 0xE2, 0x40 };
    unsigned char int_minus_123456[] = { 0xFF, 0xFE, 0x1D, 0xC0 };
    int value;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_integer (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_integer (NULL, &value));
    LONGS_EQUAL(0, weechat_relay_parse_read_integer (msg, NULL));

    /* buffer too small */
    msg->buffer = int_123456;
    msg->size = sizeof (int_123456) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_integer (msg, &value));

    /* valid integer: 123456 */
    msg->buffer = int_123456;
    msg->size = sizeof (int_123456);
    msg->position = 0;
    value = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_integer (msg, &value));
    LONGS_EQUAL(123456, value);

    /* valid integer: -123456 */
    msg->buffer = int_minus_123456;
    msg->size = sizeof (int_minus_123456);
    msg->position = 0;
    value = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_integer (msg, &value));
    LONGS_EQUAL(-123456, value);

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_string
 */

TEST(LibParse, ReadString)
{
    struct t_weechat_relay_msg *msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char str_null[] = { 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char str_empty[] = { 0x00, 0x00, 0x00, 0x00 };
    unsigned char str_abc[] = { 0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c' };
    char *str;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_string (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_string (NULL, &str));
    LONGS_EQUAL(0, weechat_relay_parse_read_string (msg, NULL));

    /* buffer too small */
    msg->buffer = str_empty;
    msg->size = sizeof (str_empty) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_string (msg, &str));

    /* buffer too small */
    msg->buffer = str_abc;
    msg->size = sizeof (str_abc) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_string (msg, &str));

    /* valid string: NULL */
    msg->buffer = str_null;
    msg->size = sizeof (str_null);
    msg->position = 0;
    str = (char *)0x123;
    LONGS_EQUAL(1, weechat_relay_parse_read_string (msg, &str));
    POINTERS_EQUAL(NULL, str);

    /* valid string: "" (empty) */
    msg->buffer = str_empty;
    msg->size = sizeof (str_empty);
    msg->position = 0;
    str = NULL;
    LONGS_EQUAL(1, weechat_relay_parse_read_string (msg, &str));
    STRCMP_EQUAL("", str);
    free (str);

    /* valid string: "abc" */
    msg->buffer = str_abc;
    msg->size = sizeof (str_abc);
    msg->position = 0;
    str = NULL;
    LONGS_EQUAL(1, weechat_relay_parse_read_string (msg, &str));
    STRCMP_EQUAL("abc", str);
    free (str);

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_buffer
 */

TEST(LibParse, ReadBuffer)
{
    struct t_weechat_relay_msg *msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char buffer_null1[] = { 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char buffer_null2[] = { 0x00, 0x00, 0x00, 0x00 };
    unsigned char buffer_abc[] = { 0x00, 0x00, 0x00, 0x03, 0x61, 0x62, 0x63 };
    void *buffer;
    int length;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL, NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (msg, NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL,  &buffer, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL, NULL, &length));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (msg, &buffer, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (msg, NULL, &length));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL, &buffer, &length));

    /* buffer too small */
    msg->buffer = buffer_null1;
    msg->size = sizeof (buffer_null1) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (msg, &buffer, &length));

    /* buffer too small */
    msg->buffer = buffer_abc;
    msg->size = sizeof (buffer_abc) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (msg, &buffer, &length));

    /* valid buffer: NULL (length = -1) */
    msg->buffer = buffer_null1;
    msg->size = sizeof (buffer_null1);
    msg->position = 0;
    buffer = (char *)0x123;
    length = 1;
    LONGS_EQUAL(1, weechat_relay_parse_read_buffer (msg, &buffer, &length));
    POINTERS_EQUAL(NULL, buffer);
    LONGS_EQUAL(-1, length);

    /* valid buffer: NULL (length = 0) */
    msg->buffer = buffer_null2;
    msg->size = sizeof (buffer_null2);
    msg->position = 0;
    buffer = (char *)0x123;
    length = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_buffer (msg, &buffer, &length));
    POINTERS_EQUAL(NULL, buffer);
    LONGS_EQUAL(0, length);

    /* valid buffer: "abc" */
    msg->buffer = buffer_abc;
    msg->size = sizeof (buffer_abc);
    msg->position = 0;
    buffer = NULL;
    length = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_buffer (msg, &buffer, &length));
    MEMCMP_EQUAL(buffer_abc + 4, buffer, sizeof (buffer_abc) - 4);
    free (buffer);

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_pointer
 */

TEST(LibParse, ReadPointer)
{
    struct t_weechat_relay_msg *msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char pointer_null[] = { 0x01, 0x30 };
    unsigned char pointer_invalid[] = { 0x01, 0x02 };
    unsigned char pointer_1a2b3c4d5[] = { 0x09, 0x31, 0x61, 0x32, 0x62,
                                          0x33, 0x63, 0x34, 0x64, 0x35 };
    const void *pointer;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (msg, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (NULL,  &pointer));

    /* buffer too small */
    msg->buffer = pointer_null;
    msg->size = sizeof (pointer_null) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (msg, &pointer));

    /* buffer too small */
    msg->buffer = pointer_1a2b3c4d5;
    msg->size = sizeof (pointer_1a2b3c4d5) - 1;
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (msg, &pointer));

    /* invalid pointer */
    msg->buffer = pointer_invalid;
    msg->size = sizeof (pointer_invalid);
    msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (msg, &pointer));

    /* valid pointer: NULL */
    msg->buffer = pointer_null;
    msg->size = sizeof (pointer_null);
    msg->position = 0;
    pointer = (char *)0x123;
    LONGS_EQUAL(1, weechat_relay_parse_read_pointer (msg, &pointer));
    POINTERS_EQUAL(NULL, pointer);

    /* valid buffer: 0x1a2b3c4d5 */
    msg->buffer = pointer_1a2b3c4d5;
    msg->size = sizeof (pointer_1a2b3c4d5);
    msg->position = 0;
    pointer = NULL;
    LONGS_EQUAL(1, weechat_relay_parse_read_pointer (msg, &pointer));
    POINTERS_EQUAL(0x1a2b3c4d5, pointer);

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_char
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjChar)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char char_a[] = { OBJ_CHAR };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_char (NULL));

    /* buffer too small */
    msg->buffer = char_a;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_char (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid char: 'a' */
        msg->buffer = char_a;
        msg->size = sizeof (char_a);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_char (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_CHAR);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_CHAR, obj->type);
        LONGS_EQUAL('a', obj->value_char);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_integer
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjInteger)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char int_123456[] = { OBJ_INTEGER };
    unsigned char int_minus_123456[] = { OBJ_INTEGER_2 };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_integer (NULL));

    /* buffer too small */
    msg->buffer = int_123456;
    msg->size = sizeof (int_123456) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_integer (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid integer: 123456 */
        msg->buffer = int_123456;
        msg->size = sizeof (int_123456);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_integer (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_INTEGER);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->type);
        LONGS_EQUAL(123456, obj->value_integer);
        weechat_relay_parse_obj_free (obj);
    }

    /* valid integer: -123456 */
    msg->buffer = int_minus_123456;
    msg->size = sizeof (int_minus_123456);
    msg->position = 0;
    obj = weechat_relay_parse_obj_integer (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->type);
    LONGS_EQUAL(-123456, obj->value_integer);
    weechat_relay_parse_obj_free (obj);

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_long
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjLong)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char long_invalid[] = { OBJ_LONG_INVALID };
    unsigned char long_1234567890[] = { OBJ_LONG };
    unsigned char long_minus_1234567890[] = { OBJ_LONG_2 };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_long (NULL));

    /* buffer too small */
    msg->buffer = long_1234567890;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_long (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = long_1234567890;
    msg->size = sizeof (long_1234567890) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_long (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid long integer */
    msg->buffer = long_invalid;
    msg->size = sizeof (long_invalid);
    msg->position = 0;
    obj = weechat_relay_parse_obj_long (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid long integer: 1234567890 */
        msg->buffer = long_1234567890;
        msg->size = sizeof (long_1234567890);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_long (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_LONG);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_LONG, obj->type);
        LONGS_EQUAL(1234567890, obj->value_long);
        weechat_relay_parse_obj_free (obj);
    }

    /* valid integer: -1234567890 */
    msg->buffer = long_minus_1234567890;
    msg->size = sizeof (long_minus_1234567890);
    msg->position = 0;
    obj = weechat_relay_parse_obj_long (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_LONG, obj->type);
    LONGS_EQUAL(-1234567890, obj->value_long);
    weechat_relay_parse_obj_free (obj);

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_string
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjString)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char str_null[] = { OBJ_STRING_NULL };
    unsigned char str_empty[] = { OBJ_STRING_EMPTY };
    unsigned char str_abc[] = { OBJ_STRING };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_string (NULL));

    /* buffer too small */
    msg->buffer = str_empty;
    msg->size = sizeof (str_empty) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_string (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = str_abc;
    msg->size = sizeof (str_abc) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_string (msg);
    POINTERS_EQUAL(NULL, obj);

    /* valid string: NULL */
    msg->buffer = str_null;
    msg->size = sizeof (str_null);
    msg->position = 0;
    obj = weechat_relay_parse_obj_string (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->type);
    POINTERS_EQUAL(NULL, obj->value_string);
    weechat_relay_parse_obj_free (obj);

    /* valid string: "" (empty) */
    msg->buffer = str_empty;
    msg->size = sizeof (str_empty);
    msg->position = 0;
    obj = weechat_relay_parse_obj_string (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->type);
    STRCMP_EQUAL("", obj->value_string);
    weechat_relay_parse_obj_free (obj);

    for (i = 0; i < 2; i++)
    {
        /* valid string: "abc" */
        msg->buffer = str_abc;
        msg->size = sizeof (str_abc);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_string (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_STRING);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->type);
        STRCMP_EQUAL("abc", obj->value_string);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_buffer
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjBuffer)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char buffer_null1[] = { OBJ_BUFFER_NULL };
    unsigned char buffer_null2[] = { OBJ_BUFFER_NULL_2 };
    unsigned char buffer_abc[] = { OBJ_BUFFER };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_buffer (NULL));

    /* buffer too small */
    msg->buffer = buffer_null1;
    msg->size = sizeof (buffer_null1) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = buffer_abc;
    msg->size = sizeof (buffer_abc) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (msg);
    POINTERS_EQUAL(NULL, obj);

    /* valid buffer: NULL (length = -1) */
    msg->buffer = buffer_null1;
    msg->size = sizeof (buffer_null1);
    msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_BUFFER, obj->type);
    POINTERS_EQUAL(NULL, obj->value_buffer.buffer);
    LONGS_EQUAL(-1, obj->value_buffer.length);
    weechat_relay_parse_obj_free (obj);

    /* valid buffer: NULL (length = 0) */
    msg->buffer = buffer_null2;
    msg->size = sizeof (buffer_null2);
    msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_BUFFER, obj->type);
    POINTERS_EQUAL(NULL, obj->value_buffer.buffer);
    LONGS_EQUAL(0, obj->value_buffer.length);
    weechat_relay_parse_obj_free (obj);

    for (i = 0; i < 2; i++)
    {
        /* valid buffer: "abc" */
        msg->buffer = buffer_abc;
        msg->size = sizeof (buffer_abc);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_buffer (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_BUFFER);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_BUFFER, obj->type);
        MEMCMP_EQUAL(buffer_abc + 4, obj->value_buffer.buffer,
                     sizeof (buffer_abc) - 4);
        LONGS_EQUAL(sizeof (buffer_abc) - 4, obj->value_buffer.length);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_pointer
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjPointer)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char pointer_null[] = { OBJ_POINTER_NULL };
    unsigned char pointer_invalid[] = { OBJ_POINTER_INVALID };
    unsigned char pointer_1a2b3c4d5[] = { OBJ_POINTER };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_pointer (NULL));

    /* buffer too small */
    msg->buffer = pointer_null;
    msg->size = sizeof (pointer_null) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = pointer_1a2b3c4d5;
    msg->size = sizeof (pointer_1a2b3c4d5) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid pointer */
    msg->buffer = pointer_invalid;
    msg->size = sizeof (pointer_invalid);
    msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (msg);
    POINTERS_EQUAL(NULL, obj);

    /* valid pointer: NULL */
    msg->buffer = pointer_null;
    msg->size = sizeof (pointer_null);
    msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_POINTER, obj->type);
    POINTERS_EQUAL(NULL, obj->value_pointer);
    weechat_relay_parse_obj_free (obj);

    for (i = 0; i < 2; i++)
    {
        /* valid buffer: 0x1a2b3c4d5 */
        msg->buffer = pointer_1a2b3c4d5;
        msg->size = sizeof (pointer_1a2b3c4d5);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_pointer (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_POINTER);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_POINTER, obj->type);
        POINTERS_EQUAL(0x1a2b3c4d5, obj->value_pointer);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_time
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjTime)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char time_invalid[] = { OBJ_TIME_INVALID };
    unsigned char time_1321993456[] = { OBJ_TIME };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_time (NULL));

    /* buffer too small */
    msg->buffer = time_1321993456;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_time (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = time_1321993456;
    msg->size = sizeof (time_1321993456) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_time (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid time */
    msg->buffer = time_invalid;
    msg->size = sizeof (time_invalid);
    msg->position = 0;
    obj = weechat_relay_parse_obj_time (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid time: 1321993456 */
        msg->buffer = time_1321993456;
        msg->size = sizeof (time_1321993456);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_time (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_TIME);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_TIME, obj->type);
        LONGS_EQUAL(1321993456, obj->value_time);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_hashtable
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjHashtable)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char hashtable_invalid_count[] = { OBJ_HASHTABLE_INVALID_COUNT };
    unsigned char hashtable[] = { OBJ_HASHTABLE };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_hashtable (NULL));

    /* buffer too small */
    msg->buffer = hashtable;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hashtable;
    msg->size = 4;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hashtable;
    msg->size = 9;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hashtable;
    msg->size = 11;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hashtable;
    msg->size = sizeof (hashtable) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid hashtable: count = -1 */
    msg->buffer = hashtable_invalid_count;
    msg->size = sizeof (hashtable_invalid_count);
    msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid hashtable: {"abc": 1, "def": 2} */
        msg->buffer = hashtable;
        msg->size = sizeof (hashtable);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_hashtable (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_HASHTABLE);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_HASHTABLE, obj->type);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->value_hashtable.type_keys);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->value_hashtable.type_values);
        LONGS_EQUAL(2, obj->value_hashtable.count);
        CHECK(obj->value_hashtable.keys);
        CHECK(obj->value_hashtable.keys[0]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->value_hashtable.keys[0]->type);
        STRCMP_EQUAL("abc", obj->value_hashtable.keys[0]->value_string);
        CHECK(obj->value_hashtable.keys[1]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->value_hashtable.keys[1]->type);
        STRCMP_EQUAL("def", obj->value_hashtable.keys[1]->value_string);
        CHECK(obj->value_hashtable.values);
        CHECK(obj->value_hashtable.values[0]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->value_hashtable.values[0]->type);
        LONGS_EQUAL(1, obj->value_hashtable.values[0]->value_integer);
        CHECK(obj->value_hashtable.values[1]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->value_hashtable.values[1]->type);
        LONGS_EQUAL(2, obj->value_hashtable.values[1]->value_integer);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_hdata_split_hpath
 */

TEST(LibParse, HdataSplitHpath)
{
    char **hpaths;
    int num_hpaths;

    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath (NULL, NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath ("test", NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath (NULL, &hpaths, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath (NULL, NULL, &num_hpaths));
    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath ("test", &hpaths, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath ("test", NULL, &num_hpaths));
    LONGS_EQUAL(0, weechat_relay_parse_hdata_split_hpath (NULL, &hpaths, &num_hpaths));

    LONGS_EQUAL(1, weechat_relay_parse_hdata_split_hpath ("",
                                                          &hpaths, &num_hpaths));
    LONGS_EQUAL(1, num_hpaths);
    STRCMP_EQUAL("", hpaths[0]);
    free (hpaths[0]);
    free (hpaths);

    LONGS_EQUAL(1, weechat_relay_parse_hdata_split_hpath ("test",
                                                          &hpaths, &num_hpaths));
    LONGS_EQUAL(1, num_hpaths);
    STRCMP_EQUAL("test", hpaths[0]);
    free (hpaths[0]);
    free (hpaths);

    LONGS_EQUAL(1, weechat_relay_parse_hdata_split_hpath ("test/path2",
                                                          &hpaths, &num_hpaths));
    LONGS_EQUAL(2, num_hpaths);
    STRCMP_EQUAL("test", hpaths[0]);
    STRCMP_EQUAL("path2", hpaths[1]);
    free (hpaths[0]);
    free (hpaths[1]);
    free (hpaths);

    LONGS_EQUAL(1, weechat_relay_parse_hdata_split_hpath ("/test//path2/",
                                                          &hpaths, &num_hpaths));
    LONGS_EQUAL(5, num_hpaths);
    STRCMP_EQUAL("", hpaths[0]);
    STRCMP_EQUAL("test", hpaths[1]);
    STRCMP_EQUAL("", hpaths[2]);
    STRCMP_EQUAL("path2", hpaths[3]);
    STRCMP_EQUAL("", hpaths[4]);
    free (hpaths[0]);
    free (hpaths[1]);
    free (hpaths[2]);
    free (hpaths[3]);
    free (hpaths[4]);
    free (hpaths);

    LONGS_EQUAL(1, weechat_relay_parse_hdata_split_hpath ("//",
                                                          &hpaths, &num_hpaths));
    LONGS_EQUAL(3, num_hpaths);
    STRCMP_EQUAL("", hpaths[0]);
    STRCMP_EQUAL("", hpaths[1]);
    STRCMP_EQUAL("", hpaths[2]);
    free (hpaths[0]);
    free (hpaths[1]);
    free (hpaths[2]);
    free (hpaths);
}

/*
 * Tests functions:
 *   weechat_relay_parse_hdata_split_keys
 */

TEST(LibParse, HdataSplitKeys)
{
    char **keys_names;
    enum t_weechat_relay_obj_type *keys_types;
    int num_keys;

    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            NULL, NULL, NULL, NULL));
    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "test", &keys_names, &keys_types, NULL));
    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "test", &keys_names, NULL, &num_keys));
    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "test", NULL, &keys_types, &num_keys));
    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            NULL, &keys_names, &keys_types, &num_keys));

    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "", &keys_names, &keys_types, &num_keys));

    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "test", &keys_names, &keys_types, &num_keys));

    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "var:xxx", &keys_names, &keys_types, &num_keys));

    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "var1,var2:str", &keys_names, &keys_types, &num_keys));

    LONGS_EQUAL(
        0,
        weechat_relay_parse_hdata_split_keys (
            "var1:xxx,var2:str", &keys_names, &keys_types, &num_keys));

    LONGS_EQUAL(
        1,
        weechat_relay_parse_hdata_split_keys (
            "var:str", &keys_names, &keys_types, &num_keys));
    LONGS_EQUAL(1, num_keys);
    STRCMP_EQUAL("var", keys_names[0]);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, keys_types[0]);
    free (keys_names[0]);
    free (keys_names);
    free (keys_types);

    LONGS_EQUAL(
        1,
        weechat_relay_parse_hdata_split_keys (
            "var1:str,var2:int", &keys_names, &keys_types, &num_keys));
    LONGS_EQUAL(2, num_keys);
    STRCMP_EQUAL("var1", keys_names[0]);
    STRCMP_EQUAL("var2", keys_names[1]);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, keys_types[0]);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, keys_types[1]);
    free (keys_names[0]);
    free (keys_names[1]);
    free (keys_names);
    free (keys_types);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_hdata
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjHdata)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char hdata_invalid_count[] = { OBJ_HDATA_INVALID_COUNT };
    unsigned char hdata[] = { OBJ_HDATA };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_hdata (NULL));

    /* buffer too small */
    msg->buffer = hdata;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hdata;
    msg->size = 4;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hdata;
    msg->size = 9;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hdata;
    msg->size = 35;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = hdata;
    msg->size = sizeof (hdata) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid hdata: count = -1 */
    msg->buffer = hdata_invalid_count;
    msg->size = sizeof (hdata_invalid_count);
    msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid hdata */
        msg->buffer = hdata;
        msg->size = sizeof (hdata);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_hdata (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_HDATA);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_HDATA, obj->type);
        STRCMP_EQUAL("p1/p2", obj->value_hdata.hpath);
        LONGS_EQUAL(2, obj->value_hdata.num_hpaths);
        CHECK(obj->value_hdata.hpaths);
        STRCMP_EQUAL("p1", obj->value_hdata.hpaths[0]);
        STRCMP_EQUAL("p2", obj->value_hdata.hpaths[1]);
        CHECK(obj->value_hdata.keys);
        STRCMP_EQUAL("k1:str,k2:int,k3:chr", obj->value_hdata.keys);
        LONGS_EQUAL(3, obj->value_hdata.num_keys);
        CHECK(obj->value_hdata.keys_names);
        STRCMP_EQUAL("k1", obj->value_hdata.keys_names[0]);
        STRCMP_EQUAL("k2", obj->value_hdata.keys_names[1]);
        STRCMP_EQUAL("k3", obj->value_hdata.keys_names[2]);
        CHECK(obj->value_hdata.keys_types);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->value_hdata.keys_types[0]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->value_hdata.keys_types[1]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_CHAR, obj->value_hdata.keys_types[2]);
        LONGS_EQUAL(2, obj->value_hdata.count);
        /* obj 1 */
        POINTERS_EQUAL(0x123, obj->value_hdata.ppath[0][0]->value_pointer);
        POINTERS_EQUAL(0x456, obj->value_hdata.ppath[0][1]->value_pointer);
        STRCMP_EQUAL("ab", obj->value_hdata.values[0][0]->value_string);
        LONGS_EQUAL(5, obj->value_hdata.values[0][1]->value_integer);
        LONGS_EQUAL('F', obj->value_hdata.values[0][2]->value_char);
        /* obj 2 */
        POINTERS_EQUAL(0xabc, obj->value_hdata.ppath[1][0]->value_pointer);
        POINTERS_EQUAL(0xdef, obj->value_hdata.ppath[1][1]->value_pointer);
        STRCMP_EQUAL("xy", obj->value_hdata.values[1][0]->value_string);
        LONGS_EQUAL(9, obj->value_hdata.values[1][1]->value_integer);
        LONGS_EQUAL('X', obj->value_hdata.values[1][2]->value_char);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_info
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjInfo)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char info[] = { OBJ_INFO };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_info (NULL));

    /* buffer too small */
    msg->buffer = info;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_info (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = info;
    msg->size = 9;
    msg->position = 0;
    obj = weechat_relay_parse_obj_info (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid info: "abc" -> "def" */
        msg->buffer = info;
        msg->size = sizeof (info);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_info (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_INFO);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INFO, obj->type);
        STRCMP_EQUAL("abc", obj->value_info.name);
        STRCMP_EQUAL("def", obj->value_info.value);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_infolist
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjInfolist)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char infolist_invalid_count[] = { OBJ_INFOLIST_INVALID_COUNT };
    unsigned char infolist_invalid_item_count[] = {
        OBJ_INFOLIST_INVALID_ITEM_COUNT };
    unsigned char infolist[] = { OBJ_INFOLIST };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_infolist (NULL));

    /* buffer too small */
    msg->buffer = infolist;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = infolist;
    msg->size = 10;
    msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = infolist;
    msg->size = 14;
    msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = infolist;
    msg->size = 18;
    msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid infolist: count = -1 */
    msg->buffer = infolist_invalid_count;
    msg->size = sizeof (infolist_invalid_count);
    msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid infolist: item count = -1 */
    msg->buffer = infolist_invalid_item_count;
    msg->size = sizeof (infolist_invalid_item_count);
    msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid infolist */
        msg->buffer = infolist;
        msg->size = sizeof (infolist);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_infolist (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_INFOLIST);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INFOLIST, obj->type);
        STRCMP_EQUAL("test", obj->value_infolist.name);
        LONGS_EQUAL(2, obj->value_infolist.count);
        CHECK(obj->value_infolist.items);
        CHECK(obj->value_infolist.items[0]);
        LONGS_EQUAL(1, obj->value_infolist.items[0]->count);
        CHECK(obj->value_infolist.items[0]->variables);
        CHECK(obj->value_infolist.items[0]->variables[0]);
        STRCMP_EQUAL("abc", obj->value_infolist.items[0]->variables[0]->name);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER,
                    obj->value_infolist.items[0]->variables[0]->obj->type);
        LONGS_EQUAL(8,
                    obj->value_infolist.items[0]->variables[0]->obj->value_integer);
        CHECK(obj->value_infolist.items[1]);
        LONGS_EQUAL(2, obj->value_infolist.items[1]->count);
        CHECK(obj->value_infolist.items[1]->variables);
        CHECK(obj->value_infolist.items[1]->variables[0]);
        STRCMP_EQUAL("def", obj->value_infolist.items[1]->variables[0]->name);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER,
                    obj->value_infolist.items[1]->variables[0]->obj->type);
        LONGS_EQUAL(4,
                    obj->value_infolist.items[1]->variables[0]->obj->value_integer);
        CHECK(obj->value_infolist.items[1]->variables[1]);
        STRCMP_EQUAL("ghi", obj->value_infolist.items[1]->variables[1]->name);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING,
                    obj->value_infolist.items[1]->variables[1]->obj->type);
        POINTERS_EQUAL(NULL,
                       obj->value_infolist.items[1]->variables[1]->obj->value_string);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_array
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjArray)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char array_invalid_count[] = { OBJ_ARRAY_INVALID_COUNT };
    unsigned char array[] = { OBJ_ARRAY };
    int i;

    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_array (NULL));

    /* buffer too small */
    msg->buffer = array;
    msg->size = 0;
    msg->position = 0;
    obj = weechat_relay_parse_obj_array (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = array;
    msg->size = 4;
    msg->position = 0;
    obj = weechat_relay_parse_obj_array (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = array;
    msg->size = 9;
    msg->position = 0;
    obj = weechat_relay_parse_obj_array (msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    msg->buffer = array;
    msg->size = sizeof (array) - 1;
    msg->position = 0;
    obj = weechat_relay_parse_obj_array (msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid array: count = -1 */
    msg->buffer = array_invalid_count;
    msg->size = sizeof (array_invalid_count);
    msg->position = 0;
    obj = weechat_relay_parse_obj_array (msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid array: ["abc" "def"] */
        msg->buffer = array;
        msg->size = sizeof (array);
        msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_array (msg);
        else
            obj = weechat_relay_parse_read_object (msg, WEECHAT_RELAY_OBJ_TYPE_ARRAY);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_ARRAY, obj->type);
        LONGS_EQUAL(2, obj->value_array.count);
        CHECK(obj->value_array.values);
        CHECK(obj->value_array.values[0]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->value_array.values[0]->type);
        STRCMP_EQUAL("abc", obj->value_array.values[0]->value_string);
        CHECK(obj->value_array.values[1]);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->value_array.values[1]->type);
        STRCMP_EQUAL("def", obj->value_array.values[1]->value_string);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_decompress
 */

TEST(LibParse, Decompress)
{
    unsigned char data_invalid[] = { 0x01, 0x02, 0x03, 0x04 };
    unsigned char data_ok[] = {
        0x78, 0x9C, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xE4, 0x8C, 0x22,
        0xC7, 0xCC, 0xBC, 0x12, 0x06, 0xC6, 0x47, 0x0E, 0x40, 0xEA,
        0xFF, 0x3F, 0xD9, 0x03, 0x39, 0xF9, 0x79, 0x5C, 0x86, 0x46,
        0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x06, 0x40, 0x1E,
        0xB7, 0x2E, 0x82, 0x5B, 0x5C, 0x52, 0xC4, 0xC0, 0xC0, 0xC0,
        0x91, 0xA8, 0x00, 0x64, 0x64, 0xE6, 0xA5, 0x43, 0xB8, 0x0C,
        0x40, 0x0A, 0x68, 0xD4, 0xFF, 0xA4, 0xD2, 0x34, 0x20, 0x87,
        0x0D, 0x48, 0xA5, 0xA5, 0x16, 0x01, 0x49, 0x90, 0x58, 0x41,
        0x49, 0x11, 0x07, 0x48, 0x7B, 0x62, 0x52, 0x72, 0x0A, 0x90,
        0xCD, 0x68, 0x50, 0x92, 0x99, 0xCB, 0x65, 0x68, 0x6C, 0x64,
        0x68, 0x69, 0x09, 0x32, 0x33, 0xB1, 0xA8, 0x08, 0x62, 0x06,
        0x13, 0x10, 0x33, 0x03, 0x15, 0x81, 0x98, 0x29, 0xA9, 0x40,
        0x61, 0x90, 0xA3, 0x80, 0x42, 0x40, 0x5C, 0xCD, 0xC0, 0xC0,
        0x78, 0x02, 0xC8, 0x14, 0x05, 0x00, 0x3B, 0xD4, 0x38, 0x34,
    };
    unsigned char data_decompressed[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0x63, 0x68, 0x72, 0x41, 0x69, 0x6E,
        0x74, 0x00, 0x01, 0xE2, 0x40, 0x69, 0x6E, 0x74, 0xFF, 0xFE,
        0x1D, 0xC0, 0x6C, 0x6F, 0x6E, 0x0A, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x6C, 0x6F, 0x6E, 0x0B,
        0x2D, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x30, 0x73, 0x74, 0x72, 0x00, 0x00, 0x00, 0x08, 0x61, 0x20,
        0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x73, 0x74, 0x72, 0x00,
        0x00, 0x00, 0x00, 0x73, 0x74, 0x72, 0xFF, 0xFF, 0xFF, 0xFF,
        0x62, 0x75, 0x66, 0x00, 0x00, 0x00, 0x06, 0x62, 0x75, 0x66,
        0x66, 0x65, 0x72, 0x62, 0x75, 0x66, 0xFF, 0xFF, 0xFF, 0xFF,
        0x70, 0x74, 0x72, 0x08, 0x31, 0x32, 0x33, 0x34, 0x61, 0x62,
        0x63, 0x64, 0x70, 0x74, 0x72, 0x01, 0x30, 0x74, 0x69, 0x6D,
        0x0A, 0x31, 0x33, 0x32, 0x31, 0x39, 0x39, 0x33, 0x34, 0x35,
        0x36, 0x61, 0x72, 0x72, 0x73, 0x74, 0x72, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x03, 0x61, 0x62, 0x63, 0x00, 0x00,
        0x00, 0x02, 0x64, 0x65, 0x61, 0x72, 0x72, 0x69, 0x6E, 0x74,
        0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x7B, 0x00, 0x00,
        0x01, 0xC8, 0x00, 0x00, 0x03, 0x15,
    };
    void *data;
    size_t size_decompressed;

    /* invalid buffer/length */
    data = weechat_relay_parse_decompress (data_invalid, 0, 0, NULL);
    POINTERS_EQUAL(NULL, data);
    data = weechat_relay_parse_decompress (data_invalid, 1, 0, NULL);
    POINTERS_EQUAL(NULL, data);
    data = weechat_relay_parse_decompress (data_invalid, 1, 1, NULL);
    POINTERS_EQUAL(NULL, data);
    data = weechat_relay_parse_decompress (data_invalid, 0, 1, &size_decompressed);
    POINTERS_EQUAL(NULL, data);
    data = weechat_relay_parse_decompress (data_invalid, 1, 0, &size_decompressed);
    POINTERS_EQUAL(NULL, data);

    /* try to decompress invalid data */
    size_decompressed = 1000000;
    data = weechat_relay_parse_decompress (data_invalid,
                                           sizeof (data_invalid),
                                           10 * sizeof (data_invalid),
                                           &size_decompressed);
    POINTERS_EQUAL(NULL, data);
    LONGS_EQUAL(0, size_decompressed);

    /* decompress valid data */
    size_decompressed = 1000000;
    data = weechat_relay_parse_decompress (data_ok,
                                           sizeof (data_ok),
                                           10 * sizeof (data_ok),
                                           &size_decompressed);
    CHECK(data);
    LONGS_EQUAL(176, size_decompressed);
    MEMCMP_EQUAL(data_decompressed, data, 176);
    free (data);

    /*
     * decompress valid data, force to start with a very small output size
     * (1 byte), so multiple pass will be needed to adjust the output buffer
     */
    size_decompressed = 1000000;
    data = weechat_relay_parse_decompress (data_ok,
                                           sizeof (data_ok),
                                           1,
                                           &size_decompressed);
    CHECK(data);
    LONGS_EQUAL(176, size_decompressed);
    MEMCMP_EQUAL(data_decompressed, data, 176);
    free (data);
}

/*
 * Tests functions:
 *   weechat_relay_parse_msg_alloc
 *   weechat_relay_parse_msg_free
 */

TEST(LibParse, MsgAllocFree)
{
    struct t_weechat_relay_msg *msg;
    unsigned char msg_too_small[] = {
        0x00, 0x00, 0x00, 0x05, 0x00,
    };
    unsigned char msg_string[] = { MESSAGE_STRING };

    POINTERS_EQUAL(NULL, weechat_relay_parse_msg_alloc (NULL, 0));
    POINTERS_EQUAL(NULL, weechat_relay_parse_msg_alloc (msg_too_small,
                                                        sizeof (msg_too_small)));

    /* message too small */
    msg = weechat_relay_parse_msg_alloc (msg_too_small,
                                         sizeof (msg_too_small));
    POINTERS_EQUAL(NULL, msg);

    /* message with one string */
    msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));
    CHECK(msg);
    MEMCMP_EQUAL(msg_string, msg->message, sizeof (msg_string));
    LONGS_EQUAL(sizeof (msg_string), msg->length);
    LONGS_EQUAL(sizeof (msg_string) - 5, msg->length_data);
    POINTERS_EQUAL(NULL, msg->data_decompressed);
    LONGS_EQUAL(sizeof (msg_string) - 5, msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_OFF, msg->compression);
    STRCMP_EQUAL("id", msg->id);
    LONGS_EQUAL(0, msg->num_objects);
    POINTERS_EQUAL(NULL, msg->objects);
    MEMCMP_EQUAL(msg_string + 5, msg->buffer, sizeof (msg_string) - 5);
    LONGS_EQUAL(sizeof (msg_string) - 5, msg->size);
    LONGS_EQUAL(6, msg->position);
    weechat_relay_parse_msg_free (msg);

    weechat_relay_parse_msg_free (NULL);
}

/*
 * Tests functions:
 *   weechat_relay_parse_message
 */

TEST(LibParse, Message)
{
    unsigned char message_invalid_size[] = { MESSAGE_INVALID_SIZE };
    unsigned char message_invalid_id[] = { MESSAGE_INVALID_ID };
    unsigned char message_invalid_compressed_data[] = {
        MESSAGE_INVALID_COMPRESSED_DATA };
    unsigned char message_not_compressed[] = { MESSAGE_NOT_COMPRESSED };
    unsigned char message_compressed_zlib[] = { MESSAGE_COMPRESSED_ZLIB };
    struct t_weechat_relay_msg *msg;

    POINTERS_EQUAL(NULL, weechat_relay_parse_message (NULL, 0));

    /* invalid size */
    msg = weechat_relay_parse_message (message_invalid_size,
                                       sizeof (message_invalid_size));
    POINTERS_EQUAL(NULL, msg);

    /* invalid size */
    msg = weechat_relay_parse_message (message_invalid_id,
                                       sizeof (message_invalid_id));
    POINTERS_EQUAL(NULL, msg);

    /* invalid compressed data */
    msg = weechat_relay_parse_message (
        message_invalid_compressed_data,
        sizeof (message_invalid_compressed_data));
    POINTERS_EQUAL(NULL, msg);

    /* message not compressed */
    msg = weechat_relay_parse_message (message_not_compressed, 181);
    MEMCMP_EQUAL(message_not_compressed, msg->message, 181);
    LONGS_EQUAL(181, msg->length);
    LONGS_EQUAL(176, msg->length_data);
    POINTERS_EQUAL(NULL, msg->data_decompressed);
    LONGS_EQUAL(176, msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_OFF, msg->compression);
    POINTERS_EQUAL(NULL, msg->id);
    LONGS_EQUAL(15, msg->num_objects);
    weechat_relay_parse_msg_free (msg);

    /* compressed message (zlib) */
    msg = weechat_relay_parse_message (message_compressed_zlib, 145);
    MEMCMP_EQUAL(message_compressed_zlib, msg->message, 145);
    LONGS_EQUAL(145, msg->length);
    LONGS_EQUAL(140, msg->length_data);
    MEMCMP_EQUAL(message_not_compressed + 5, msg->data_decompressed, 176);
    LONGS_EQUAL(176, msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_ZLIB, msg->compression);
    POINTERS_EQUAL(NULL, msg->id);
    LONGS_EQUAL(15, msg->num_objects);
    weechat_relay_parse_msg_free (msg);
}
