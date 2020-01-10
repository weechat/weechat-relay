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
};

/*
 * Tests functions:
 *   weechat_relay_session_init
 *   weechat_relay_session_free
 */

TEST(LibSession, Init)
{
    struct t_weechat_relay_session *session;
    void *gnutls_session;

    session = weechat_relay_session_init (123, NULL);
    LONGS_EQUAL(123, session->sock);
    LONGS_EQUAL(0, session->ssl);
    POINTERS_EQUAL(NULL, session->gnutls_session);
    weechat_relay_session_free (session);

    gnutls_session = (void *)0x123abc;
    session = weechat_relay_session_init (456, gnutls_session);
    LONGS_EQUAL(456, session->sock);
    LONGS_EQUAL(1, session->ssl);
    POINTERS_EQUAL(gnutls_session, session->gnutls_session);
    weechat_relay_session_free (session);
}
