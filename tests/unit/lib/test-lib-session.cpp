/*
 * test-lib-session.cpp - test sessions
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
#include <string.h>
#include "lib/weechat-relay.h"
}

TEST_GROUP(LibSession)
{
    struct t_weechat_relay_session *relay_session;
    int fd_pipe[2];

    void setup()
    {
        relay_session = weechat_relay_session_init (0, NULL);
        pipe (fd_pipe);
        relay_session->sock = fd_pipe[1];
    }

    void teardown()
    {
        close (fd_pipe[0]);
        close (fd_pipe[1]);
        weechat_relay_session_free (relay_session);
    }
};

/*
 * Tests functions:
 *   weechat_relay_session_init
 *   weechat_relay_session_free
 */

TEST(LibSession, Init)
{
    struct t_weechat_relay_session *session;
    void *gnutls_sess;

    session = weechat_relay_session_init (123, NULL);

    LONGS_EQUAL(123, session->sock);
    LONGS_EQUAL(0, session->ssl);
    POINTERS_EQUAL(NULL, session->gnutls_sess);
    weechat_relay_session_free (session);

    gnutls_sess = (void *)0x123abc;
    session = weechat_relay_session_init (456, gnutls_sess);
    LONGS_EQUAL(456, session->sock);
    LONGS_EQUAL(1, session->ssl);
    POINTERS_EQUAL(gnutls_sess, session->gnutls_sess);

    weechat_relay_session_free (session);
}

/*
 * Tests functions:
 *   weechat_relay_session_send
 *   weechat_relay_session_recv
 */

TEST(LibSession, SendRecv)
{
    ssize_t num_read, read_buffer[4096];
    char buffer[3] = {'a', 'b', 'c'};

    LONGS_EQUAL(-1, weechat_relay_session_send (NULL, NULL, 0));
    LONGS_EQUAL(-1, weechat_relay_session_send (relay_session, NULL, 0));
    LONGS_EQUAL(-1, weechat_relay_session_send (relay_session, buffer, -1));
    LONGS_EQUAL(-1, weechat_relay_session_send (relay_session, buffer, 0));

    LONGS_EQUAL(3, weechat_relay_session_send (relay_session, buffer, 3));
    relay_session->sock = fd_pipe[0];
    num_read = weechat_relay_session_recv (relay_session, read_buffer, 3);
    relay_session->sock = fd_pipe[1];
    LONGS_EQUAL(3, num_read);
    MEMCMP_EQUAL(buffer, read_buffer, 3);

    LONGS_EQUAL(3, weechat_relay_session_send (relay_session, buffer, 3));
    relay_session->sock = fd_pipe[0];
    num_read = weechat_relay_session_recv (relay_session,
                                           read_buffer, sizeof (read_buffer));
    relay_session->sock = fd_pipe[1];
    LONGS_EQUAL(3, num_read);
    MEMCMP_EQUAL(buffer, read_buffer, 3);
}

/*
 * Tests functions:
 *   weechat_relay_session_buffer_add_bytes
 */

TEST(LibSession, BufferAddBytes)
{
    unsigned char buffer[] = {
        0x00, 0x00, 0x00, 0x0F,                 /* length: 15     */
        0x00,                                   /* no compression */
        's', 't', 'r',                          /* str            */
        0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',  /* "abc"          */
    };

    POINTERS_EQUAL(NULL, relay_session->buffer);
    LONGS_EQUAL(0, relay_session->buffer_size);

    LONGS_EQUAL(0, weechat_relay_session_buffer_add_bytes (NULL,
                                                           NULL, 0));
    LONGS_EQUAL(0, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           NULL, 0));
    LONGS_EQUAL(0, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer, 0L));

    LONGS_EQUAL(1, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer, 3));
    LONGS_EQUAL(3, relay_session->buffer_size);
    MEMCMP_EQUAL(buffer, relay_session->buffer, 3);

    LONGS_EQUAL(1, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer + 3,
                                                           sizeof (buffer) - 3));
    LONGS_EQUAL(sizeof (buffer), relay_session->buffer_size);
    MEMCMP_EQUAL(buffer, relay_session->buffer, sizeof (buffer));
}

/*
 * Tests functions:
 *   weechat_relay_session_buffer_pop
 */

TEST(LibSession, BufferPop)
{
    unsigned char buffer1[] = {
        0x00, 0x00, 0x00, 0x0F,                 /* length: 15     */
        0x00,                                   /* no compression */
        's', 't', 'r',                          /* str            */
        0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',  /* "abc"          */
    };
    unsigned char buffer2[] = {
        0x00, 0x00, 0x00, 0x0F,                 /* length: 15     */
        0x00,                                   /* no compression */
        's', 't', 'r',                          /* str            */
        0x00, 0x00, 0x00, 0x03, 'd', 'e', 'f',  /* "def"          */
    };
    void *buffer;
    size_t size;

    weechat_relay_session_buffer_pop (NULL, NULL, NULL);
    weechat_relay_session_buffer_pop (relay_session, NULL, NULL);
    weechat_relay_session_buffer_pop (relay_session, &buffer, NULL);

    weechat_relay_session_buffer_pop (relay_session, &buffer, &size);
    POINTERS_EQUAL(NULL, buffer);
    LONGS_EQUAL(0, size);
    POINTERS_EQUAL(NULL, relay_session->buffer);
    LONGS_EQUAL(0, relay_session->buffer_size);

    LONGS_EQUAL(1, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer1, 3));

    weechat_relay_session_buffer_pop (relay_session, &buffer, &size);
    POINTERS_EQUAL(NULL, buffer);
    LONGS_EQUAL(0, size);
    CHECK(relay_session->buffer);
    LONGS_EQUAL(3, relay_session->buffer_size);

    LONGS_EQUAL(1, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer1 + 3,
                                                           sizeof (buffer1) - 3));
    LONGS_EQUAL(1, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer2, 3));

    LONGS_EQUAL(sizeof (buffer1) + 3, relay_session->buffer_size);

    weechat_relay_session_buffer_pop (relay_session, &buffer, &size);
    CHECK(buffer);
    MEMCMP_EQUAL(buffer1, buffer, sizeof (buffer1));
    LONGS_EQUAL(sizeof (buffer1), size);
    CHECK(relay_session->buffer);
    LONGS_EQUAL(3, relay_session->buffer_size);
    free (buffer);

    LONGS_EQUAL(1, weechat_relay_session_buffer_add_bytes (relay_session,
                                                           buffer2 + 3,
                                                           sizeof (buffer2) - 3));

    LONGS_EQUAL(sizeof (buffer2), relay_session->buffer_size);

    weechat_relay_session_buffer_pop (relay_session, &buffer, &size);
    CHECK(buffer);
    MEMCMP_EQUAL(buffer2, buffer, sizeof (buffer2));
    LONGS_EQUAL(sizeof (buffer1), size);
    POINTERS_EQUAL(NULL, relay_session->buffer);
    LONGS_EQUAL(0, relay_session->buffer_size);
    free (buffer);
}
