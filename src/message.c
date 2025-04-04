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

/* Message functions for WeeChat relay command line tool */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "lib/weechat-relay.h"

#include "cli.h"
#include "util.h"


int parse_indent = 0;


void relay_message_display_object (struct t_weechat_relay_obj *obj);


/*
 * Displays a string with indentation.
 */

void
relay_message_printf (const char *format, ...)
{
    va_list args;
    char str_format[32];

    snprintf (str_format, sizeof (str_format),
              "%%-%ds", parse_indent);
    printf (str_format, " ");

    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}

/*
 * Displays a char object.
 */

void
relay_message_display_char (char value)
{
    relay_message_printf ("char: '%c' (%d, 0x%02x)\n",
                          (value < 32) ? '?' : value,
                          value,
                          value);
}

/*
 * Displays an integer object.
 */

void
relay_message_display_integer (int value)
{
    relay_message_printf ("integer: %d\n", value);
}

/*
 * Displays an long integer object.
 */

void
relay_message_display_long (long value)
{
    relay_message_printf ("long: %ld\n", value);
}

/*
 * Displays a string object.
 */

void
relay_message_display_string (const char *value)
{
    if (value)
        relay_message_printf ("str: \"%s\"\n", value);
    else
        relay_message_printf ("str: NULL\n");
}

/*
 * Displays a buffer object.
 */

void
relay_message_display_buffer (struct t_weechat_relay_obj_buffer *value)
{
    char *str_dump, str_format[32], str_spaces[1024];

    relay_message_printf ("buffer: 0x%lx, length: %d\n",
            (unsigned long int)value->buffer, value->length);
    if (value->buffer)
    {
        snprintf (str_format, sizeof (str_format),
                  "%%-%ds", parse_indent + 2);
        snprintf (str_spaces, sizeof (str_spaces), str_format, " ");
        str_dump = string_hex_dump (value->buffer, value->length,
                                    16, str_spaces, NULL);
        if (str_dump)
        {
            printf ("%s\n", str_dump);
            free (str_dump);
        }
    }
}

/*
 * Displays a pointer object.
 */

void
relay_message_display_pointer (const void *value)
{
    relay_message_printf ("pointer: 0x%lx\n", (unsigned long)value);
}

/*
 * Displays a time object.
 */

void
relay_message_display_time (time_t value)
{
    char str_time[1024], *pos;

    ctime_r (&value, str_time);
    pos = strchr (str_time, '\n');
    if (pos)
        pos[0] = '\0';
    relay_message_printf ("time: %ld (%s)\n", (long)value, str_time);
}

/*
 * Displays an hashtable object.
 */

void
relay_message_display_hashtable (struct t_weechat_relay_obj_hashtable *value)
{
    int i;

    relay_message_printf ("hashtable: %d items:\n", value->count);
    parse_indent += 2;
    for (i = 0; i < value->count; i++)
    {
        relay_message_printf ("item %d:\n", i + 1);
        parse_indent += 2;
        relay_message_printf ("key:\n");
        parse_indent += 2;
        relay_message_display_object (value->keys[i]);
        parse_indent -= 2;
        relay_message_printf ("value:\n");
        parse_indent += 2;
        relay_message_display_object (value->values[i]);
        parse_indent -= 2;
        parse_indent -= 2;
    }
    parse_indent -= 2;
}

/*
 * Displays an hdata object.
 */

void
relay_message_display_hdata (struct t_weechat_relay_obj_hdata *value)
{
    int i, j;

    relay_message_printf ("hdata: h-path: %s, %d items:\n",
                          value->hpath,
                          value->count);
    parse_indent += 2;
    relay_message_printf ("keys: %s\n", value->keys);
    for (i = 0; i < value->count; i++)
    {
        relay_message_printf ("item %d:\n", i + 1);
        parse_indent += 2;
        relay_message_printf ("p-path:\n");
        parse_indent += 2;
        for (j = 0; j < value->num_hpaths; j++)
        {
            relay_message_display_object (value->ppath[i][j]);
        }
        parse_indent -= 2;
        relay_message_printf ("values:\n");
        parse_indent += 2;
        for (j = 0; j < value->num_keys; j++)
        {
            relay_message_printf ("%s:\n", value->keys_names[j]);
            parse_indent += 2;
            relay_message_display_object (value->values[i][j]);
            parse_indent -= 2;
        }
        parse_indent -= 2;
        parse_indent -= 2;
    }
    parse_indent -= 2;
}

/*
 * Displays an info object.
 */

void
relay_message_display_info (struct t_weechat_relay_obj_info *value)
{
    relay_message_printf ("info: \"%s\" -> \"%s\"\n", value->name, value->value);
}

/*
 * Displays an infolist object.
 */

void
relay_message_display_infolist (struct t_weechat_relay_obj_infolist *value)
{
    struct t_weechat_relay_obj_infolist_item *ptr_item;
    struct t_weechat_relay_obj_infolist_var *ptr_var;
    int i, j;

    relay_message_printf ("infolist:\n");
    parse_indent += 2;
    for (i = 0; i < value->count; i++)
    {
        relay_message_printf ("item %d:\n", i + 1);
        parse_indent += 2;
        ptr_item = value->items[i];
        for (j = 0; j < ptr_item->count; j++)
        {
            ptr_var = ptr_item->variables[j];
            relay_message_printf ("%s:\n", ptr_var->name);
            parse_indent += 2;
            relay_message_display_object (ptr_var->value);
            parse_indent -= 2;
        }
        parse_indent -= 2;
    }
    parse_indent -= 2;
}

/*
 * Displays an array object.
 */

void
relay_message_display_array (struct t_weechat_relay_obj_array *value)
{
    int i;

    relay_message_printf ("array: %d values:\n", value->count);
    parse_indent += 2;
    for (i = 0; i < value->count; i++)
    {
        relay_message_display_object (value->values[i]);
    }
    parse_indent -= 2;
}

/*
 * Displays a message object.
 */

void
relay_message_display_object (struct t_weechat_relay_obj *obj)
{
    if (!obj)
    {
        relay_message_printf ("NULL");
        return;
    }

    switch (obj->type)
    {
        case WEECHAT_RELAY_OBJ_TYPE_CHAR:
            relay_message_display_char (obj->value_char);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INTEGER:
            relay_message_display_integer (obj->value_integer);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_LONG:
            relay_message_display_long (obj->value_long);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_STRING:
            relay_message_display_string (obj->value_string);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_BUFFER:
            relay_message_display_buffer (&obj->value_buffer);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_POINTER:
            relay_message_display_pointer (obj->value_pointer);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_TIME:
            relay_message_display_time (obj->value_time);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HASHTABLE:
            relay_message_display_hashtable (&obj->value_hashtable);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HDATA:
            relay_message_display_hdata (&obj->value_hdata);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFO:
            relay_message_display_info (&obj->value_info);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFOLIST:
            relay_message_display_infolist (&obj->value_infolist);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_ARRAY:
            relay_message_display_array (&obj->value_array);
            break;
        case WEECHAT_RELAY_NUM_OBJ_TYPES:
            break;
    }
}

/*
 * Parses and displays a message.
 */

void
relay_message_display (const void *buffer, size_t size)
{
    struct t_weechat_relay_parsed_msg *parsed_msg;
    char *str_dump, str_format[32], str_spaces[1024];
    struct timeval tv1, tv2;
    long long time_diff;
    int i, ratio;

    parse_indent = 4;

    gettimeofday (&tv1, NULL);

    parsed_msg = weechat_relay_parse_message (buffer, size);
    if (!parsed_msg)
    {
        relay_message_printf ("ERROR: parse of message failed\n");
        return;
    }

    gettimeofday (&tv2, NULL);
    time_diff = timeval_diff (&tv1, &tv2);

    /* message info */
    if (parsed_msg->compression)
    {
        ratio = 100 - (int)((parsed_msg->length_data * 100)
                            / parsed_msg->length_data_decompressed);
        relay_message_printf (
            "message (length: %d bytes, data: %d bytes, "
            "decompressed (%s): %d bytes, ratio: %d%%, parsed in %.2fms):\n",
            parsed_msg->length,
            parsed_msg->length_data,
            weechat_relay_compression_string[parsed_msg->compression],
            parsed_msg->length_data_decompressed,
            ratio,
            ((float)time_diff) / 1000);

        if (relay_cli_debug >= 2)
        {
            parse_indent += 2;
            snprintf (str_format, sizeof (str_format),
                      "%%-%ds", parse_indent + 2);
            snprintf (str_spaces, sizeof (str_spaces), str_format, " ");
            str_dump = string_hex_dump (parsed_msg->data_decompressed,
                                        parsed_msg->length_data_decompressed,
                                        16, str_spaces, NULL);
            if (str_dump)
            {
                relay_message_printf ("decompressed data (%d bytes):\n",
                                      parsed_msg->length_data_decompressed);
                printf ("%s\n", str_dump);
                free (str_dump);
            }
            parse_indent -= 2;
        }
    }
    else
    {
        relay_message_printf ("message (length: %d bytes, data: %d bytes, "
                              "no compression, parsed in %.2fms):\n",
                              parsed_msg->length,
                              parsed_msg->length_data,
                              ((float)time_diff) / 1000);
    }

    parse_indent += 2;

    if (parsed_msg->id)
        relay_message_printf ("id: \"%s\"\n", parsed_msg->id);
    else
        relay_message_printf ("id: NULL\n");

    /* message objects */
    if (!parsed_msg->id || (strcmp (parsed_msg->id, "quiet") != 0))
    {
        for (i = 0; i < parsed_msg->num_objects; i++)
        {
            relay_message_display_object (parsed_msg->objects[i]);
        }
    }

    weechat_relay_parse_msg_free (parsed_msg);
}
