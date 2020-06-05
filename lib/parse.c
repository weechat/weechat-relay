/*
 * parse.c - parse binary messages sent by WeeChat
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#include <zlib.h>

#include "weechat-relay.h"


const char *weechat_relay_obj_types_str[WEECHAT_RELAY_NUM_OBJ_TYPES] = {
    "chr", "int", "lon", "str", "buf", "ptr", "tim", "htb",
    "hda", "inf", "inl", "arr",
};


struct t_weechat_relay_obj *weechat_relay_parse_read_object (
    struct t_weechat_relay_msg *msg, enum t_weechat_relay_obj_type type);


/*
 * Searches type.
 *
 * Returns index of type found (enum t_weechat_relay_obj_type),
 * -1 if not found.
 */

int
weechat_relay_parse_search_type (const char *type)
{
    int i;

    if (!type)
        return -1;

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        if (strcmp (type, weechat_relay_obj_types_str[i]) == 0)
            return i;
    }

    return -1;
}

/*
 * Allocates an object structure.
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_alloc (enum t_weechat_relay_obj_type type)
{
    struct t_weechat_relay_obj *obj;

    obj = calloc (1, sizeof (*obj));
    if (!obj)
        return NULL;

    obj->type = type;

    return obj;
}

/*
 * Frees an object.
 */

void
weechat_relay_parse_obj_free (struct t_weechat_relay_obj *obj)
{
    int i, j;

    if (!obj)
        return;

    switch (obj->type)
    {
        case WEECHAT_RELAY_OBJ_TYPE_CHAR:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INTEGER:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_LONG:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_STRING:
            if (obj->value_string)
                free (obj->value_string);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_BUFFER:
            if (obj->value_buffer.buffer)
                free (obj->value_buffer.buffer);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_POINTER:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_TIME:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HASHTABLE:
            for (i = 0; i < obj->value_hashtable.count; i++)
            {
                if (obj->value_hashtable.keys)
                    weechat_relay_parse_obj_free (obj->value_hashtable.keys[i]);
                if (obj->value_hashtable.values)
                    weechat_relay_parse_obj_free (obj->value_hashtable.values[i]);
            }
            if (obj->value_hashtable.keys)
                free (obj->value_hashtable.keys);
            if (obj->value_hashtable.values)
                free (obj->value_hashtable.values);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HDATA:
            if (obj->value_hdata.hpath)
                free (obj->value_hdata.hpath);
            if (obj->value_hdata.keys)
                free (obj->value_hdata.keys);
            if (obj->value_hdata.hpaths)
            {
                for (i = 0; i < obj->value_hdata.num_hpaths; i++)
                {
                    if (obj->value_hdata.hpaths[i])
                        free (obj->value_hdata.hpaths[i]);
                }
                free (obj->value_hdata.hpaths);
            }
            if (obj->value_hdata.keys_names)
            {
                for (i = 0; i < obj->value_hdata.num_keys; i++)
                {
                    if (obj->value_hdata.keys_names[i])
                        free (obj->value_hdata.keys_names[i]);
                }
                free (obj->value_hdata.keys_names);
            }
            if (obj->value_hdata.keys_types)
                free (obj->value_hdata.keys_types);
            for (i = 0; i < obj->value_hdata.count; i++)
            {
                if (obj->value_hdata.ppath && obj->value_hdata.ppath[i])
                {
                    for (j = 0; j < obj->value_hdata.num_hpaths; j++)
                    {
                        weechat_relay_parse_obj_free (
                            obj->value_hdata.ppath[i][j]);
                    }
                    free (obj->value_hdata.ppath[i]);
                }
                if (obj->value_hdata.values && obj->value_hdata.values[i])
                {
                    for (j = 0; j < obj->value_hdata.num_keys; j++)
                    {
                        weechat_relay_parse_obj_free (
                            obj->value_hdata.values[i][j]);
                    }
                    free (obj->value_hdata.values[i]);
                }
            }
            if (obj->value_hdata.ppath)
                free (obj->value_hdata.ppath);
            if (obj->value_hdata.values)
                free (obj->value_hdata.values);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFO:
            if (obj->value_info.name)
                free (obj->value_info.name);
            if (obj->value_info.value)
                free (obj->value_info.value);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFOLIST:
            if (obj->value_infolist.name)
                free (obj->value_infolist.name);
            if (obj->value_infolist.items)
            {
                for (i = 0; i < obj->value_infolist.count; i++)
                {
                    if (obj->value_infolist.items[i])
                    {
                        if (obj->value_infolist.items[i]->variables)
                        {
                            for (j = 0; j < obj->value_infolist.items[i]->count; j++)
                            {
                                if (obj->value_infolist.items[i]->variables[j])
                                {
                                    if (obj->value_infolist.items[i]->variables[j]->name)
                                        free (obj->value_infolist.items[i]->variables[j]->name);
                                    weechat_relay_parse_obj_free (obj->value_infolist.items[i]->variables[j]->obj);
                                    free (obj->value_infolist.items[i]->variables[j]);
                                }
                            }
                            free (obj->value_infolist.items[i]->variables);
                        }
                        free (obj->value_infolist.items[i]);
                    }
                }
                free (obj->value_infolist.items);
            }
            break;
        case WEECHAT_RELAY_OBJ_TYPE_ARRAY:
            if (obj->value_array.values)
            {
                for (i = 0; i < obj->value_array.count; i++)
                {
                    if (obj->value_array.values[i])
                        weechat_relay_parse_obj_free (obj->value_array.values[i]);
                }
                free (obj->value_array.values);
            }
            break;
        case WEECHAT_RELAY_NUM_OBJ_TYPES:
            break;
    }

    free (obj);
}

/*
 * Reads bytes in message.
 *
 * Returns:
 *   1: OK
 *   0: error (not enough bytes remaining in buffer)
 */

int
weechat_relay_parse_read_bytes (struct t_weechat_relay_msg *msg,
                                void *output, size_t count)
{
    if (!msg || !output)
        return 0;

    if (count == 0)
        return 1;

    if (msg->position + count > msg->size)
        return 0;

    if (count > 0)
    {
        memcpy (output, msg->buffer + msg->position, count);
        msg->position += count;
    }

    return 1;
}

/*
 * Reads type in message (3 chars).
 *
 * Returns:
 *   1: OK
 *   0: error: unknown type or not enough bytes remaining in buffer
 */

int
weechat_relay_parse_read_type (struct t_weechat_relay_msg *msg,
                               enum t_weechat_relay_obj_type *type)
{
    char str_type[4];
    int type_found;

    if (!msg || !type)
        return 0;

    if (!weechat_relay_parse_read_bytes (msg, str_type, 3))
        return 0;

    str_type[3] = 0;

    type_found = weechat_relay_parse_search_type (str_type);
    if (type_found < 0)
        return 0;

    *type = (enum t_weechat_relay_obj_type)type_found;

    return 1;
}

/*
 * Reads an integer in message (4 bytes).
 *
 * Returns:
 *   1: OK
 *   0: error (not enough bytes remaining in buffer)
 */

int
weechat_relay_parse_read_integer (struct t_weechat_relay_msg *msg,
                                  int *value)
{
    uint32_t value32;

    if (!msg || !value)
        return 0;

    if (!weechat_relay_parse_read_bytes (msg, &value32, 4))
        return 0;

    *value = (int) ntohl (value32);

    return 1;
}

/*
 * Reads string in message (4 bytes + content).
 *
 * Returns:
 *   1: OK
 *   0: error (not enough bytes remaining in buffer)
 */

int
weechat_relay_parse_read_string (struct t_weechat_relay_msg *msg,
                                 char **string)
{
    int length;

    if (!msg || !string)
        return 0;

    *string = NULL;

    if (!weechat_relay_parse_read_integer (msg, &length))
        goto error;
    if (length < 0)
        return 1;

    *string = malloc (length + 1);
    if (!*string)
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, *string, length))
        goto error;

    (*string)[length] = '\0';

    return 1;

error:
    if (*string)
    {
        free (*string);
        *string = NULL;
    }
    return 0;
}

/*
 * Reads buffer in message (4 bytes + content).
 *
 * Returns:
 *   1: OK
 *   0: error (not enough bytes remaining in buffer)
 */

int
weechat_relay_parse_read_buffer (struct t_weechat_relay_msg *msg,
                                 void **buffer, int *length)
{
    if (!msg || !buffer || !length)
        return 0;

    *buffer = NULL;
    *length = 0;

    if (!weechat_relay_parse_read_integer (msg, length))
        goto error;
    if (*length <= 0)
        return 1;

    *buffer = malloc (*length);
    if (!*buffer)
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, *buffer, *length))
        goto error;

    return 1;

error:
    if (*buffer)
    {
        free (*buffer);
        *buffer = NULL;
    }
    *length = 0;
    return 0;
}

/*
 * Reads pointer in message (1 byte + content).
 *
 * Returns:
 *   1: OK
 *   0: error (not enough bytes remaining in buffer)
 */

int
weechat_relay_parse_read_pointer (struct t_weechat_relay_msg *msg,
                                  const void **pointer)
{
    int rc;
    unsigned char length;
    unsigned long value;
    char str_pointer[256];

    if (!weechat_relay_parse_read_bytes (msg, &length, 1))
        return 0;

    if (!weechat_relay_parse_read_bytes (msg, str_pointer, length))
        return 0;
    str_pointer[length] = '\0';

    rc = sscanf (str_pointer, "%lx", &value);
    if ((rc != EOF) && (rc != 0))
    {
        *pointer = (const void *)value;
        return 1;
    }

    return 0;
}

/*
 * Reads a char object in message (1 byte).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_char (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_CHAR);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, &obj->value_char, 1))
        goto error;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads an integer object in message (4 bytes).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_integer (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_INTEGER);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_integer (msg, &obj->value_integer))
        goto error;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads a long integer object in message (1 byte + content).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_long (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;
    unsigned char length;
    char str_long[256];
    long value;
    int rc;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_LONG);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, &length, 1))
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, str_long, length))
        goto error;

    str_long[length] = '\0';

    rc = sscanf (str_long, "%ld", &value);
    if ((rc == EOF) || (rc == 0))
        goto error;

    obj->value_long = value;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads a string object in message (4 bytes + content).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_string (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_STRING);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_string (msg, &obj->value_string))
        goto error;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads a buffer object in message (4 bytes + content).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_buffer (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_BUFFER);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_buffer (msg,
                                          &obj->value_buffer.buffer,
                                          &obj->value_buffer.length))
    {
        goto error;
    }

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads a pointer object in message (1 byte + content).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_pointer (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_POINTER);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_pointer (msg, &obj->value_pointer))
        goto error;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads a time object in message (1 byte + content).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_time (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;
    unsigned char length;
    char str_time[256];
    unsigned long value;
    int rc;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_TIME);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, &length, 1))
        goto error;

    if (!weechat_relay_parse_read_bytes (msg, str_time, length))
        goto error;

    str_time[length] = '\0';

    rc = sscanf (str_time, "%lu", &value);
    if ((rc == EOF) || (rc == 0))
        goto error;

    obj->value_time = value;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads a hashtable object in message (variable length).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_hashtable (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;
    int i;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_HASHTABLE);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_type (msg, &obj->value_hashtable.type_keys))
        goto error;

    if (!weechat_relay_parse_read_type (msg, &obj->value_hashtable.type_values))
        goto error;

    if (!weechat_relay_parse_read_integer (msg, &obj->value_hashtable.count))
        goto error;
    if (obj->value_hashtable.count < 0)
        goto error;

    obj->value_hashtable.keys = calloc (obj->value_hashtable.count,
                                        sizeof (*obj->value_hashtable.keys));
    if (!obj->value_hashtable.keys)
        goto error;

    obj->value_hashtable.values = calloc (obj->value_hashtable.count,
                                          sizeof (*obj->value_hashtable.values));
    if (!obj->value_hashtable.values)
        goto error;

    for (i = 0; i < obj->value_hashtable.count; i++)
    {
        obj->value_hashtable.keys[i] = weechat_relay_parse_read_object (
            msg, obj->value_hashtable.type_keys);
        if (!obj->value_hashtable.keys[i])
            goto error;
        obj->value_hashtable.values[i] = weechat_relay_parse_read_object (
            msg, obj->value_hashtable.type_values);
        if (!obj->value_hashtable.values[i])
            goto error;
    }

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Splits hpath in a hdata object.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_parse_hdata_split_hpath (const char *hpath,
                                       char ***hpaths, int *num_hpaths)
{
    const char *ptr_hpath, *pos_slash;
    int i;

    if (!hpath || !hpaths || !num_hpaths)
        return 0;

    *num_hpaths = 1;
    for (i = 0; hpath[i]; i++)
    {
        if (hpath[i] == '/')
            (*num_hpaths)++;
    }

    *hpaths = calloc (*num_hpaths, sizeof (char *));
    if (!*hpaths)
        return 0;

    ptr_hpath = hpath;
    for (i = 0; i < *num_hpaths; i++)
    {
        pos_slash = strchr (ptr_hpath, '/');
        if (pos_slash)
        {
            (*hpaths)[i] = strndup (ptr_hpath, pos_slash - ptr_hpath);
            ptr_hpath = pos_slash + 1;
        }
        else
        {
            (*hpaths)[i] = strdup (ptr_hpath);
            ptr_hpath += strlen (ptr_hpath);
        }
    }

    return 1;
}

/*
 * Splits keys in a hdata object.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_parse_hdata_split_keys (const char *keys,
                                      char ***keys_names,
                                      enum t_weechat_relay_obj_type **keys_types,
                                      int *num_keys)
{
    const char *ptr_keys, *pos_comma, *pos_colon;
    char *str_type;
    int i, type;

    if (!keys || !keys_names || !keys_types || !num_keys)
        return 0;

    *keys_names = NULL;
    *keys_types = NULL;
    *num_keys = 0;

    *num_keys = 1;
    for (i = 0; keys[i]; i++)
    {
        if (keys[i] == ',')
            (*num_keys)++;
    }

    *keys_names = calloc (*num_keys, sizeof (char *));
    if (!*keys_names)
        goto error;

    *keys_types = calloc (*num_keys, sizeof (enum t_weechat_relay_obj_type));
    if (!*keys_types)
        goto error;

    ptr_keys = keys;
    for (i = 0; i < *num_keys; i++)
    {
        pos_comma = strchr (ptr_keys, ',');
        if (pos_comma)
        {
            pos_colon = strchr (ptr_keys, ':');
            if (!pos_colon || (pos_colon > pos_comma))
                goto error;
            (*keys_names)[i] = strndup (ptr_keys, pos_colon - ptr_keys);
            str_type = strndup (pos_colon + 1, 3);
            if (!str_type)
                goto error;
            type = weechat_relay_parse_search_type (str_type);
            free (str_type);
            if (type < 0)
                goto error;
            (*keys_types)[i] = (enum t_weechat_relay_obj_type)type;
            ptr_keys = pos_comma + 1;
        }
        else
        {
            pos_colon = strchr (ptr_keys, ':');
            if (!pos_colon)
                goto error;
            (*keys_names)[i] = strndup (ptr_keys, pos_colon - ptr_keys);
            str_type = strndup (pos_colon + 1, 3);
            if (!str_type)
                goto error;
            type = weechat_relay_parse_search_type (str_type);
            free (str_type);
            if (type < 0)
                goto error;
            (*keys_types)[i] = (enum t_weechat_relay_obj_type)type;
            ptr_keys += strlen (ptr_keys);
        }
    }

    return 1;

error:
    if (*keys_names)
    {
        for (i = 0; i < *num_keys; i++)
        {
            if ((*keys_names)[i])
                free ((*keys_names)[i]);
        }
        free (*keys_names);
        *keys_names = NULL;
    }
    if (*keys_types)
    {
        free (*keys_types);
        *keys_types = NULL;
    }
    *num_keys = 0;
    return 0;
}

/*
 * Reads a hdata object in message (variable length).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_hdata (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj, *obj2;
    int i, j;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_HDATA);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_string (msg, &obj->value_hdata.hpath))
        goto error;

    if (!weechat_relay_parse_hdata_split_hpath (obj->value_hdata.hpath,
                                                &obj->value_hdata.hpaths,
                                                &obj->value_hdata.num_hpaths))
        goto error;

    if (!weechat_relay_parse_read_string (msg, &obj->value_hdata.keys))
        goto error;

    if (!weechat_relay_parse_hdata_split_keys (obj->value_hdata.keys,
                                               &obj->value_hdata.keys_names,
                                               &obj->value_hdata.keys_types,
                                               &obj->value_hdata.num_keys))
        goto error;

    if (!weechat_relay_parse_read_integer (msg, &obj->value_hdata.count))
        goto error;
    if (obj->value_hdata.count < 0)
        goto error;

    obj->value_hdata.ppath = calloc (obj->value_hdata.count,
                                     sizeof (*obj->value_hdata.ppath));
    if (!obj->value_hdata.ppath)
        goto error;

    obj->value_hdata.values = calloc (obj->value_hdata.count,
                                      sizeof (*obj->value_hdata.values));
    if (!obj->value_hdata.values)
        goto error;

    for (i = 0; i < obj->value_hdata.count; i++)
    {
        obj->value_hdata.ppath[i] = calloc (
            obj->value_hdata.num_hpaths,
            sizeof (*(obj->value_hdata.ppath[i])));
        if (!obj->value_hdata.ppath[i])
            goto error;
        for (j = 0; j < obj->value_hdata.num_hpaths; j++)
        {
            obj2 = weechat_relay_parse_read_object (msg,
                                                    WEECHAT_RELAY_OBJ_TYPE_POINTER);
            if (!obj2)
                goto error;
            obj->value_hdata.ppath[i][j] = obj2;
        }

        obj->value_hdata.values[i] = calloc (
            obj->value_hdata.num_keys,
            sizeof (*(obj->value_hdata.values[i])));
        if (!obj->value_hdata.values[i])
            goto error;
        for (j = 0; j < obj->value_hdata.num_keys; j++)
        {
            obj2 = weechat_relay_parse_read_object (msg,
                                                    obj->value_hdata.keys_types[j]);
            if (!obj2)
                goto error;
            obj->value_hdata.values[i][j] = obj2;
        }
    }

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads an info object in message (2 strings).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_info (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_INFO);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_string (msg, &obj->value_info.name))
        goto error;
    if (!weechat_relay_parse_read_string (msg, &obj->value_info.value))
        goto error;

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads an infolist object in message (variable length).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_infolist (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;
    enum t_weechat_relay_obj_type type;
    int i, j;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_INFOLIST);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_string (msg, &obj->value_infolist.name))
        goto error;
    if (!weechat_relay_parse_read_integer (msg, &obj->value_infolist.count))
        goto error;
    if (obj->value_infolist.count < 0)
        goto error;

    obj->value_infolist.items = calloc (obj->value_infolist.count,
                                        sizeof (*obj->value_infolist.items));
    if (!obj->value_infolist.items)
        goto error;

    for (i = 0; i < obj->value_infolist.count; i++)
    {
        obj->value_infolist.items[i] = calloc (1, sizeof (*(obj->value_infolist.items[i])));
        if (!obj->value_infolist.items[i])
            goto error;
        if (!weechat_relay_parse_read_integer (msg, &obj->value_infolist.items[i]->count))
            goto error;
        if (obj->value_infolist.items[i]->count < 0)
            goto error;
        obj->value_infolist.items[i]->variables = calloc (
            obj->value_infolist.items[i]->count,
            sizeof (*(obj->value_infolist.items[i]->variables)));
        for (j = 0; j < obj->value_infolist.items[i]->count; j++)
        {
            obj->value_infolist.items[i]->variables[j] = calloc (
                1,
                sizeof (*obj->value_infolist.items[i]->variables[j]));
            if (!obj->value_infolist.items[i]->variables[j])
                goto error;
            if (!weechat_relay_parse_read_string (msg, &obj->value_infolist.items[i]->variables[j]->name))
                goto error;
            if (!weechat_relay_parse_read_type (msg, &type))
                goto error;
            obj->value_infolist.items[i]->variables[j]->obj = weechat_relay_parse_read_object (msg, type);
        }
    }

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads an array object in message (variable length).
 */

struct t_weechat_relay_obj *
weechat_relay_parse_obj_array (struct t_weechat_relay_msg *msg)
{
    struct t_weechat_relay_obj *obj;
    enum t_weechat_relay_obj_type type;
    int i;

    if (!msg)
        return NULL;

    obj = weechat_relay_parse_obj_alloc (WEECHAT_RELAY_OBJ_TYPE_ARRAY);
    if (!obj)
        goto error;

    if (!weechat_relay_parse_read_type (msg, &type))
        goto error;

    if (!weechat_relay_parse_read_integer (msg, &obj->value_array.count))
        goto error;
    if (obj->value_array.count < 0)
        goto error;

    obj->value_array.values = calloc (obj->value_array.count,
                                      sizeof (*obj->value_array.values));
    if (!obj->value_array.values)
        goto error;

    for (i = 0; i < obj->value_array.count; i++)
    {
        obj->value_array.values[i] = weechat_relay_parse_read_object (msg,
                                                                      type);
        if (!obj->value_array.values[i])
            goto error;
    }

    return obj;

error:
    if (obj)
        weechat_relay_parse_obj_free (obj);
    return NULL;
}

/*
 * Reads an object in a message.
 */

struct t_weechat_relay_obj *
weechat_relay_parse_read_object (struct t_weechat_relay_msg *msg,
                                 enum t_weechat_relay_obj_type type)
{
    struct t_weechat_relay_obj *obj;

    if (!msg)
        return NULL;

    obj = NULL;

    switch (type)
    {
        case WEECHAT_RELAY_OBJ_TYPE_CHAR:
            obj = weechat_relay_parse_obj_char (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INTEGER:
            obj = weechat_relay_parse_obj_integer (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_LONG:
            obj = weechat_relay_parse_obj_long (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_STRING:
            obj = weechat_relay_parse_obj_string (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_BUFFER:
            obj = weechat_relay_parse_obj_buffer (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_POINTER:
            obj = weechat_relay_parse_obj_pointer (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_TIME:
            obj = weechat_relay_parse_obj_time (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HASHTABLE:
            obj = weechat_relay_parse_obj_hashtable (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HDATA:
            obj = weechat_relay_parse_obj_hdata (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFO:
            obj = weechat_relay_parse_obj_info (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFOLIST:
            obj = weechat_relay_parse_obj_infolist (msg);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_ARRAY:
            obj = weechat_relay_parse_obj_array (msg);
            break;
        case WEECHAT_RELAY_NUM_OBJ_TYPES:
            break;
    }

    return obj;
}

/*
 * Decompresses data.
 *
 * The variable "size_decompressed" is set with the size of decompressed
 * buffer returned (in bytes).
 *
 * The initial output size must be set to an estimate output size, but the
 * output buffer size ("size_decompressed") can be higher.
 * A typical value here could be 10 * size if we estimate zlib can compress
 * data with a 90% ratio (which is excellent).
 *
 * Returns a pointer to the decompressed message, NULL if error.
 */

void *
weechat_relay_parse_decompress (const void *data, size_t size,
                                size_t initial_output_size,
                                size_t *size_decompressed)
{
    int rc;
    Bytef *dest, *dest2;
    uLongf dest_size, dest_size_alloc, source_size, pct_used, pct_to_add;
    uLongf size_to_add;

    if (!data || (size == 0) || (initial_output_size == 0)
        || !size_decompressed)
    {
        return NULL;
    }

    dest = NULL;

    *size_decompressed = 0;

    /* estimate the decompressed size, by default 10 * size */
    dest_size_alloc = initial_output_size;

    dest = malloc (dest_size_alloc);
    if (!dest)
        goto error;

    /* loop until we manage to decompress whole data in dest */
    while (1)
    {
        dest_size = dest_size_alloc;
        source_size = size;

        rc = uncompress2 (dest,
                          &dest_size,
                          (Bytef *)data,
                          &source_size);

        if (rc == Z_OK)
        {
            /* data successfully decompressed */
            if (dest_size < dest_size_alloc)
            {
                dest2 = realloc (dest, dest_size);
                if (dest2)
                    dest = dest2;
            }
            *size_decompressed = dest_size;
            break;
        }
        if (rc == Z_BUF_ERROR)
        {
            /* not enough space in dest, compute new estimated size */
            if ((source_size > 0) && (source_size < size))
            {
                /* estimate new size needed, according to the bytes used */
                pct_used = (source_size * 100) / size;
                /* we add 10% extra margin */
                pct_to_add = (100 - pct_used) + 10;
                size_to_add = (size * pct_to_add) / 100;
                dest_size_alloc = (size_to_add > 0) ?
                    dest_size_alloc + size_to_add : dest_size_alloc * 2;
            }
            else
            {
                /* double the output buffer */
                dest_size_alloc *= 2;
            }
            dest2 = realloc (dest, dest_size_alloc);
            if (!dest2)
                goto error;
            dest = dest2;
        }
        else
        {
            /* any other error like Z_MEM_ERROR or Z_DATA_ERROR is fatal */
            goto error;
        }
    }

    return dest;

error:
    if (dest)
        free (dest);
    return NULL;
}

/*
 * Allocates a message structure.
 */

struct t_weechat_relay_msg *
weechat_relay_parse_msg_alloc (const void *buffer, size_t size)
{
    struct t_weechat_relay_msg *msg;
    uint32_t msg_size;

    if (!buffer || (size < 6))
        return NULL;

    msg = calloc (1, sizeof (*msg));
    if (!msg)
        return NULL;

    memcpy (&msg_size, buffer, 4);
    msg_size = ntohl (msg_size);

    if (msg_size != size)
        goto error;

    msg->message = malloc (size);
    if (!msg->message)
        goto error;

    memcpy (msg->message, buffer, size);
    msg->length = size;
    msg->length_data = size - 5;

    msg->compression = ((const char *)buffer)[4];

    msg->position = 0;

    switch (msg->compression)
    {
        case WEECHAT_RELAY_COMPRESSION_OFF:
            msg->data_decompressed = NULL;
            msg->length_data_decompressed = size - 5;
            msg->buffer = buffer + 5;
            msg->size = size - 5;
            break;
        case WEECHAT_RELAY_COMPRESSION_ZLIB:
            msg->data_decompressed = weechat_relay_parse_decompress (
                buffer + 5,
                size - 5,
                10 * (size - 5),
                &msg->length_data_decompressed);
            if (!msg->data_decompressed)
                goto error;
            msg->buffer = msg->data_decompressed;
            msg->size = msg->length_data_decompressed;
            break;
        case WEECHAT_RELAY_NUM_COMPRESSIONS:
            break;
    }

    if (!weechat_relay_parse_read_string (msg, &msg->id))
        goto error;

    return msg;

error:
    weechat_relay_parse_msg_free (msg);
    return NULL;
}

/*
 * Frees a message.
 */

void
weechat_relay_parse_msg_free (struct t_weechat_relay_msg *msg)
{
    int i;

    if (!msg)
        return;

    if (msg->message)
        free (msg->message);
    if (msg->data_decompressed)
        free (msg->data_decompressed);

    if (msg->id)
        free (msg->id);

    for (i = 0; i < msg->num_objects; i++)
    {
        weechat_relay_parse_obj_free (msg->objects[i]);
    }
    if (msg->objects)
        free (msg->objects);

    free (msg);
}

/*
 * Parses a WeeChat binary message.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

struct t_weechat_relay_msg *
weechat_relay_parse_message (const void *buffer, size_t size)
{
    struct t_weechat_relay_msg *msg;
    struct t_weechat_relay_obj *obj, **objects;
    enum t_weechat_relay_obj_type type;

    msg = weechat_relay_parse_msg_alloc (buffer, size);
    if (!msg)
        return NULL;

    while (msg->position < msg->size)
    {
        if (!weechat_relay_parse_read_type (msg, &type))
            break;

        obj = weechat_relay_parse_read_object (msg, type);
        if (!obj)
            break;

        /* add object to the list of objects in parsed message */
        msg->num_objects++;
        if (msg->objects)
        {
            objects = realloc (msg->objects,
                               sizeof (*msg->objects) * msg->num_objects);
            if (!objects)
                break;
            msg->objects = objects;
        }
        else
        {
            msg->objects = malloc (sizeof (*msg->objects) * msg->num_objects);
            if (!msg->objects)
                break;
        }
        msg->objects[msg->num_objects - 1] = obj;
    }

    return msg;
}
