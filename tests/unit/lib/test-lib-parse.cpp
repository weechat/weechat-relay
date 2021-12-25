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
    struct t_weechat_relay_parsed_msg *parsed_msg, void *output, size_t count);
extern int weechat_relay_parse_read_type (
    struct t_weechat_relay_parsed_msg *parsed_msg,
    enum t_weechat_relay_obj_type *type);
extern int weechat_relay_parse_read_integer (
    struct t_weechat_relay_parsed_msg *parsed_msg, int *value);
extern int weechat_relay_parse_read_string (
    struct t_weechat_relay_parsed_msg *parsed_msg, char **string);
extern int weechat_relay_parse_read_buffer (
    struct t_weechat_relay_parsed_msg *parsed_msg, void **buffer, int *length);
extern int weechat_relay_parse_read_pointer (
    struct t_weechat_relay_parsed_msg *parsed_msg, const void **pointer);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_char (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_integer (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_long (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_string (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_buffer (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_pointer (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_time (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_hashtable (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern int weechat_relay_parse_hdata_split_hpath (
    const char *hpath, char ***hpaths, int *num_hpaths);
extern int weechat_relay_parse_hdata_split_keys (
    const char *keys, char ***keys_names,
    enum t_weechat_relay_obj_type **keys_types, int *num_keys);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_hdata (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_info (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_infolist (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_array (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_read_object (
    struct t_weechat_relay_parsed_msg *parsed_msg,
    enum t_weechat_relay_obj_type type);
extern void *weechat_relay_parse_decompress_zlib (const void *data,
                                                  size_t size,
                                                  size_t initial_output_size,
                                                  size_t *size_decompressed);
extern void *weechat_relay_parse_decompress_zstd (const void *data,
                                                  size_t size,
                                                  size_t initial_output_size,
                                                  size_t *size_decompressed);
extern struct t_weechat_relay_parsed_msg *weechat_relay_parse_msg_alloc (
    const void *buffer, size_t size);
extern void weechat_relay_parse_msg_free (
    struct t_weechat_relay_parsed_msg *parsed_msg);
}

#define CHECK_OBJS_FAKE_MSG                                     \
    /* obj #1 */                                                \
    ptr_obj = parsed_msg->objects[0];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_CHAR, ptr_obj->type);    \
    LONGS_EQUAL('A', ptr_obj->value_char);                      \
    /* obj #2 */                                                \
    ptr_obj = parsed_msg->objects[1];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_CHAR, ptr_obj->type);    \
    LONGS_EQUAL('Z', ptr_obj->value_char);                      \
    /* obj #3 */                                                \
    ptr_obj = parsed_msg->objects[2];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, ptr_obj->type); \
    LONGS_EQUAL(123456, ptr_obj->value_integer);                \
    /* obj #4 */                                                \
    ptr_obj = parsed_msg->objects[3];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, ptr_obj->type); \
    LONGS_EQUAL(-987654, ptr_obj->value_integer);               \
    /* obj #5 */                                                \
    ptr_obj = parsed_msg->objects[4];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_LONG, ptr_obj->type);    \
    LONGS_EQUAL(1234567890L, ptr_obj->value_long);              \
    /* obj #6 */                                                \
    ptr_obj = parsed_msg->objects[5];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_LONG, ptr_obj->type);    \
    LONGS_EQUAL(-9876543210L, ptr_obj->value_long);             \
    /* obj #7 */                                                \
    ptr_obj = parsed_msg->objects[6];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, ptr_obj->type);  \
    STRCMP_EQUAL(LOREM_IPSUM_4096, ptr_obj->value_string);      \
    /* obj #8 */                                                \
    ptr_obj = parsed_msg->objects[7];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_POINTER, ptr_obj->type); \
    LONGS_EQUAL((void *)0x1234abcd, ptr_obj->value_pointer);    \
    /* obj #9 */                                                \
    ptr_obj = parsed_msg->objects[8];                           \
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_TIME, ptr_obj->type);    \
    LONGS_EQUAL(1640091762, ptr_obj->value_time);

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
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    char type[4], output[4];
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (NULL, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (NULL, output, 0));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (NULL, output, 3));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (parsed_msg, NULL, 0));
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (parsed_msg, NULL, 3));

    /* buffer too small */
    parsed_msg->buffer = type;
    parsed_msg->size = 2;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_bytes (parsed_msg, output, 3));

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        snprintf (type, sizeof (type), "%s", weechat_relay_obj_types_str[i]);
        parsed_msg->buffer = type;
        parsed_msg->size = 3;
        parsed_msg->position = 0;
        LONGS_EQUAL(1, weechat_relay_parse_read_bytes (parsed_msg, output, 3));
        MEMCMP_EQUAL(weechat_relay_obj_types_str[i], output, 3);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_type
 */

TEST(LibParse, ReadType)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    enum t_weechat_relay_obj_type type;
    unsigned char msg_one_str[] = { MESSAGE_STRING };
    char str_type[4];
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_one_str, sizeof (msg_one_str));

    LONGS_EQUAL(0, weechat_relay_parse_read_type (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_type (NULL, &type));
    LONGS_EQUAL(0, weechat_relay_parse_read_type (parsed_msg, NULL));

    /* buffer too small */
    parsed_msg->buffer = str_type;
    parsed_msg->size = 2;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_type (parsed_msg, &type));

    /* invalid type */
    snprintf (str_type, sizeof (str_type), "XXX");
    parsed_msg->buffer = str_type;
    parsed_msg->size = 3;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_type (parsed_msg, &type));

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        type = (i == WEECHAT_RELAY_OBJ_TYPE_CHAR) ?
            WEECHAT_RELAY_OBJ_TYPE_STRING : WEECHAT_RELAY_OBJ_TYPE_CHAR;
        snprintf (str_type, sizeof (str_type),
                  "%s", weechat_relay_obj_types_str[i]);
        parsed_msg->buffer = str_type;
        parsed_msg->size = 3;
        parsed_msg->position = 0;
        LONGS_EQUAL(1, weechat_relay_parse_read_type (parsed_msg, &type));
        LONGS_EQUAL(i, type);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_integer
 */

TEST(LibParse, ReadInteger)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char int_123456[] = { 0x00, 0x01, 0xE2, 0x40 };
    unsigned char int_minus_123456[] = { 0xFF, 0xFE, 0x1D, 0xC0 };
    int value;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_integer (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_integer (NULL, &value));
    LONGS_EQUAL(0, weechat_relay_parse_read_integer (parsed_msg, NULL));

    /* buffer too small */
    parsed_msg->buffer = int_123456;
    parsed_msg->size = sizeof (int_123456) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_integer (parsed_msg, &value));

    /* valid integer: 123456 */
    parsed_msg->buffer = int_123456;
    parsed_msg->size = sizeof (int_123456);
    parsed_msg->position = 0;
    value = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_integer (parsed_msg, &value));
    LONGS_EQUAL(123456, value);

    /* valid integer: -123456 */
    parsed_msg->buffer = int_minus_123456;
    parsed_msg->size = sizeof (int_minus_123456);
    parsed_msg->position = 0;
    value = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_integer (parsed_msg, &value));
    LONGS_EQUAL(-123456, value);

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_string
 */

TEST(LibParse, ReadString)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char str_null[] = { 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char str_empty[] = { 0x00, 0x00, 0x00, 0x00 };
    unsigned char str_abc[] = { 0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c' };
    char *str;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_string (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_string (NULL, &str));
    LONGS_EQUAL(0, weechat_relay_parse_read_string (parsed_msg, NULL));

    /* buffer too small */
    parsed_msg->buffer = str_empty;
    parsed_msg->size = sizeof (str_empty) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_string (parsed_msg, &str));

    /* buffer too small */
    parsed_msg->buffer = str_abc;
    parsed_msg->size = sizeof (str_abc) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_string (parsed_msg, &str));

    /* valid string: NULL */
    parsed_msg->buffer = str_null;
    parsed_msg->size = sizeof (str_null);
    parsed_msg->position = 0;
    str = (char *)0x123;
    LONGS_EQUAL(1, weechat_relay_parse_read_string (parsed_msg, &str));
    POINTERS_EQUAL(NULL, str);

    /* valid string: "" (empty) */
    parsed_msg->buffer = str_empty;
    parsed_msg->size = sizeof (str_empty);
    parsed_msg->position = 0;
    str = NULL;
    LONGS_EQUAL(1, weechat_relay_parse_read_string (parsed_msg, &str));
    STRCMP_EQUAL("", str);
    free (str);

    /* valid string: "abc" */
    parsed_msg->buffer = str_abc;
    parsed_msg->size = sizeof (str_abc);
    parsed_msg->position = 0;
    str = NULL;
    LONGS_EQUAL(1, weechat_relay_parse_read_string (parsed_msg, &str));
    STRCMP_EQUAL("abc", str);
    free (str);

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_buffer
 */

TEST(LibParse, ReadBuffer)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char buffer_null1[] = { 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char buffer_null2[] = { 0x00, 0x00, 0x00, 0x00 };
    unsigned char buffer_abc[] = { 0x00, 0x00, 0x00, 0x03, 0x61, 0x62, 0x63 };
    void *buffer;
    int length;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL, NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (parsed_msg, NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL,  &buffer, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL, NULL, &length));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (parsed_msg, &buffer, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (parsed_msg, NULL, &length));
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (NULL, &buffer, &length));

    /* buffer too small */
    parsed_msg->buffer = buffer_null1;
    parsed_msg->size = sizeof (buffer_null1) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (parsed_msg, &buffer, &length));

    /* buffer too small */
    parsed_msg->buffer = buffer_abc;
    parsed_msg->size = sizeof (buffer_abc) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_buffer (parsed_msg, &buffer, &length));

    /* valid buffer: NULL (length = -1) */
    parsed_msg->buffer = buffer_null1;
    parsed_msg->size = sizeof (buffer_null1);
    parsed_msg->position = 0;
    buffer = (char *)0x123;
    length = 1;
    LONGS_EQUAL(1, weechat_relay_parse_read_buffer (parsed_msg, &buffer, &length));
    POINTERS_EQUAL(NULL, buffer);
    LONGS_EQUAL(-1, length);

    /* valid buffer: NULL (length = 0) */
    parsed_msg->buffer = buffer_null2;
    parsed_msg->size = sizeof (buffer_null2);
    parsed_msg->position = 0;
    buffer = (char *)0x123;
    length = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_buffer (parsed_msg, &buffer, &length));
    POINTERS_EQUAL(NULL, buffer);
    LONGS_EQUAL(0, length);

    /* valid buffer: "abc" */
    parsed_msg->buffer = buffer_abc;
    parsed_msg->size = sizeof (buffer_abc);
    parsed_msg->position = 0;
    buffer = NULL;
    length = -1;
    LONGS_EQUAL(1, weechat_relay_parse_read_buffer (parsed_msg, &buffer, &length));
    MEMCMP_EQUAL(buffer_abc + 4, buffer, sizeof (buffer_abc) - 4);
    free (buffer);

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_read_pointer
 */

TEST(LibParse, ReadPointer)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char pointer_null[] = { 0x01, 0x30 };
    unsigned char pointer_invalid[] = { 0x01, 0x02 };
    unsigned char pointer_1a2b3c4d5[] = { 0x09, 0x31, 0x61, 0x32, 0x62,
                                          0x33, 0x63, 0x34, 0x64, 0x35 };
    const void *pointer;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (NULL, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (parsed_msg, NULL));
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (NULL,  &pointer));

    /* buffer too small */
    parsed_msg->buffer = pointer_null;
    parsed_msg->size = sizeof (pointer_null) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (parsed_msg, &pointer));

    /* buffer too small */
    parsed_msg->buffer = pointer_1a2b3c4d5;
    parsed_msg->size = sizeof (pointer_1a2b3c4d5) - 1;
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (parsed_msg, &pointer));

    /* invalid pointer */
    parsed_msg->buffer = pointer_invalid;
    parsed_msg->size = sizeof (pointer_invalid);
    parsed_msg->position = 0;
    LONGS_EQUAL(0, weechat_relay_parse_read_pointer (parsed_msg, &pointer));

    /* valid pointer: NULL */
    parsed_msg->buffer = pointer_null;
    parsed_msg->size = sizeof (pointer_null);
    parsed_msg->position = 0;
    pointer = (char *)0x123;
    LONGS_EQUAL(1, weechat_relay_parse_read_pointer (parsed_msg, &pointer));
    POINTERS_EQUAL(NULL, pointer);

    /* valid buffer: 0x1a2b3c4d5 */
    parsed_msg->buffer = pointer_1a2b3c4d5;
    parsed_msg->size = sizeof (pointer_1a2b3c4d5);
    parsed_msg->position = 0;
    pointer = NULL;
    LONGS_EQUAL(1, weechat_relay_parse_read_pointer (parsed_msg, &pointer));
    POINTERS_EQUAL(0x1a2b3c4d5, pointer);

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_char
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjChar)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char char_a[] = { OBJ_CHAR };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_char (NULL));

    /* buffer too small */
    parsed_msg->buffer = char_a;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_char (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid char: 'a' */
        parsed_msg->buffer = char_a;
        parsed_msg->size = sizeof (char_a);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_char (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_CHAR);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_CHAR, obj->type);
        LONGS_EQUAL('a', obj->value_char);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_integer
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjInteger)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char int_123456[] = { OBJ_INTEGER };
    unsigned char int_minus_123456[] = { OBJ_INTEGER_2 };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_integer (NULL));

    /* buffer too small */
    parsed_msg->buffer = int_123456;
    parsed_msg->size = sizeof (int_123456) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_integer (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid integer: 123456 */
        parsed_msg->buffer = int_123456;
        parsed_msg->size = sizeof (int_123456);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_integer (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_INTEGER);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->type);
        LONGS_EQUAL(123456, obj->value_integer);
        weechat_relay_parse_obj_free (obj);
    }

    /* valid integer: -123456 */
    parsed_msg->buffer = int_minus_123456;
    parsed_msg->size = sizeof (int_minus_123456);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_integer (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INTEGER, obj->type);
    LONGS_EQUAL(-123456, obj->value_integer);
    weechat_relay_parse_obj_free (obj);

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_long
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjLong)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char long_invalid[] = { OBJ_LONG_INVALID };
    unsigned char long_1234567890[] = { OBJ_LONG };
    unsigned char long_minus_1234567890[] = { OBJ_LONG_2 };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_long (NULL));

    /* buffer too small */
    parsed_msg->buffer = long_1234567890;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_long (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = long_1234567890;
    parsed_msg->size = sizeof (long_1234567890) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_long (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid long integer */
    parsed_msg->buffer = long_invalid;
    parsed_msg->size = sizeof (long_invalid);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_long (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid long integer: 1234567890 */
        parsed_msg->buffer = long_1234567890;
        parsed_msg->size = sizeof (long_1234567890);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_long (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_LONG);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_LONG, obj->type);
        LONGS_EQUAL(1234567890, obj->value_long);
        weechat_relay_parse_obj_free (obj);
    }

    /* valid integer: -1234567890 */
    parsed_msg->buffer = long_minus_1234567890;
    parsed_msg->size = sizeof (long_minus_1234567890);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_long (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_LONG, obj->type);
    LONGS_EQUAL(-1234567890, obj->value_long);
    weechat_relay_parse_obj_free (obj);

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_string
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjString)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char str_null[] = { OBJ_STRING_NULL };
    unsigned char str_empty[] = { OBJ_STRING_EMPTY };
    unsigned char str_abc[] = { OBJ_STRING };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_string (NULL));

    /* buffer too small */
    parsed_msg->buffer = str_empty;
    parsed_msg->size = sizeof (str_empty) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_string (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = str_abc;
    parsed_msg->size = sizeof (str_abc) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_string (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* valid string: NULL */
    parsed_msg->buffer = str_null;
    parsed_msg->size = sizeof (str_null);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_string (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->type);
    POINTERS_EQUAL(NULL, obj->value_string);
    weechat_relay_parse_obj_free (obj);

    /* valid string: "" (empty) */
    parsed_msg->buffer = str_empty;
    parsed_msg->size = sizeof (str_empty);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_string (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->type);
    STRCMP_EQUAL("", obj->value_string);
    weechat_relay_parse_obj_free (obj);

    for (i = 0; i < 2; i++)
    {
        /* valid string: "abc" */
        parsed_msg->buffer = str_abc;
        parsed_msg->size = sizeof (str_abc);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_string (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_STRING);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_STRING, obj->type);
        STRCMP_EQUAL("abc", obj->value_string);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_buffer
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjBuffer)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char buffer_null1[] = { OBJ_BUFFER_NULL };
    unsigned char buffer_null2[] = { OBJ_BUFFER_NULL_2 };
    unsigned char buffer_abc[] = { OBJ_BUFFER };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_buffer (NULL));

    /* buffer too small */
    parsed_msg->buffer = buffer_null1;
    parsed_msg->size = sizeof (buffer_null1) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = buffer_abc;
    parsed_msg->size = sizeof (buffer_abc) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* valid buffer: NULL (length = -1) */
    parsed_msg->buffer = buffer_null1;
    parsed_msg->size = sizeof (buffer_null1);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_BUFFER, obj->type);
    POINTERS_EQUAL(NULL, obj->value_buffer.buffer);
    LONGS_EQUAL(-1, obj->value_buffer.length);
    weechat_relay_parse_obj_free (obj);

    /* valid buffer: NULL (length = 0) */
    parsed_msg->buffer = buffer_null2;
    parsed_msg->size = sizeof (buffer_null2);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_buffer (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_BUFFER, obj->type);
    POINTERS_EQUAL(NULL, obj->value_buffer.buffer);
    LONGS_EQUAL(0, obj->value_buffer.length);
    weechat_relay_parse_obj_free (obj);

    for (i = 0; i < 2; i++)
    {
        /* valid buffer: "abc" */
        parsed_msg->buffer = buffer_abc;
        parsed_msg->size = sizeof (buffer_abc);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_buffer (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_BUFFER);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_BUFFER, obj->type);
        MEMCMP_EQUAL(buffer_abc + 4, obj->value_buffer.buffer,
                     sizeof (buffer_abc) - 4);
        LONGS_EQUAL(sizeof (buffer_abc) - 4, obj->value_buffer.length);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_pointer
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjPointer)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char pointer_null[] = { OBJ_POINTER_NULL };
    unsigned char pointer_invalid[] = { OBJ_POINTER_INVALID };
    unsigned char pointer_1a2b3c4d5[] = { OBJ_POINTER };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_pointer (NULL));

    /* buffer too small */
    parsed_msg->buffer = pointer_null;
    parsed_msg->size = sizeof (pointer_null) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = pointer_1a2b3c4d5;
    parsed_msg->size = sizeof (pointer_1a2b3c4d5) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid pointer */
    parsed_msg->buffer = pointer_invalid;
    parsed_msg->size = sizeof (pointer_invalid);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* valid pointer: NULL */
    parsed_msg->buffer = pointer_null;
    parsed_msg->size = sizeof (pointer_null);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_pointer (parsed_msg);
    CHECK(obj);
    LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_POINTER, obj->type);
    POINTERS_EQUAL(NULL, obj->value_pointer);
    weechat_relay_parse_obj_free (obj);

    for (i = 0; i < 2; i++)
    {
        /* valid buffer: 0x1a2b3c4d5 */
        parsed_msg->buffer = pointer_1a2b3c4d5;
        parsed_msg->size = sizeof (pointer_1a2b3c4d5);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_pointer (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_POINTER);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_POINTER, obj->type);
        POINTERS_EQUAL(0x1a2b3c4d5, obj->value_pointer);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_time
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjTime)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char time_invalid[] = { OBJ_TIME_INVALID };
    unsigned char time_1321993456[] = { OBJ_TIME };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_time (NULL));

    /* buffer too small */
    parsed_msg->buffer = time_1321993456;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_time (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = time_1321993456;
    parsed_msg->size = sizeof (time_1321993456) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_time (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid time */
    parsed_msg->buffer = time_invalid;
    parsed_msg->size = sizeof (time_invalid);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_time (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid time: 1321993456 */
        parsed_msg->buffer = time_1321993456;
        parsed_msg->size = sizeof (time_1321993456);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_time (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_TIME);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_TIME, obj->type);
        LONGS_EQUAL(1321993456, obj->value_time);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_hashtable
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjHashtable)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char hashtable_invalid_count[] = { OBJ_HASHTABLE_INVALID_COUNT };
    unsigned char hashtable[] = { OBJ_HASHTABLE };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_hashtable (NULL));

    /* buffer too small */
    parsed_msg->buffer = hashtable;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hashtable;
    parsed_msg->size = 4;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hashtable;
    parsed_msg->size = 9;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hashtable;
    parsed_msg->size = 11;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hashtable;
    parsed_msg->size = sizeof (hashtable) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid hashtable: count = -1 */
    parsed_msg->buffer = hashtable_invalid_count;
    parsed_msg->size = sizeof (hashtable_invalid_count);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hashtable (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid hashtable: {"abc": 1, "def": 2} */
        parsed_msg->buffer = hashtable;
        parsed_msg->size = sizeof (hashtable);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_hashtable (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_HASHTABLE);
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

    weechat_relay_parse_msg_free (parsed_msg);
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
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char hdata_invalid_count[] = { OBJ_HDATA_INVALID_COUNT };
    unsigned char hdata[] = { OBJ_HDATA };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_hdata (NULL));

    /* buffer too small */
    parsed_msg->buffer = hdata;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hdata;
    parsed_msg->size = 4;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hdata;
    parsed_msg->size = 9;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hdata;
    parsed_msg->size = 35;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = hdata;
    parsed_msg->size = sizeof (hdata) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid hdata: count = -1 */
    parsed_msg->buffer = hdata_invalid_count;
    parsed_msg->size = sizeof (hdata_invalid_count);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_hdata (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid hdata */
        parsed_msg->buffer = hdata;
        parsed_msg->size = sizeof (hdata);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_hdata (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_HDATA);
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

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_info
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjInfo)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char info[] = { OBJ_INFO };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_info (NULL));

    /* buffer too small */
    parsed_msg->buffer = info;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_info (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = info;
    parsed_msg->size = 9;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_info (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid info: "abc" -> "def" */
        parsed_msg->buffer = info;
        parsed_msg->size = sizeof (info);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_info (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_INFO);
        CHECK(obj);
        LONGS_EQUAL(WEECHAT_RELAY_OBJ_TYPE_INFO, obj->type);
        STRCMP_EQUAL("abc", obj->value_info.name);
        STRCMP_EQUAL("def", obj->value_info.value);
        weechat_relay_parse_obj_free (obj);
    }

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_infolist
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjInfolist)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char infolist_invalid_count[] = { OBJ_INFOLIST_INVALID_COUNT };
    unsigned char infolist_invalid_item_count[] = {
        OBJ_INFOLIST_INVALID_ITEM_COUNT };
    unsigned char infolist[] = { OBJ_INFOLIST };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_infolist (NULL));

    /* buffer too small */
    parsed_msg->buffer = infolist;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = infolist;
    parsed_msg->size = 10;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = infolist;
    parsed_msg->size = 14;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = infolist;
    parsed_msg->size = 18;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid infolist: count = -1 */
    parsed_msg->buffer = infolist_invalid_count;
    parsed_msg->size = sizeof (infolist_invalid_count);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid infolist: item count = -1 */
    parsed_msg->buffer = infolist_invalid_item_count;
    parsed_msg->size = sizeof (infolist_invalid_item_count);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_infolist (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid infolist */
        parsed_msg->buffer = infolist;
        parsed_msg->size = sizeof (infolist);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_infolist (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_INFOLIST);
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

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_obj_array
 *   weechat_relay_parse_read_object
 */

TEST(LibParse, ObjArray)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *obj;
    unsigned char msg_string[] = { MESSAGE_STRING };
    unsigned char array_invalid_count[] = { OBJ_ARRAY_INVALID_COUNT };
    unsigned char array[] = { OBJ_ARRAY };
    int i;

    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));

    POINTERS_EQUAL(NULL, weechat_relay_parse_obj_array (NULL));

    /* buffer too small */
    parsed_msg->buffer = array;
    parsed_msg->size = 0;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_array (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = array;
    parsed_msg->size = 4;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_array (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = array;
    parsed_msg->size = 9;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_array (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* buffer too small */
    parsed_msg->buffer = array;
    parsed_msg->size = sizeof (array) - 1;
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_array (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    /* invalid array: count = -1 */
    parsed_msg->buffer = array_invalid_count;
    parsed_msg->size = sizeof (array_invalid_count);
    parsed_msg->position = 0;
    obj = weechat_relay_parse_obj_array (parsed_msg);
    POINTERS_EQUAL(NULL, obj);

    for (i = 0; i < 2; i++)
    {
        /* valid array: ["abc" "def"] */
        parsed_msg->buffer = array;
        parsed_msg->size = sizeof (array);
        parsed_msg->position = 0;
        if (i == 0)
            obj = weechat_relay_parse_obj_array (parsed_msg);
        else
            obj = weechat_relay_parse_read_object (parsed_msg, WEECHAT_RELAY_OBJ_TYPE_ARRAY);
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

    weechat_relay_parse_msg_free (parsed_msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_decompress_zlib
 */

TEST(LibParse, DecompressZlib)
{
    unsigned char data_invalid[] = { 0x01, 0x02, 0x03, 0x04 };
    struct t_weechat_relay_msg *msg;
    void *msg_comp, *data_decomp;
    size_t size_comp, size_decomp;

    /* invalid buffer/length */
    data_decomp = weechat_relay_parse_decompress_zlib (
        data_invalid, 0, 0, NULL);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zlib (
        data_invalid, 1, 0, NULL);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zlib (
        data_invalid, 1, 1, NULL);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zlib (
        data_invalid, 0, 1, &size_decomp);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zlib (
        data_invalid, 1, 0, &size_decomp);
    POINTERS_EQUAL(NULL, data_decomp);

    /* try to decompress invalid data */
    size_decomp = 1000000;
    data_decomp = weechat_relay_parse_decompress_zlib (
        data_invalid,
        sizeof (data_invalid),
        10 * sizeof (data_invalid),
        &size_decomp);
    POINTERS_EQUAL(NULL, data_decomp);
    LONGS_EQUAL(0, size_decomp);

    /* create a message and compress it for the following tests */
    MESSAGE_BUILD_FAKE(msg);
    msg_comp = weechat_relay_msg_compress_zlib (msg, 5, &size_comp);

    /* decompress valid data */
    size_decomp = 1000000;
    data_decomp = weechat_relay_parse_decompress_zlib (
        (void *)((char *)msg_comp + 5),
        size_comp - 5,
        10 * size_comp,
        &size_decomp);
    CHECK(data_decomp);
    LONGS_EQUAL(4181, size_decomp);
    free (data_decomp);

    /*
     * decompress valid data, force to start with a very small output size
     * (1 byte), so multiple pass will be needed to adjust the output buffer
     */
    size_decomp = 1000000;
    data_decomp = weechat_relay_parse_decompress_zlib (
        (void *)((char *)msg_comp + 5),
        size_comp - 5,
        1,
        &size_decomp);
    CHECK(data_decomp);
    LONGS_EQUAL(4181, size_decomp);
    free (data_decomp);

    free (msg_comp);
    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_decompress_zstd
 */

TEST(LibParse, DecompressZstd)
{
    unsigned char data_invalid[] = { 0x01, 0x02, 0x03, 0x04 };
    struct t_weechat_relay_msg *msg;
    void *msg_comp, *data_decomp;
    size_t size_comp, size_decomp;

    /* invalid buffer/length */
    data_decomp = weechat_relay_parse_decompress_zstd (
        data_invalid, 0, 0, NULL);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zstd (
        data_invalid, 1, 0, NULL);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zstd (
        data_invalid, 1, 1, NULL);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zstd (
        data_invalid, 0, 1, &size_decomp);
    POINTERS_EQUAL(NULL, data_decomp);
    data_decomp = weechat_relay_parse_decompress_zstd (
        data_invalid, 1, 0, &size_decomp);
    POINTERS_EQUAL(NULL, data_decomp);

    /* try to decompress invalid data */
    size_decomp = 1000000;
    data_decomp = weechat_relay_parse_decompress_zstd (
        data_invalid,
        sizeof (data_invalid),
        10 * sizeof (data_invalid),
        &size_decomp);
    POINTERS_EQUAL(NULL, data_decomp);
    LONGS_EQUAL(0, size_decomp);

    /* create a message and compress it for the following tests */
    MESSAGE_BUILD_FAKE(msg);
    msg_comp = weechat_relay_msg_compress_zstd (msg, 10, &size_comp);

    /* decompress valid data */
    size_decomp = 1000000;
    data_decomp = weechat_relay_parse_decompress_zstd (
        (void *)((char *)msg_comp + 5),
        size_comp - 5,
        10 * size_comp,
        &size_decomp);
    CHECK(data_decomp);
    LONGS_EQUAL(4181, size_decomp);
    free (data_decomp);

    /*
     * decompress valid data, force to start with a very small output size
     * (1 byte), so multiple pass will be needed to adjust the output buffer
     */
    size_decomp = 1000000;
    data_decomp = weechat_relay_parse_decompress_zstd (
        (void *)((char *)msg_comp + 5),
        size_comp - 5,
        1,
        &size_decomp);
    CHECK(data_decomp);
    LONGS_EQUAL(4181, size_decomp);
    free (data_decomp);

    free (msg_comp);
    weechat_relay_msg_free (msg);
}

/*
 * Tests functions:
 *   weechat_relay_parse_msg_alloc
 *   weechat_relay_parse_msg_free
 */

TEST(LibParse, MsgAllocFree)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    unsigned char msg_too_small[] = {
        0x00, 0x00, 0x00, 0x05, 0x00,
    };
    unsigned char msg_string[] = { MESSAGE_STRING };

    POINTERS_EQUAL(NULL, weechat_relay_parse_msg_alloc (NULL, 0));
    POINTERS_EQUAL(NULL, weechat_relay_parse_msg_alloc (msg_too_small,
                                                        sizeof (msg_too_small)));

    /* message too small */
    parsed_msg = weechat_relay_parse_msg_alloc (msg_too_small,
                                          sizeof (msg_too_small));
    POINTERS_EQUAL(NULL, parsed_msg);

    /* message with one string */
    parsed_msg = weechat_relay_parse_msg_alloc (msg_string, sizeof (msg_string));
    CHECK(parsed_msg);
    MEMCMP_EQUAL(msg_string, parsed_msg->message, sizeof (msg_string));
    LONGS_EQUAL(sizeof (msg_string), parsed_msg->length);
    LONGS_EQUAL(sizeof (msg_string) - 5, parsed_msg->length_data);
    POINTERS_EQUAL(NULL, parsed_msg->data_decompressed);
    LONGS_EQUAL(sizeof (msg_string) - 5, parsed_msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_OFF, parsed_msg->compression);
    STRCMP_EQUAL("id", parsed_msg->id);
    LONGS_EQUAL(0, parsed_msg->num_objects);
    POINTERS_EQUAL(NULL, parsed_msg->objects);
    MEMCMP_EQUAL(msg_string + 5, parsed_msg->buffer, sizeof (msg_string) - 5);
    LONGS_EQUAL(sizeof (msg_string) - 5, parsed_msg->size);
    LONGS_EQUAL(6, parsed_msg->position);
    weechat_relay_parse_msg_free (parsed_msg);

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
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_parsed_msg *parsed_msg;
    struct t_weechat_relay_obj *ptr_obj;
    void *msg_comp;
    size_t size_comp;

    POINTERS_EQUAL(NULL, weechat_relay_parse_message (NULL, 0));

    /* invalid size */
    parsed_msg = weechat_relay_parse_message (message_invalid_size,
                                              sizeof (message_invalid_size));
    POINTERS_EQUAL(NULL, parsed_msg);

    /* invalid size */
    parsed_msg = weechat_relay_parse_message (message_invalid_id,
                                              sizeof (message_invalid_id));
    POINTERS_EQUAL(NULL, parsed_msg);

    /* invalid compressed data */
    parsed_msg = weechat_relay_parse_message (
        message_invalid_compressed_data,
        sizeof (message_invalid_compressed_data));
    POINTERS_EQUAL(NULL, parsed_msg);

    /* create a message for the following tests */
    MESSAGE_BUILD_FAKE(msg);

    /* message not compressed */
    parsed_msg = weechat_relay_parse_message (msg->data, msg->data_size);
    CHECK(parsed_msg);
    MEMCMP_EQUAL(msg->data, parsed_msg->message, msg->data_size);
    LONGS_EQUAL(4186, parsed_msg->length);
    LONGS_EQUAL(4181, parsed_msg->length_data);
    POINTERS_EQUAL(NULL, parsed_msg->data_decompressed);
    LONGS_EQUAL(4181, parsed_msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_OFF, parsed_msg->compression);
    STRCMP_EQUAL("test", parsed_msg->id);
    LONGS_EQUAL(9, parsed_msg->num_objects);
    CHECK_OBJS_FAKE_MSG;
    weechat_relay_parse_msg_free (parsed_msg);

    /* compressed message (zlib) */
    msg_comp = weechat_relay_msg_compress_zlib (msg, 5, &size_comp);
    parsed_msg = weechat_relay_parse_message (msg_comp, size_comp);
    CHECK(parsed_msg);
    CHECK(parsed_msg->length > 0);
    CHECK(parsed_msg->length < 4186);
    LONGS_EQUAL(parsed_msg->length - 5, parsed_msg->length_data);
    LONGS_EQUAL(4181, parsed_msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_ZLIB, parsed_msg->compression);
    STRCMP_EQUAL("test", parsed_msg->id);
    LONGS_EQUAL(9, parsed_msg->num_objects);
    CHECK_OBJS_FAKE_MSG;
    weechat_relay_parse_msg_free (parsed_msg);
    free (msg_comp);

    /* compressed message (zstd) */
    msg_comp = weechat_relay_msg_compress_zstd (msg, 5, &size_comp);
    parsed_msg = weechat_relay_parse_message (msg_comp, size_comp);
    CHECK(parsed_msg);
    CHECK(parsed_msg->length > 0);
    CHECK(parsed_msg->length < 4186);
    LONGS_EQUAL(parsed_msg->length - 5, parsed_msg->length_data);
    LONGS_EQUAL(4181, parsed_msg->length_data_decompressed);
    LONGS_EQUAL(WEECHAT_RELAY_COMPRESSION_ZSTD, parsed_msg->compression);
    STRCMP_EQUAL("test", parsed_msg->id);
    LONGS_EQUAL(9, parsed_msg->num_objects);
    CHECK_OBJS_FAKE_MSG;
    weechat_relay_parse_msg_free (parsed_msg);
    free (msg_comp);

    weechat_relay_msg_free (msg);
}
