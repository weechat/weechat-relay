/*
 * test-lib-session.cpp - test sessions
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

#include "CppUTest/TestHarness.h"

extern "C"
{
#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#endif
#include <unistd.h>
#include <string.h>
#include "tests/tests.h"
#include "lib/weechat-relay.h"
}

TEST_GROUP(LibSession)
{
    struct t_weechat_relay_session relay_session;
    int fd_pipe[2];

    void setup()
    {
        pipe (fd_pipe);
        relay_session.sock = fd_pipe[1];
        relay_session.ssl = 0;
        relay_session.gnutls_sess = NULL;
    }

    void teardown()
    {
        close (fd_pipe[0]);
        close (fd_pipe[1]);
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

TEST(LibSession, SessionSendRecv)
{
    ssize_t num_read, read_buffer[4096];
    char buffer[3] = {'a', 'b', 'c'};

    LONGS_EQUAL(-1, weechat_relay_session_send (NULL, NULL, 0));
    LONGS_EQUAL(-1, weechat_relay_session_send (&relay_session, NULL, 0));
    LONGS_EQUAL(-1, weechat_relay_session_send (&relay_session, buffer, -1));
    LONGS_EQUAL(-1, weechat_relay_session_send (&relay_session, buffer, 0));

    LONGS_EQUAL(3, weechat_relay_session_send (&relay_session, buffer, 3));
    relay_session.sock = fd_pipe[0];
    num_read = weechat_relay_session_recv (&relay_session, read_buffer, 3);
    relay_session.sock = fd_pipe[1];
    LONGS_EQUAL(3, num_read);
    MEMCMP_EQUAL(buffer, read_buffer, 3);

    LONGS_EQUAL(3, weechat_relay_session_send (&relay_session, buffer, 3));
    relay_session.sock = fd_pipe[0];
    num_read = weechat_relay_session_recv (&relay_session,
                                           read_buffer, sizeof (read_buffer));
    relay_session.sock = fd_pipe[1];
    LONGS_EQUAL(3, num_read);
    MEMCMP_EQUAL(buffer, read_buffer, 3);
}
