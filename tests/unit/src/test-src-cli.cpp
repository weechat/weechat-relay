/*
 * test-src-cli.cpp - test command-line interface
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
#include "lib/weechat-relay.h"
#include "src/cli.h"

extern int relay_cli_debug;
extern int relay_cli_force_ipv4;
extern int relay_cli_force_ipv6;
extern int relay_cli_ssl;
extern const char *relay_cli_hostname;
extern const char *relay_cli_port;
extern struct t_weechat_relay_session *relay_cli_session;

extern void relay_cli_display_copyright ();
extern void relay_cli_display_license ();
extern void relay_cli_display_usage ();
extern void relay_cli_display_arg_error (const char *error);
extern int relay_cli_parse_args (int argc, char *argv[]);
extern void relay_cli_display_hex_dump (const void *buffer, size_t size);
extern ssize_t relay_cli_send_command (const char *command);
extern ssize_t relay_cli_recv_message ();
extern void relay_cli_line_handler (char *line);
}

#define DEBUG_0            0
#define DEBUG_1            1
#define DEBUG_2            2
#define IPV4_OFF           0
#define IPV4_ON            1
#define IPV6_OFF           0
#define IPV6_ON            1
#define SSL_OFF            0
#define SSL_ON             1
#define HOSTNAME_NULL      NULL
#define HOSTNAME_LOCALHOST "localhost"
#define PORT_NULL          NULL
#define PORT_15000         "15000"

#define RELAY_CHECK_CLI_ARGS(__result, __debug, __ipv4, __ipv6, __ssl,  \
                             __hostname, __port)                        \
    optind = 1;                                                         \
    argc = 0;                                                           \
    while (argv[argc])                                                  \
    {                                                                   \
        argc++;                                                         \
    }                                                                   \
    LONGS_EQUAL(__result, relay_cli_parse_args (argc, (char **)argv));  \
    LONGS_EQUAL(__debug, relay_cli_debug);                              \
    LONGS_EQUAL(__ipv4, relay_cli_force_ipv4);                          \
    LONGS_EQUAL(__ipv6, relay_cli_force_ipv6);                          \
    LONGS_EQUAL(__ssl, relay_cli_ssl);                                  \
    if (__hostname == NULL)                                             \
    {                                                                   \
        POINTERS_EQUAL(NULL, relay_cli_hostname);                       \
    }                                                                   \
    else                                                                \
    {                                                                   \
        STRCMP_EQUAL(__hostname, relay_cli_hostname);                   \
    }                                                                   \
    if (__port == NULL)                                                 \
    {                                                                   \
        POINTERS_EQUAL(NULL, relay_cli_port);                           \
    }                                                                   \
    else                                                                \
    {                                                                   \
        STRCMP_EQUAL(__port, relay_cli_port);                           \
    }

#define RELAY_CHECK_RECV(__command)                                     \
    num_read = read (fd_pipe[0],                                        \
                     read_buffer, sizeof (read_buffer) - 1);            \
    read_buffer[num_read] = '\0';                                       \
    STRCMP_EQUAL(__command, read_buffer);                               \
    LONGS_EQUAL(strlen (__command), num_read);                          \
    STRCMP_EQUAL(__command, read_buffer);

TEST_GROUP(SrcCli)
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
 *   relay_cli_display_copyright
 */

TEST(SrcCli, DisplayCopyright)
{
    relay_cli_display_copyright ();
}

/*
 * Tests functions:
 *   relay_cli_display_license
 */

TEST(SrcCli, DisplayLicense)
{
    relay_cli_display_license ();
}

/*
 * Tests functions:
 *   relay_cli_display_usage
 */

TEST(SrcCli, DisplayUsage)
{
    relay_cli_display_usage ();
}

/*
 * Tests functions:
 *   relay_cli_display_arg_error
 */

TEST(SrcCli, DisplayArgError)
{
    relay_cli_display_arg_error (NULL);
    relay_cli_display_arg_error ("");
    relay_cli_display_arg_error ("this is a fatal error!");
}

/*
 * Tests functions:
 *   relay_cli_parse_args
 */

TEST(SrcCli, ParseArgs)
{
    const char *argv[16];
    int argc;

    argv[0] = "weechat-relay-cli";

    /* missing hostname */
    argv[1] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* unknown option */
    argv[1] = "-Z";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);
    argv[1] = "--unknown-option";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* missing port */
    argv[1] = "localhost";
    argv[2] = "-p";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);
    argv[1] = "localhost";
    argv[2] = "--port";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* incompatible flags: ipv4 and ipv6 */
    argv[1] = "-4";
    argv[2] = "-6";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_ON, IPV6_ON, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);
    argv[1] = "--ipv4";
    argv[2] = "--ipv6";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(-1, DEBUG_0, IPV4_ON, IPV6_ON, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* OK */
    argv[1] = "localhost";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_LOCALHOST, PORT_NULL);

    /* OK: help */
    argv[1] = "-h";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(0, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);
    argv[1] = "--help";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(0, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* OK: license */
    argv[1] = "-l";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(0, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);
    argv[1] = "--license";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(0, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* OK: version */
    argv[1] = "-v";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(0, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);
    argv[1] = "--version";
    argv[2] = NULL;
    RELAY_CHECK_CLI_ARGS(0, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_NULL, PORT_NULL);

    /* OK with debug (1) */
    argv[1] = "-d";
    argv[2] = "localhost";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_1, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_LOCALHOST, PORT_NULL);
    argv[1] = "--debug";
    argv[2] = "localhost";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_1, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_LOCALHOST, PORT_NULL);

    /* OK with debug (2) */
    argv[1] = "-d";
    argv[2] = "-d";
    argv[3] = "localhost";
    argv[4] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_2, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_LOCALHOST, PORT_NULL);
    argv[1] = "--debug";
    argv[2] = "--debug";
    argv[3] = "localhost";
    argv[4] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_2, IPV4_OFF, IPV6_OFF, SSL_OFF,
                         HOSTNAME_LOCALHOST, PORT_NULL);

    /* OK with SSL */
    argv[1] = "-s";
    argv[2] = "localhost";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_NULL);
    argv[1] = "--ssl";
    argv[2] = "localhost";
    argv[3] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_OFF, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_NULL);

    /* OK with SSL + IPv4 */
    argv[1] = "-s";
    argv[2] = "-4";
    argv[3] = "localhost";
    argv[4] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_ON, IPV6_OFF, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_NULL);
    argv[1] = "--ssl";
    argv[2] = "--ipv4";
    argv[3] = "localhost";
    argv[4] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_ON, IPV6_OFF, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_NULL);

    /* OK with SSL + IPv6 */
    argv[1] = "-s";
    argv[2] = "-6";
    argv[3] = "localhost";
    argv[4] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_ON, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_NULL);
    argv[1] = "--ssl";
    argv[2] = "--ipv6";
    argv[3] = "localhost";
    argv[4] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_ON, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_NULL);

    /* OK with port + SSL + IPv6 */
    argv[1] = "-s";
    argv[2] = "-p";
    argv[3] = "15000";
    argv[4] = "-6";
    argv[5] = "localhost";
    argv[6] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_ON, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_15000);
    argv[1] = "--ssl";
    argv[2] = "--port";
    argv[3] = "15000";
    argv[4] = "--ipv6";
    argv[5] = "localhost";
    argv[6] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_0, IPV4_OFF, IPV6_ON, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_15000);

    /* OK with debug (2) + port + SSL + IPv6 */
    argv[1] = "-d";
    argv[2] = "-d";
    argv[3] = "-s";
    argv[4] = "-p";
    argv[5] = "15000";
    argv[6] = "-6";
    argv[7] = "localhost";
    argv[8] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_2, IPV4_OFF, IPV6_ON, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_15000);
    argv[1] = "--debug";
    argv[2] = "--debug";
    argv[3] = "--ssl";
    argv[4] = "--port";
    argv[5] = "15000";
    argv[6] = "--ipv6";
    argv[7] = "localhost";
    argv[8] = NULL;
    RELAY_CHECK_CLI_ARGS(1, DEBUG_2, IPV4_OFF, IPV6_ON, SSL_ON,
                         HOSTNAME_LOCALHOST, PORT_15000);
}

/*
 * Tests functions:
 *   relay_cli_display_hex_dump
 */

TEST(SrcCli, DisplayHexDump)
{
    const char *string = "abcdefghijklmnopqrstuvwzyz";

    relay_cli_display_hex_dump (NULL, 0);
    relay_cli_display_hex_dump (string, 0);
    relay_cli_display_hex_dump (string, strlen (string));
}

/*
 * Tests functions:
 *   relay_cli_send_command
 */

TEST(SrcCli, SendCommand)
{
    relay_cli_session = &relay_session;
    relay_cli_debug = 1;

    LONGS_EQUAL(-1, relay_cli_send_command (NULL));

    /* failed: invalid socket */
    relay_session.sock = -1;
    LONGS_EQUAL(-1, relay_cli_send_command ("test"));
    relay_session.sock = fd_pipe[1];

    /* success: "test\n" sent */
    LONGS_EQUAL(5, relay_cli_send_command ("test"));
    RELAY_CHECK_RECV("test\n");
}

/*
 * Tests functions:
 *   relay_cli_recv_message
 */

TEST(SrcCli, RecvMessage)
{
    relay_cli_session = &relay_session;

    /* failed: invalid socket */
    relay_session.sock = -1;
    LONGS_EQUAL(-1, relay_cli_recv_message());
    relay_session.sock = fd_pipe[1];

    /* success: "test\n" received */
    LONGS_EQUAL(5, relay_cli_send_command ("test"));
    relay_session.sock = fd_pipe[0];
    LONGS_EQUAL(5, relay_cli_recv_message());
    relay_session.sock = fd_pipe[1];
}

/*
 * Tests functions:
 *   relay_cli_line_handler
 */

TEST(SrcCli, LineHandler)
{
    char *line;

    relay_cli_session = &relay_session;

    /* "quit\n" is sent when line is NULL */
    relay_cli_line_handler (NULL);
    relay_session.sock = fd_pipe[0];
    LONGS_EQUAL(5, relay_cli_recv_message());
    relay_session.sock = fd_pipe[1];

    line = strdup ("abc");
    relay_cli_line_handler (line);
    relay_session.sock = fd_pipe[0];
    LONGS_EQUAL(4, relay_cli_recv_message());
    relay_session.sock = fd_pipe[1];
}

/*
 * Tests functions:
 *   relay_cli_main_loop
 */

TEST(SrcCli, MainLoop)
{
}

/*
 * Tests functions:
 *   relay_cli_main
 */

TEST(SrcCli, Main)
{
    const char *argv[16];

    argv[0] = "weechat-relay-cli";

    /* missing hostname */
    argv[1] = NULL;
    LONGS_EQUAL(1, relay_cli_main (1, (char **)argv));

    /* help */
    argv[1] = "--help";
    argv[2] = NULL;
    LONGS_EQUAL(0, relay_cli_main (2, (char **)argv));
}
