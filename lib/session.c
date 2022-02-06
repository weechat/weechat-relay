/*
 * session.c - session functions of WeeChat Relay lib
 *
 * Copyright (C) 2019-2022 Sébastien Helleu <flashcode@flashtux.org>
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <gnutls/gnutls.h>

#include "weechat-relay.h"


/*
 * Initializes a relay session.
 *
 * The session can be used in both directions:
 * - as server: read commands from client, send binary messages to client
 * - as client: read messages from WeeChat, send commands to WeeChat
 *
 * Note: the session must be freed by weechat_relay_session_free.
 *
 * Returns the pointer to the new session, NULL if error.
 *
 */

struct t_weechat_relay_session *
weechat_relay_session_init (int sock, void *gnutls_sess)
{
    struct t_weechat_relay_session *session;

    session = calloc (1, sizeof (*session));
    if (!session)
        return NULL;

    session->sock = sock;
    session->ssl = (gnutls_sess) ? 1 : 0;
    session->gnutls_sess = gnutls_sess;

    session->buffer = NULL;
    session->buffer_size = 0;

    return session;
}

/*
 * Sends bytes from WeeChat or client.
 *
 * Returns the number of bytes stored in buffer, -1 if error.
 */

ssize_t
weechat_relay_session_send (struct t_weechat_relay_session *session,
                            void *buffer, size_t size)
{
    ssize_t num_sent;

    if (!session || !buffer || (size == 0))
        return -1;

    if (session->ssl)
    {
        num_sent = gnutls_record_send (
            *((gnutls_session_t *)session->gnutls_sess), buffer, size);
    }
    else
    {
        num_sent = write (session->sock, buffer, size);
    }

    return (num_sent >= 0) ? num_sent : -1;
}

/*
 * Reads bytes from WeeChat or client.
 *
 * Returns the number of bytes stored in buffer, -1 if error.
 */

ssize_t
weechat_relay_session_recv (struct t_weechat_relay_session *session,
                            void *buffer, size_t size)
{
    ssize_t num_recv;

    if (!session || !buffer || (size == 0))
        return -1;

    if (session->ssl)
    {
        num_recv = gnutls_record_recv (
            *((gnutls_session_t *)session->gnutls_sess), buffer, size);
    }
    else
    {
        num_recv = read (session->sock, buffer, size);
    }

    return (num_recv >= 0) ? num_recv : -1;
}

/*
 * Adds bytes to the session buffer.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
weechat_relay_session_buffer_add_bytes (struct t_weechat_relay_session *session,
                                        const void *buffer, size_t size)
{
    void *new_buffer;

    if (!session || !buffer || (size == 0))
        return 0;

    if (session->buffer)
    {
        new_buffer = realloc (session->buffer, session->buffer_size + size);
        if (!new_buffer)
            return 0;
        session->buffer = new_buffer;
        memcpy (session->buffer + session->buffer_size, buffer, size);
        session->buffer_size += size;
    }
    else
    {
        session->buffer = malloc (size);
        if (!session->buffer)
            return 0;
        memcpy (session->buffer, buffer, size);
        session->buffer_size = size;
    }

    return 1;
}

/*
 * Pops the first message from the session buffer.
 *
 * If a complete message is not ready, *buffer is set to NULL and *size to 0.
 *
 * If a message is returned, it is removed from the session buffer.
 *
 * Note: *buffer returned must be freed after use.
 */

void
weechat_relay_session_buffer_pop (struct t_weechat_relay_session *session,
                                  void **buffer, size_t *size)
{
    uint32_t msg_size;
    size_t new_size;
    void *new_buffer;

    if (!session || !buffer || !size)
        return;

    *buffer = NULL;
    *size = 0;

    if (!session->buffer || session->buffer_size < 5)
        return;

    memcpy (&msg_size, session->buffer, 4);
    msg_size = ntohl (msg_size);
    if (msg_size > session->buffer_size)
    {
        /* incomplete message, it will be processed later */
        return;
    }

    *buffer = malloc (msg_size);
    if (!*buffer)
        return;
    memcpy (*buffer, session->buffer, msg_size);
    *size = msg_size;

    if (msg_size == session->buffer_size)
    {
        free (session->buffer);
        session->buffer = NULL;
        session->buffer_size = 0;
    }
    else
    {
        new_size = session->buffer_size - msg_size;
        new_buffer = malloc (new_size);
        if (new_buffer)
        {
            memcpy (new_buffer, session->buffer + msg_size, new_size);
            free (session->buffer);
            session->buffer = new_buffer;
            session->buffer_size = new_size;
        }
        else
        {
            free (session->buffer);
            session->buffer = NULL;
            session->buffer_size = 0;
        }
    }
}

/*
 * Frees a relay session.
 */

void
weechat_relay_session_free (struct t_weechat_relay_session *session)
{
    if (!session)
        return;

    if (session->buffer)
        free (session->buffer);

    free (session);
}
