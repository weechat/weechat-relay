/*
 * message.c - build and send binary messages from WeeChat to client
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <gnutls/gnutls.h>
#include <zlib.h>
#include <zstd.h>

#include "weechat-relay.h"


/*
 * Builds a new message (for sending to client).
 *
 * Returns pointer to new message, NULL if error.
 */

struct t_weechat_relay_msg *
weechat_relay_msg_new (const char *id)
{
    struct t_weechat_relay_msg *new_msg;

    new_msg = malloc (sizeof (*new_msg));
    if (!new_msg)
        return NULL;

    new_msg->id = (id) ? strdup (id) : NULL;
    new_msg->data = malloc (WEECHAT_RELAY_MSG_INITIAL_ALLOC);
    if (!new_msg->data)
    {
        free (new_msg);
        return NULL;
    }
    new_msg->data_alloc = WEECHAT_RELAY_MSG_INITIAL_ALLOC;
    new_msg->data_size = 0;

    /* add size and compression flag (they will be set later) */
    weechat_relay_msg_add_integer (new_msg, 0);
    weechat_relay_msg_add_char (new_msg, 0);

    /* add id */
    weechat_relay_msg_add_string (new_msg, id);

    return new_msg;
}

/*
 * Sets some bytes in a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_set_bytes (struct t_weechat_relay_msg *msg,
                             size_t position, const void *buffer, size_t size)
{
    if (!msg || !msg->data || !buffer || (size == 0)
        || (position + size) > msg->data_size)
    {
        return 0;
    }

    memcpy (msg->data + position, buffer, size);

    return 1;
}

/*
 * Adds some bytes to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_bytes (struct t_weechat_relay_msg *msg,
                             const void *buffer, size_t size)
{
    char *ptr;
    size_t old_data_alloc;
    uint32_t size32;

    if (!msg || !msg->data || !buffer || (size == 0))
        return 0;

    old_data_alloc = msg->data_alloc;
    while (msg->data_size + size > msg->data_alloc)
    {
        msg->data_alloc *= 2;
    }

    if (msg->data_alloc != old_data_alloc)
    {
        ptr = realloc (msg->data, msg->data_alloc);
        if (!ptr)
        {
            free (msg->data);
            msg->data = NULL;
            msg->data_alloc = 0;
            msg->data_size = 0;
            return 0;
        }
        msg->data = ptr;
    }

    memcpy (msg->data + msg->data_size, buffer, size);
    msg->data_size += size;

    size32 = htonl ((uint32_t)msg->data_size);
    weechat_relay_msg_set_bytes (msg, 0, &size32, 4);

    return 1;
}

/*
 * Adds type to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_type (struct t_weechat_relay_msg *msg,
                            enum t_weechat_relay_obj_type obj_type)
{
    return weechat_relay_msg_add_bytes (
        msg,
        weechat_relay_obj_types_str[obj_type],
        strlen (weechat_relay_obj_types_str[obj_type]));
}

/*
 * Adds a char to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_char (struct t_weechat_relay_msg *msg, char c)
{
    return weechat_relay_msg_add_bytes (msg, &c, 1);
}

/*
 * Adds an integer to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_integer (struct t_weechat_relay_msg *msg, int value)
{
    uint32_t value32;

    value32 = htonl ((uint32_t)value);

    return weechat_relay_msg_add_bytes (msg, &value32, 4);
}

/*
 * Adds a long integer to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_long (struct t_weechat_relay_msg *msg, long value)
{
    char str_long[128];
    unsigned char length;

    snprintf (str_long, sizeof (str_long), "%ld", value);
    length = strlen (str_long);
    if (!weechat_relay_msg_add_bytes (msg, &length, 1))
        return 0;
    return weechat_relay_msg_add_bytes (msg, str_long, length);
}

/*
 * Adds length + string to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_string (struct t_weechat_relay_msg *msg,
                              const char *string)
{
    size_t length;

    if (string)
    {
        length = strlen (string);
        if (!weechat_relay_msg_add_integer (msg, length))
            return 0;
        if (length > 0)
            return weechat_relay_msg_add_bytes (msg, string, length);
        else
            return 1;
    }
    else
    {
        return weechat_relay_msg_add_integer (msg, -1);
    }
}

/*
 * Adds buffer (length + data) to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_buffer (struct t_weechat_relay_msg *msg,
                              struct t_weechat_relay_obj_buffer *buffer)
{
    if (!buffer)
        return 0;

    if (buffer->buffer)
    {
        if (!weechat_relay_msg_add_integer (msg, buffer->length))
            return 0;
        return (buffer->length > 0) ?
            weechat_relay_msg_add_bytes (msg, buffer->buffer, buffer->length) : 1;
    }
    else
    {
        return weechat_relay_msg_add_integer (msg, -1);
    }
}

/*
 * Adds a pointer to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_pointer (struct t_weechat_relay_msg *msg,
                               const void *pointer)
{
    char str_pointer[128];
    unsigned char length;

    snprintf (str_pointer, sizeof (str_pointer),
              "%lx", (unsigned long)pointer);
    length = strlen (str_pointer);
    if (!weechat_relay_msg_add_bytes (msg, &length, 1))
        return 0;
    return weechat_relay_msg_add_bytes (msg, str_pointer, length);
}

/*
 * Adds a time to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_time (struct t_weechat_relay_msg *msg, time_t time)
{
    char str_time[128];
    unsigned char length;

    snprintf (str_time, sizeof (str_time), "%lld", (long long)time);
    length = strlen (str_time);
    if (!weechat_relay_msg_add_bytes (msg, &length, 1))
        return 0;
    return weechat_relay_msg_add_bytes (msg, str_time, length);
}

/*
 * Adds a hashtable to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_hashtable (struct t_weechat_relay_msg *msg,
                                 struct t_weechat_relay_obj_hashtable *hashtable)
{
    int i;

    if (!hashtable)
        return 0;

    if ((hashtable->type_keys != WEECHAT_RELAY_OBJ_TYPE_INTEGER)
        && (hashtable->type_keys != WEECHAT_RELAY_OBJ_TYPE_STRING)
        && (hashtable->type_keys != WEECHAT_RELAY_OBJ_TYPE_POINTER)
        && (hashtable->type_keys != WEECHAT_RELAY_OBJ_TYPE_BUFFER)
        && (hashtable->type_keys != WEECHAT_RELAY_OBJ_TYPE_TIME))
    {
        return 0;
    }

    if ((hashtable->type_values != WEECHAT_RELAY_OBJ_TYPE_INTEGER)
        && (hashtable->type_values != WEECHAT_RELAY_OBJ_TYPE_STRING)
        && (hashtable->type_values != WEECHAT_RELAY_OBJ_TYPE_POINTER)
        && (hashtable->type_values != WEECHAT_RELAY_OBJ_TYPE_BUFFER)
        && (hashtable->type_values != WEECHAT_RELAY_OBJ_TYPE_TIME))
    {
        return 0;
    }

    if (!weechat_relay_msg_add_type (msg, hashtable->type_keys))
        return 0;
    if (!weechat_relay_msg_add_type (msg, hashtable->type_values))
        return 0;
    if (!weechat_relay_msg_add_integer (msg, hashtable->count))
        return 0;

    for (i = 0; i < hashtable->count; i++)
    {
        weechat_relay_msg_add_object_value (msg, hashtable->keys[i]);
        weechat_relay_msg_add_object_value (msg, hashtable->values[i]);
    }

    return 1;
}

/*
 * Adds a hdata to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_hdata (struct t_weechat_relay_msg *msg,
                             struct t_weechat_relay_obj_hdata *hdata)
{
    int i, j;

    if (!hdata || !hdata->hpath || (hdata->num_hpaths <= 0) || !hdata->hpaths
        || !hdata->keys || (hdata->num_keys <= 0) || !hdata->keys_names
        || !hdata->keys_types || (hdata->count <= 0) || !hdata->ppath
        || !hdata->values)
    {
        return 0;
    }

    if (!weechat_relay_msg_add_string (msg, hdata->hpath))
        return 0;
    if (!weechat_relay_msg_add_string (msg, hdata->keys))
        return 0;
    if (!weechat_relay_msg_add_integer (msg, hdata->count))
        return 0;

    for (i = 0; i < hdata->count; i++)
    {
        for (j = 0; j < hdata->num_hpaths; j++)
        {
            if (!weechat_relay_msg_add_pointer (msg, hdata->ppath[i][j]->value_pointer))
                return 0;
        }
        for (j = 0; j < hdata->num_keys; j++)
        {
            if (!weechat_relay_msg_add_object_value (msg, hdata->values[i][j]))
                return 0;
        }
    }

    return 1;
}

/*
 * Adds an info to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_info (struct t_weechat_relay_msg *msg,
                            struct t_weechat_relay_obj_info *info)
{
    if (!info)
        return 0;

    if (!weechat_relay_msg_add_string (msg, info->name))
        return 0;
    if (!weechat_relay_msg_add_string (msg, info->value))
        return 0;

    return 1;
}

/*
 * Adds an infolist to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_infolist (struct t_weechat_relay_msg *msg,
                                struct t_weechat_relay_obj_infolist *infolist)
{
    int i, j;
    struct t_weechat_relay_obj_infolist_item *ptr_item;
    struct t_weechat_relay_obj_infolist_var *ptr_var;

    if (!infolist || (infolist->count < 0))
        return 0;

    if (!weechat_relay_msg_add_string (msg, infolist->name))
        return 0;
    if (!weechat_relay_msg_add_integer (msg, infolist->count))
        return 0;

    /* loop on items */
    for (i = 0; i < infolist->count; i++)
    {
        ptr_item = infolist->items[i];
        if (!weechat_relay_msg_add_integer (msg, ptr_item->count))
            return 0;
        /* loop on variables in item */
        for (j = 0; j < ptr_item->count; j++)
        {
            ptr_var = ptr_item->variables[j];
            if (!weechat_relay_msg_add_string (msg, ptr_var->name))
                return 0;
            if (!weechat_relay_msg_add_object (msg, ptr_var->value))
                return 0;
        }
    }

    return 1;
}

/*
 * Adds an array to a message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_array (struct t_weechat_relay_msg *msg,
                             struct t_weechat_relay_obj_array *array)
{
    int i;

    if (!array || (array->count < 0))
        return 0;

    if (!weechat_relay_msg_add_type (msg, array->type))
        return 0;
    if (!weechat_relay_msg_add_integer (msg, array->count))
        return 0;

    for (i = 0; i < array->count; i++)
    {
        if (!weechat_relay_msg_add_object_value (msg, array->values[i]))
            return 0;
    }

    return 1;
}

/*
 * Adds the value of an object to a message (without the object type), which
 * can be any type supported in enum t_weechat_relay_obj_type.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_object_value (struct t_weechat_relay_msg *msg,
                                    struct t_weechat_relay_obj *obj)
{
    switch (obj->type)
    {
        case WEECHAT_RELAY_OBJ_TYPE_CHAR:
            if (!weechat_relay_msg_add_char (msg, obj->value_char))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INTEGER:
            if (!weechat_relay_msg_add_integer (msg, obj->value_integer))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_LONG:
            if (!weechat_relay_msg_add_long (msg, obj->value_long))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_STRING:
            if (!weechat_relay_msg_add_string (msg, obj->value_string))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_BUFFER:
            if (!weechat_relay_msg_add_buffer (msg, &obj->value_buffer))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_POINTER:
            if (!weechat_relay_msg_add_pointer (msg, (void *)obj->value_pointer))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_TIME:
            if (!weechat_relay_msg_add_time (msg, obj->value_time))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HASHTABLE:
            if (!weechat_relay_msg_add_hashtable (msg, &obj->value_hashtable))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HDATA:
            if (!weechat_relay_msg_add_hdata (msg, &obj->value_hdata))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFO:
            if (!weechat_relay_msg_add_info (msg, &obj->value_info))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFOLIST:
            if (!weechat_relay_msg_add_infolist (msg, &obj->value_infolist))
                return 0;
            break;
        case WEECHAT_RELAY_OBJ_TYPE_ARRAY:
            if (!weechat_relay_msg_add_array (msg, &obj->value_array))
                return 0;
            break;
        case WEECHAT_RELAY_NUM_OBJ_TYPES:
            break;

    }

    return 1;
}

/*
 * Adds an object to a message, which can be any type supported in
 * enum t_weechat_relay_obj_type.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_msg_add_object (struct t_weechat_relay_msg *msg,
                              struct t_weechat_relay_obj *obj)
{
    if (!weechat_relay_msg_add_type (msg, obj->type))
        return 0;

    if (!weechat_relay_msg_add_object_value (msg, obj))
        return 0;

    return 1;
}

/*
 * Compresses a message with zlib.
 *
 * The variable "size" is set with the size of buffer returned (in bytes).
 *
 * Returns a pointer to the compressed message, NULL if error.
 */

void *
weechat_relay_msg_compress_zlib (struct t_weechat_relay_msg *msg,
                                 int compression_level,
                                 size_t *size)
{
    int rc;
    Bytef *dest;
    uLongf dest_size;
    uint32_t size32;

    if (!msg || !size)
        return NULL;

    *size = 0;

    dest_size = compressBound (msg->data_size - 5);
    dest = malloc (dest_size + 5);
    if (!dest)
        return NULL;

    rc = compress2 (dest + 5,
                    &dest_size,
                    (Bytef *)(msg->data + 5),
                    msg->data_size - 5,
                    compression_level);

    if (rc != Z_OK)
    {
        free (dest);
        return NULL;
    }

    *size = dest_size + 5;

    /* set size and compression flag */
    size32 = htonl ((uint32_t)(*size));
    memcpy (dest, &size32, 4);
    dest[4] = WEECHAT_RELAY_COMPRESSION_ZLIB;

    return dest;
}

/*
 * Compresses a message with zstd.
 *
 * The variable "size" is set with the size of buffer returned (in bytes).
 *
 * Returns a pointer to the compressed message, NULL if error.
 */

void *
weechat_relay_msg_compress_zstd (struct t_weechat_relay_msg *msg,
                                 int compression_level,
                                 size_t *size)
{
    Bytef *dest;
    size_t dest_size, comp_size;
    uint32_t size32;

    if (!msg || !size)
        return NULL;

    *size = 0;

    dest_size = ZSTD_compressBound (msg->data_size - 5);
    dest = malloc (dest_size + 5);
    if (!dest)
        return NULL;

    comp_size = ZSTD_compress(
        dest + 5,
        dest_size,
        (void *)(msg->data + 5),
        msg->data_size - 5,
        compression_level);

    if (comp_size == 0)
    {
        free (dest);
        return NULL;
    }

    *size = comp_size + 5;

    /* set size and compression flag */
    size32 = htonl ((uint32_t)(*size));
    memcpy (dest, &size32, 4);
    dest[4] = WEECHAT_RELAY_COMPRESSION_ZSTD;

    return dest;
}

/*
 * Frees a message.
 */

void
weechat_relay_msg_free (struct t_weechat_relay_msg *msg)
{
    if (!msg)
        return;

    if (msg->id)
        free (msg->id);
    if (msg->data)
        free (msg->data);

    free (msg);
}
