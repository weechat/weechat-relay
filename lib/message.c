/*
 * message.c - build and send binary messages from WeeChat to client
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
    weechat_relay_msg_add_int (new_msg, 0);
    weechat_relay_msg_add_char (new_msg, 0);

    /* add id */
    weechat_relay_msg_add_string (new_msg, id);

    return new_msg;
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

    return 1;
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
                             int position, const void *buffer, size_t size)
{
    if (!msg || !msg->data || (position < 0) || !buffer || (size == 0)
        || (position + size) > msg->data_size)
    {
        return 0;
    }

    memcpy (msg->data + position, buffer, size);

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
                            const char *string)
{
    return weechat_relay_msg_add_bytes (msg,
                                        string,
                                        (string) ? strlen (string) : 0);
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
weechat_relay_msg_add_int (struct t_weechat_relay_msg *msg, int value)
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
        if (!weechat_relay_msg_add_int (msg, length))
            return 0;
        if (length > 0)
            return weechat_relay_msg_add_bytes (msg, string, length);
        else
            return 1;
    }
    else
    {
        return weechat_relay_msg_add_int (msg, -1);
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
                              const void *buffer, size_t length)
{
    if (buffer)
    {
        if (!weechat_relay_msg_add_int (msg, length))
            return 0;
        if (length > 0)
            return weechat_relay_msg_add_bytes (msg, buffer, length);
        else
            return 1;
    }
    else
    {
        return weechat_relay_msg_add_int (msg, -1);
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
weechat_relay_msg_add_pointer (struct t_weechat_relay_msg *msg, void *pointer)
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
