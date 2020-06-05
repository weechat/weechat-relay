/*
 * cli.c - command line tool to test WeeChat relay connection
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
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <gnutls/gnutls.h>
#include <readline.h>
#include <history.h>

#include "lib/weechat-relay.h"
#include "config-git.h"

#include "cli.h"
#include "message.h"
#include "network.h"
#include "util.h"


/* command line arguments */
const char *ptr_argv0;                 /* pointer to first argv             */
int relay_cli_debug = 0;               /* debug level                       */
int relay_cli_force_ipv4 = 0;          /* force IPv4 connection             */
int relay_cli_force_ipv6 = 0;          /* force IPv6 connection             */
int relay_cli_ssl = 0;                 /* use SSL/TLS                       */
const char *relay_cli_hostname = NULL; /* pointer to hostname argument      */
const char *relay_cli_port = NULL;     /* pointer to port argument          */
const char *relay_cli_commands[RELAY_CLI_MAX_COMMANDS];
                                       /* commands to send once connected   */
int relay_cli_num_commands = 0;        /* number of commands to send        */

/* other variables */
int relay_cli_quit = 0;                /* 1 to exit program                 */
struct t_weechat_relay_session *relay_cli_session = NULL; /* relay session  */


/*
 * Displays copyright on standard output.
 */

void
relay_cli_display_copyright ()
{
    printf ("\n");
    printf ("%s %s (%s) Copyright %s, compiled on %s %s\n"
            "Developed by Sébastien Helleu <flashcode@flashtux.org> "
            "- %s",
            PACKAGE_NAME,
            WEECHAT_RELAY_VERSION,
            WEECHAT_RELAY_VERSION_GIT,
            RELAY_CLI_COPYRIGHT_DATE,
            __DATE__,
            __TIME__,
            RELAY_CLI_WEB_SITE);
    printf ("\n");
}

/*
 * Displays license on standard output.
 */

void
relay_cli_display_license ()
{
    printf ("\n");
    printf ("%s", RELAY_CLI_LICENSE_TEXT);
    printf ("\n");
}

/*
 * Displays usage on standard output.
 */

void
relay_cli_display_usage ()
{
    relay_cli_display_copyright ();
    printf ("\n");
    printf ("Usage: %s [option...] hostname\n", ptr_argv0);
    printf ("\n");
    printf (
        "  -4, --ipv4         force connection with IPv4 (default: auto)\n"
        "  -6, --ipv6         force connection with IPv6 (default: auto)\n"
        "  -c, --command      send commands once connected "
        "(up to " RELAY_CLI_MAX_COMMANDS_STR " commands are allowed)\n"
        "  -d, --debug        debug mode: long objects view; "
        "with two -d: display raw messages\n"
        "  -h, --help         display help and exit\n"
        "  -l, --license      display license and exit\n"
        "  -p, --port <port>  the port to connect to "
        "(default: " RELAY_CLI_DEFAULT_PORT ")\n"
        "  -s, --ssl          use SSL/TLS\n"
        "  -v, --version      display version and exit\n"
        "  hostname           hostname with running WeeChat\n");
    printf ("\n");
}

/*
 * Displays an error and exits with code 1.
 */

void
relay_cli_display_arg_error (const char *error)
{
    fprintf (stderr, "ERROR: %s\n", error);
    relay_cli_display_usage ();
}

/*
 * Parses command line arguments.
 *
 * Arguments argc and argv come from main() function.
 *
 * Returns:
 *    1: OK, connection will be done
 *    0: OK, exit with return code 0
 *   -1: error in parsing of arguments, exit with return code 1
 */

int
relay_cli_parse_args (int argc, char *argv[])
{
    int rc, i, opt, long_index;
    struct option long_options[] = {
        { "ipv4",     no_argument,       NULL, '4' },
        { "ipv6",     no_argument,       NULL, '6' },
        { "command",  required_argument, NULL, 'c' },
        { "debug",    no_argument,       NULL, 'd' },
        { "help",     no_argument,       NULL, 'h' },
        { "license",  no_argument,       NULL, 'l' },
        { "port",     required_argument, NULL, 'p' },
        { "ssl",      no_argument,       NULL, 's' },
        { "version",  no_argument,       NULL, 'v' },
        { NULL,       0,                 NULL, 0   },
    };

    rc = 1;

    ptr_argv0 = argv[0];
    relay_cli_debug = 0;
    relay_cli_force_ipv4 = 0;
    relay_cli_force_ipv6 = 0;
    relay_cli_ssl = 0;
    relay_cli_hostname = NULL;
    relay_cli_port = NULL;
    for (i = 0; i < RELAY_CLI_MAX_COMMANDS; i++)
    {
        relay_cli_commands[i] = NULL;
    }
    relay_cli_num_commands = 0;

    long_index = 0;

    while ((opt = getopt_long (argc, argv, "46c:dhlp:sv",
                               long_options, &long_index)) != -1)
    {
        switch (opt)
        {
            case '4':
                relay_cli_force_ipv4 = 1;
                break;
            case '6':
                relay_cli_force_ipv6 = 1;
                break;
            case 'c':
                if (relay_cli_num_commands == RELAY_CLI_MAX_COMMANDS)
                {
                    relay_cli_display_arg_error ("too many commands");
                    rc = -1;
                }
                else
                {
                    relay_cli_commands[relay_cli_num_commands++] = optarg;
                }
                break;
            case 'd':
                relay_cli_debug++;
                break;
            case 'h':
                relay_cli_display_usage ();
                rc = 0;
                break;
            case 'l':
                relay_cli_display_copyright ();
                relay_cli_display_license ();
                rc = 0;
                break;
            case 'p':
                relay_cli_port = optarg;
                break;
            case 's':
                relay_cli_ssl = 1;
                break;
            case 'v':
                printf ("%s (%s)\n",
                        WEECHAT_RELAY_VERSION, WEECHAT_RELAY_VERSION_GIT);
                rc = 0;
                break;
            default:
                relay_cli_display_usage ();
                rc = -1;
                break;
        }
    }

    if ((rc == 1) && relay_cli_force_ipv4 && relay_cli_force_ipv6)
    {
        relay_cli_display_arg_error ("ipv4 and ipv6 are incompatible options");
        rc = -1;
    }

    if ((rc == 1) && (optind >= argc))
    {
        relay_cli_display_arg_error ("missing hostname");
        rc = -1;
    }

    if (rc == 1)
        relay_cli_hostname = argv[optind];

    return rc;
}

/*
 * Displays hexadecimal dump of a message.
 */

void
relay_cli_display_hex_dump (const void *buffer, size_t size)
{
    char *str_dump;

    if (!buffer || (size == 0))
        return;

    str_dump = string_hex_dump (buffer, size, 16, "      ", NULL);
    if (str_dump)
    {
        printf ("%s\n", str_dump);
        free (str_dump);
    }
}

/*
 * Displays an incoming message.
 */

void
relay_cli_display_message (const void *buffer, size_t size)
{
    printf ("\r--> %ld bytes received\n", size);
    if (relay_cli_debug >= 2)
        relay_cli_display_hex_dump (buffer, size);
    relay_message_display (buffer, size);
    rl_forced_update_display ();
}

/*
 * Sends a command to WeeChat.
 *
 * Returns number of bytes sent, -1 if error.
 */

ssize_t
relay_cli_send_command (const char *command)
{
    char *buffer;
    size_t size;
    ssize_t num_sent;

    if (!command)
        return -1;

    size = strlen (command) + 1 + 1;
    buffer = malloc (size);
    if (!buffer)
        return -1;

    printf ("<-- %s", command);
    snprintf (buffer, size, "%s\n", command);

    num_sent = weechat_relay_session_send (relay_cli_session,
                                           buffer, strlen (buffer));

    if (num_sent < 0)
    {
        printf ("\r\n");
        if (relay_cli_ssl)
            gnutls_perror (num_sent);
        else
            perror ("ERROR: send");
    }
    else
    {
        if (relay_cli_debug)
            printf(" (%ld bytes)", num_sent);
        printf ("\n");
        if (relay_cli_debug >= 2)
            relay_cli_display_hex_dump (buffer, strlen (buffer));
    }

    if (strcmp (command, "quit") == 0)
        relay_cli_quit = 1;

    free (buffer);

    return num_sent;
}

/*
 * Checks if bytes are pending (ready to read on the socket).
 */

int
relay_cli_check_pending ()
{
    if (!relay_cli_ssl)
        return 0;

    return (gnutls_record_check_pending (
                *((gnutls_session_t *)relay_cli_session->gnutls_sess))) ?
        1 : 0;
}

/*
 * Receives a message from WeeChat.
 *
 * Returns number of bytes received, -1 if error.
 */

ssize_t
relay_cli_recv_message ()
{
    ssize_t num_recv;
    char buffer_recv[4096];
    void *buffer;
    size_t buffer_size;

    num_recv = weechat_relay_session_recv (relay_cli_session,
                                           buffer_recv, sizeof (buffer_recv));

    if (num_recv < 0)
    {
        printf ("\r\n");
        if (relay_cli_ssl)
            gnutls_perror (num_recv);
        else
            perror ("ERROR: recv");
        relay_cli_quit = 1;
    }
    else if (num_recv == 0)
    {
        printf ("\r\nDisconnected\n");
        relay_cli_quit = 1;
    }
    else
    {
        /* add bytes to buffer */
        weechat_relay_session_buffer_add_bytes (relay_cli_session,
                                                buffer_recv, num_recv);
        /* pop messages and display them */
        while (1)
        {
            weechat_relay_session_buffer_pop (relay_cli_session,
                                              &buffer, &buffer_size);
            if (!buffer)
                break;
            relay_cli_display_message (buffer, buffer_size);
            free (buffer);
        }
    }

    return num_recv;
}

/*
 * Readline line handler.
 */

void
relay_cli_line_handler (char *line)
{
    if (!line)
    {
        printf ("\r\n");
        relay_cli_send_command ("quit");
        rl_callback_handler_remove ();
        relay_cli_quit = 1;
    }
    else if (line && line[0])
    {
        add_history (line);
        relay_cli_send_command (line);
    }

    if (line)
        free (line);
}

/*
 * Main loop for command-line interface.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
relay_cli_main_loop ()
{
    int i,sock, stdin_fd, max_fd, rc;
    char *line;
    fd_set read_fds;
    struct timeval tv;
    gnutls_session_t gnutls_sess, *ptr_gnutls_sess;

    relay_cli_quit = 0;

    /* connect to WeeChat */
    ptr_gnutls_sess = (relay_cli_ssl) ? &gnutls_sess : NULL;
    sock = relay_network_connect (
        relay_cli_hostname,
        (relay_cli_port) ? relay_cli_port : RELAY_CLI_DEFAULT_PORT,
        relay_cli_force_ipv4,
        relay_cli_force_ipv6,
        ptr_gnutls_sess);
    if (sock < 0)
        return 0;

    /* initialize relay session */
    relay_cli_session = weechat_relay_session_init (sock, ptr_gnutls_sess);
    if (!relay_cli_session)
    {
        fprintf (stderr, "ERROR: unable to initialize relay session\n");
        return 0;
    }

    rl_callback_handler_install ("weechat-relay> ", relay_cli_line_handler);

    stdin_fd = fileno (stdin);

    /* send command(s) received with option -c/--command */
    if (relay_cli_num_commands > 0)
    {
        printf ("\r");
        for (i = 0; i < relay_cli_num_commands; i++)
        {
            line = strdup (relay_cli_commands[i]);
            relay_cli_line_handler (line);
        }
        rl_forced_update_display ();
    }

    /* main loop */
    while (!relay_cli_quit)
    {
        FD_ZERO(&read_fds);
        FD_SET(stdin_fd, &read_fds);
        FD_SET(sock, &read_fds);
        max_fd = (sock > stdin_fd) ? sock : stdin_fd;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        rc = select (max_fd + 1, &read_fds, NULL, NULL, &tv);
        if (rc < 0)
        {
            printf ("\r\n");
            perror ("ERROR: select");
        }
        else
        {
            /* read data from keyboard */
            if (FD_ISSET(stdin_fd, &read_fds))
                rl_callback_read_char ();

            /* read socket: message from WeeChat */
            if (FD_ISSET(sock, &read_fds))
            {
                do
                {
                    relay_cli_recv_message ();
                }
                while (relay_cli_check_pending ());
            }
        }
    }

    relay_network_disconnect ((relay_cli_ssl) ? &gnutls_sess : NULL);

    rl_callback_handler_remove ();

    weechat_relay_session_free (relay_cli_session);

    return 1;
}

/*
 * Main function.
 */

int
relay_cli_main (int argc, char *argv[])
{
    int rc;

    relay_network_init ();

    rc = relay_cli_parse_args (argc, argv);
    if (rc < 1)
        rc = (rc == 0) ? 1 : 0;
    else
        rc = relay_cli_main_loop ();

    relay_network_end ();

    return (rc) ? 0 : 1;
}
