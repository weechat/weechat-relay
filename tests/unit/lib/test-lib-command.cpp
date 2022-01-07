/*
 * test-lib-command.cpp - test commands
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

#include "CppUTest/TestHarness.h"

extern "C"
{
#include <unistd.h>
#include <string.h>
#include "lib/weechat-relay.h"
#include "lib/command.h"
}

#define RELAY_CHECK_RECV(__command)                                     \
    num_read = read (fd_pipe[0],                                        \
                     read_buffer, sizeof (read_buffer) - 1);            \
    read_buffer[num_read] = '\0';                                       \
    LONGS_EQUAL(strlen (__command), num_read);                          \
    STRCMP_EQUAL(__command, read_buffer);

TEST_GROUP(LibCommand)
{
    struct t_weechat_relay_session relay_session;
    int fd_pipe[2];
    char read_buffer[4096];
    ssize_t num_read;

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
 *   weechat_relay_cmd_escape
 */

TEST(LibCommand, Escape)
{
    char *result;

    result = weechat_relay_cmd_escape (NULL, NULL);
    POINTERS_EQUAL(NULL, result);

    result = weechat_relay_cmd_escape (NULL, "");
    POINTERS_EQUAL(NULL, result);

    result = weechat_relay_cmd_escape ("", NULL);
    POINTERS_EQUAL(NULL, result);

    result = weechat_relay_cmd_escape ("", "");
    STRCMP_EQUAL("", result);
    free (result);

    result = weechat_relay_cmd_escape ("abc", "");
    STRCMP_EQUAL("abc", result);
    free (result);

    result = weechat_relay_cmd_escape ("abc", ".,;");
    STRCMP_EQUAL("abc", result);
    free (result);

    result = weechat_relay_cmd_escape ("abc.def,ghi;jkl/mno", ",");
    STRCMP_EQUAL("abc.def\\,ghi;jkl/mno", result);
    free (result);

    result = weechat_relay_cmd_escape ("abc.def,ghi;jkl/mno", ".,;");
    STRCMP_EQUAL("abc\\.def\\,ghi\\;jkl/mno", result);
    free (result);
}

/*
 * Tests functions:
 *   weechat_relay_cmd_send
 */

TEST(LibCommand, Send)
{
    const char *one_arg[2] = { "arg1", NULL };
    const char *two_args[3] = { "arg1", "arg2", NULL };

    LONGS_EQUAL(-1, weechat_relay_cmd_send (NULL, NULL, NULL, NULL));
    LONGS_EQUAL(-1, weechat_relay_cmd_send (&relay_session, NULL, NULL, NULL));
    LONGS_EQUAL(-1, weechat_relay_cmd_send (NULL, "id", NULL, NULL));
    LONGS_EQUAL(-1, weechat_relay_cmd_send (&relay_session, "id", NULL, NULL));

    LONGS_EQUAL(5, weechat_relay_cmd_send (&relay_session, NULL, "test", NULL));
    RELAY_CHECK_RECV("test\n");

    LONGS_EQUAL(10, weechat_relay_cmd_send (&relay_session, "id", "test", NULL));
    RELAY_CHECK_RECV("(id) test\n");

    LONGS_EQUAL(15, weechat_relay_cmd_send (&relay_session, "id", "test", one_arg));
    RELAY_CHECK_RECV("(id) test arg1\n");

    LONGS_EQUAL(20, weechat_relay_cmd_send (&relay_session, "id", "test", two_args));
    RELAY_CHECK_RECV("(id) test arg1 arg2\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_handshake
 */

TEST(LibCommand, Handshake)
{
    LONGS_EQUAL(-1,
                weechat_relay_cmd_handshake (NULL, NULL, NULL,
                                             WEECHAT_RELAY_COMPRESSION_OFF));

    LONGS_EQUAL(34,
                weechat_relay_cmd_handshake (&relay_session, "my_id", NULL,
                                             WEECHAT_RELAY_COMPRESSION_OFF));
    RELAY_CHECK_RECV("(my_id) handshake compression=off\n");

    LONGS_EQUAL(35,
                weechat_relay_cmd_handshake (&relay_session, "my_id", NULL,
                                             WEECHAT_RELAY_COMPRESSION_ZLIB));
    RELAY_CHECK_RECV("(my_id) handshake compression=zlib\n");

    LONGS_EQUAL(35,
                weechat_relay_cmd_handshake (&relay_session, "my_id", NULL,
                                             WEECHAT_RELAY_COMPRESSION_ZSTD));
    RELAY_CHECK_RECV("(my_id) handshake compression=zstd\n");

    LONGS_EQUAL(81,
                weechat_relay_cmd_handshake (
                    &relay_session,
                    "my_id",
                    "plain:sha256:pbkdf2+sha256",
                    WEECHAT_RELAY_COMPRESSION_ZLIB));
    RELAY_CHECK_RECV("(my_id) handshake password_hash_algo=plain:sha256:"
                     "pbkdf2+sha256,compression=zlib\n");

    LONGS_EQUAL(81,
                weechat_relay_cmd_handshake (
                    &relay_session,
                    "my_id",
                    "plain:sha256:pbkdf2+sha256",
                    WEECHAT_RELAY_COMPRESSION_ZSTD));
    RELAY_CHECK_RECV("(my_id) handshake password_hash_algo=plain:sha256:"
                     "pbkdf2+sha256,compression=zstd\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_init
 */

TEST(LibCommand, Init)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_init (NULL, NULL, NULL,
                                            WEECHAT_RELAY_COMPRESSION_OFF));

    LONGS_EQUAL(29, weechat_relay_cmd_init (&relay_session, "my_id", NULL,
                                            WEECHAT_RELAY_COMPRESSION_OFF));
    RELAY_CHECK_RECV("(my_id) init compression=off\n");

    LONGS_EQUAL(30, weechat_relay_cmd_init (&relay_session, "my_id", NULL,
                                            WEECHAT_RELAY_COMPRESSION_ZLIB));
    RELAY_CHECK_RECV("(my_id) init compression=zlib\n");

    LONGS_EQUAL(30, weechat_relay_cmd_init (&relay_session, "my_id", NULL,
                                            WEECHAT_RELAY_COMPRESSION_ZSTD));
    RELAY_CHECK_RECV("(my_id) init compression=zstd\n");

    LONGS_EQUAL(46, weechat_relay_cmd_init (&relay_session, "my_id", "secret",
                                            WEECHAT_RELAY_COMPRESSION_ZLIB));
    RELAY_CHECK_RECV("(my_id) init password=secret,compression=zlib\n");

    LONGS_EQUAL(46, weechat_relay_cmd_init (&relay_session, "my_id", "secret",
                                            WEECHAT_RELAY_COMPRESSION_ZSTD));
    RELAY_CHECK_RECV("(my_id) init password=secret,compression=zstd\n");

    LONGS_EQUAL(48, weechat_relay_cmd_init (&relay_session, "my_id", "sec,ret",
                                            WEECHAT_RELAY_COMPRESSION_ZLIB));
    RELAY_CHECK_RECV("(my_id) init password=sec\\,ret,compression=zlib\n");

    LONGS_EQUAL(48, weechat_relay_cmd_init (&relay_session, "my_id", "sec,ret",
                                            WEECHAT_RELAY_COMPRESSION_ZSTD));
    RELAY_CHECK_RECV("(my_id) init password=sec\\,ret,compression=zstd\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_hdata
 */

TEST(LibCommand, Hdata)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_hdata (NULL, NULL, NULL, NULL));

    LONGS_EQUAL(6, weechat_relay_cmd_hdata (&relay_session, NULL, NULL, NULL));
    RELAY_CHECK_RECV("hdata\n");

    LONGS_EQUAL(14, weechat_relay_cmd_hdata (&relay_session, "my_id",
                                             NULL, NULL));
    RELAY_CHECK_RECV("(my_id) hdata\n");

    LONGS_EQUAL(14, weechat_relay_cmd_hdata (&relay_session, "my_id",
                                             NULL, NULL));
    RELAY_CHECK_RECV("(my_id) hdata\n");

    LONGS_EQUAL(19, weechat_relay_cmd_hdata (&relay_session, "my_id",
                                             "path", NULL));
    RELAY_CHECK_RECV("(my_id) hdata path\n");

    LONGS_EQUAL(14, weechat_relay_cmd_hdata (&relay_session, "my_id",
                                             NULL, "keys"));
    RELAY_CHECK_RECV("(my_id) hdata\n");

    LONGS_EQUAL(24, weechat_relay_cmd_hdata (&relay_session, "my_id",
                                             "path", "keys"));
    RELAY_CHECK_RECV("(my_id) hdata path keys\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_info
 */

TEST(LibCommand, Info)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_info (NULL, NULL, NULL));

    LONGS_EQUAL(5, weechat_relay_cmd_info (&relay_session, NULL, NULL));
    RELAY_CHECK_RECV("info\n");

    LONGS_EQUAL(13, weechat_relay_cmd_info (&relay_session, "my_id", NULL));
    RELAY_CHECK_RECV("(my_id) info\n");

    LONGS_EQUAL(18, weechat_relay_cmd_info (&relay_session, "my_id", "name"));
    RELAY_CHECK_RECV("(my_id) info name\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_infolist
 */

TEST(LibCommand, Infolist)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_infolist (NULL, NULL, NULL, NULL, NULL));

    LONGS_EQUAL(9, weechat_relay_cmd_infolist (&relay_session, NULL,
                                               NULL, NULL, NULL));
    RELAY_CHECK_RECV("infolist\n");

    LONGS_EQUAL(17, weechat_relay_cmd_infolist (&relay_session, "my_id",
                                                NULL, NULL, NULL));
    RELAY_CHECK_RECV("(my_id) infolist\n");

    LONGS_EQUAL(22, weechat_relay_cmd_infolist (&relay_session, "my_id",
                                                "name", NULL, NULL));
    RELAY_CHECK_RECV("(my_id) infolist name\n");

    LONGS_EQUAL(31, weechat_relay_cmd_infolist (&relay_session, "my_id",
                                                "name", "0x123abc", NULL));
    RELAY_CHECK_RECV("(my_id) infolist name 0x123abc\n");

    LONGS_EQUAL(36, weechat_relay_cmd_infolist (&relay_session, "my_id",
                                                "name", "0x123abc", "args"));
    RELAY_CHECK_RECV("(my_id) infolist name 0x123abc args\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_nicklist
 */

TEST(LibCommand, Nicklist)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_nicklist (NULL, NULL, NULL));

    LONGS_EQUAL(9, weechat_relay_cmd_nicklist (&relay_session, NULL, NULL));
    RELAY_CHECK_RECV("nicklist\n");

    LONGS_EQUAL(17, weechat_relay_cmd_nicklist (&relay_session, "my_id",
                                                NULL));
    RELAY_CHECK_RECV("(my_id) nicklist\n");

    LONGS_EQUAL(30, weechat_relay_cmd_nicklist (&relay_session, "my_id",
                                                "core.weechat"));
    RELAY_CHECK_RECV("(my_id) nicklist core.weechat\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_input
 */

TEST(LibCommand, Input)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_input (NULL, NULL, NULL, NULL));

    LONGS_EQUAL(6, weechat_relay_cmd_input (&relay_session, NULL, NULL, NULL));
    RELAY_CHECK_RECV("input\n");

    LONGS_EQUAL(14, weechat_relay_cmd_input (&relay_session, "my_id",
                                             NULL, NULL));
    RELAY_CHECK_RECV("(my_id) input\n");

    LONGS_EQUAL(27, weechat_relay_cmd_input (&relay_session, "my_id",
                                             "core.weechat", NULL));
    RELAY_CHECK_RECV("(my_id) input core.weechat\n");

    LONGS_EQUAL(40, weechat_relay_cmd_input (&relay_session, "my_id",
                                             "core.weechat", "/help filter"));
    RELAY_CHECK_RECV("(my_id) input core.weechat /help filter\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_completion
 */

TEST(LibCommand, Completion)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_completion (NULL, NULL, NULL, 0, NULL));

    LONGS_EQUAL(11, weechat_relay_cmd_completion (&relay_session, NULL,
                                                 NULL, 0, NULL));
    RELAY_CHECK_RECV("completion\n");

    LONGS_EQUAL(19, weechat_relay_cmd_completion (&relay_session, "my_id",
                                                  NULL, 0, NULL));
    RELAY_CHECK_RECV("(my_id) completion\n");

    LONGS_EQUAL(34, weechat_relay_cmd_completion (&relay_session, "my_id",
                                                  "core.weechat", 0, NULL));
    RELAY_CHECK_RECV("(my_id) completion core.weechat 0\n");

    LONGS_EQUAL(44, weechat_relay_cmd_completion (&relay_session, "my_id",
                                                  "core.weechat",
                                                  -1,
                                                  "/help fi"));
    RELAY_CHECK_RECV("(my_id) completion core.weechat -1 /help fi\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_sync
 */

TEST(LibCommand, Sync)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_sync (NULL, NULL, NULL, NULL));

    LONGS_EQUAL(5, weechat_relay_cmd_sync (&relay_session, NULL, NULL, NULL));
    RELAY_CHECK_RECV("sync\n");

    LONGS_EQUAL(13, weechat_relay_cmd_sync (&relay_session, "my_id",
                                            NULL, NULL));
    RELAY_CHECK_RECV("(my_id) sync\n");

    LONGS_EQUAL(35, weechat_relay_cmd_sync (&relay_session, "my_id",
                                            "irc.freenode.#weechat", NULL));
    RELAY_CHECK_RECV("(my_id) sync irc.freenode.#weechat\n");

    LONGS_EQUAL(42, weechat_relay_cmd_sync (&relay_session, "my_id",
                                            "irc.freenode.#weechat",
                                            "buffer"));
    RELAY_CHECK_RECV("(my_id) sync irc.freenode.#weechat buffer\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_desync
 */

TEST(LibCommand, Desync)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_desync (NULL, NULL, NULL, NULL));

    LONGS_EQUAL(7, weechat_relay_cmd_desync (&relay_session, NULL, NULL, NULL));
    RELAY_CHECK_RECV("desync\n");

    LONGS_EQUAL(15, weechat_relay_cmd_desync (&relay_session, "my_id",
                                             NULL, NULL));
    RELAY_CHECK_RECV("(my_id) desync\n");

    LONGS_EQUAL(37, weechat_relay_cmd_desync (&relay_session, "my_id",
                                              "irc.freenode.#weechat", NULL));
    RELAY_CHECK_RECV("(my_id) desync irc.freenode.#weechat\n");

    LONGS_EQUAL(44, weechat_relay_cmd_desync (&relay_session, "my_id",
                                              "irc.freenode.#weechat",
                                              "buffer"));
    RELAY_CHECK_RECV("(my_id) desync irc.freenode.#weechat buffer\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_test
 */

TEST(LibCommand, Test)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_test (NULL, NULL));

    LONGS_EQUAL(5, weechat_relay_cmd_test (&relay_session, NULL));
    RELAY_CHECK_RECV("test\n");

    LONGS_EQUAL(13, weechat_relay_cmd_test (&relay_session, "my_id"));
    RELAY_CHECK_RECV("(my_id) test\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_ping
 */

TEST(LibCommand, Ping)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_ping (NULL, NULL, NULL));

    LONGS_EQUAL(5, weechat_relay_cmd_ping (&relay_session, NULL, NULL));
    RELAY_CHECK_RECV("ping\n");

    LONGS_EQUAL(13, weechat_relay_cmd_ping (&relay_session, "my_id", NULL));
    RELAY_CHECK_RECV("(my_id) ping\n");

    LONGS_EQUAL(23, weechat_relay_cmd_ping (&relay_session, "my_id",
                                            "test args"));
    RELAY_CHECK_RECV("(my_id) ping test args\n");
}

/*
 * Tests functions:
 *   weechat_relay_cmd_quit
 */

TEST(LibCommand, Quit)
{
    LONGS_EQUAL(-1, weechat_relay_cmd_quit (NULL, NULL));

    LONGS_EQUAL(5, weechat_relay_cmd_quit (&relay_session, NULL));
    RELAY_CHECK_RECV("quit\n");

    LONGS_EQUAL(13, weechat_relay_cmd_quit (&relay_session, "my_id"));
    RELAY_CHECK_RECV("(my_id) quit\n");
}
