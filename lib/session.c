/*
 * session.c - session functions of WeeChat Relay lib
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

#include <unistd.h>
#include <stdlib.h>

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

    session = malloc (sizeof (*session));
    if (!session)
        return NULL;

    session->sock = sock;
    session->ssl = (gnutls_sess) ? 1 : 0;
    session->gnutls_sess = gnutls_sess;

    return session;
}
/*
 * Sends bytes from WeeChat or client.
 *
 * Returns the number of bytes stored in buffer, -1 if error.
 */

ssize_t
weechat_relay_session_send (struct t_weechat_relay_session *session,
                            void *buffer, size_t length)
{
    ssize_t num_sent;

    if (!session || (length == 0))
        return -1;

    if (session->ssl)
    {
        num_sent = gnutls_record_send (
            *((gnutls_session_t *)session->gnutls_sess), buffer, length);
    }
    else
    {
        num_sent = write (session->sock, buffer, length);
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
                            void *buffer, size_t length)
{
    ssize_t num_recv;

    if (!session || (length <= 0))
        return -1;

    if (session->ssl)
    {
        num_recv = gnutls_record_recv (
            *((gnutls_session_t *)session->gnutls_sess), buffer, length);
    }
    else
    {
        num_recv = read (session->sock, buffer, length);
    }

    return (num_recv >= 0) ? num_recv : -1;
}

/*
 * Frees a relay session.
 */

void
weechat_relay_session_free (struct t_weechat_relay_session *session)
{
    free (session);
}
