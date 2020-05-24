/*
 * test-src-network.cpp - test network functions in command-line interface
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
#include <gnutls/gnutls.h>
#include "src/network.h"
}

TEST_GROUP(SrcNetwork)
{
};

/*
 * Tests functions:
 *   relay_network_init
 *   relay_network_end
 */

TEST(SrcNetwork, InitEnd)
{
    relay_network_init ();
    relay_network_end ();
}

/*
 * Tests functions:
 *   relay_network_connect
 */

TEST(SrcNetwork, Connect)
{
    gnutls_session_t gnutls_sess;

    LONGS_EQUAL(-1,
                relay_network_connect ("localhost", "800000",
                                       0, 0, NULL));
    LONGS_EQUAL(-1,
                relay_network_connect ("localhost", "800000",
                                       0, 0, &gnutls_sess));
}

/*
 * Tests functions:
 *   relay_network_disconnect
 */

TEST(SrcNetwork, Disconnect)
{
    relay_network_disconnect (NULL);
}
