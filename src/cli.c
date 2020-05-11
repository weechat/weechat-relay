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
#include "network.h"


/* command line arguments */
const char *ptr_argv0;                 /* pointer to first argv             */
int relay_debug = 0;                   /* debug level                       */
int relay_force_ipv4 = 0;              /* force IPv4 connection             */
int relay_force_ipv6 = 0;              /* force IPv6 connection             */
int relay_ssl = 0;                     /* use SSL/TLS                       */
const char *relay_hostname = NULL;     /* pointer to hostname argument      */
const char *relay_port = NULL;         /* pointer to port argument          */

/* other variables */
int relay_quit = 0;                    /* 1 to exit program                 */
int relay_socket = -1;                 /* socket for I/O with WeeChat       */
gnutls_session_t gnutls_sess;          /* GnuTLS session                    */


/*
 * Displays copyright on standard output.
 */

void
display_copyright ()
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
display_license ()
{
    printf ("\n");
    printf ("%s", RELAY_CLI_LICENSE_TEXT);
    printf ("\n");
}

/*
 * Displays usage on standard output.
 */

void
display_usage ()
{
    display_copyright ();
    printf ("\n");
    printf ("Usage: %s [option...] hostname\n", ptr_argv0);
    printf ("\n");
    printf (
        "  -4, --ipv4         force connection with IPv4 (default: auto)\n"
        "  -6, --ipv6         force connection with IPv6 (default: auto)\n"
        "  -s, --ssl          use SSL/TLS\n"
        "  -d, --debug        debug mode: long objects view; "
        "with two -d: display raw messages\n"
        "  -h, --help         display help and exit\n"
        "  -l, --license      display license and exit\n"
        "  -p, --port <port>  the port to connect to "
        "(default: " RELAY_CLI_DEFAULT_PORT ")\n"
        "  -v, --version      display version and exit\n"
        "  hostname           hostname with running WeeChat\n");
    printf ("\n");
}

/*
 * Displays an error and exits with code 1.
 */

void
exit_arg_error (const char *error)
{
    printf ("ERROR: %s\n", error);
    display_usage ();
    exit (EXIT_FAILURE);
}

/*
 * Parses command line arguments.
 *
 * Arguments argc and argv come from main() function.
 */

void
relay_parse_args (int argc, char *argv[])
{
    int opt, long_index;
    struct option long_options[] = {
        { "ipv4",     no_argument,       NULL, '4' },
        { "ipv6",     no_argument,       NULL, '6' },
        { "ssl",      no_argument,       NULL, 's' },
        { "debug",    no_argument,       NULL, 'd' },
        { "help",     no_argument,       NULL, 'h' },
        { "license",  no_argument,       NULL, 'l' },
        { "port",     required_argument, NULL, 'p' },
        { "version",  no_argument,       NULL, 'v' },
        { NULL,       0,                 NULL, 0   },
    };

    ptr_argv0 = argv[0];
    relay_debug = 0;
    relay_force_ipv4 = 0;
    relay_force_ipv6 = 0;
    relay_ssl = 0;
    relay_hostname = NULL;
    relay_port = NULL;
    long_index = 0;

    while ((opt = getopt_long (argc, argv, "46sdhlp:v",
                               long_options, &long_index)) != -1)
    {
        switch (opt)
        {
            case '4':
                relay_force_ipv4 = 1;
                break;
            case '6':
                relay_force_ipv6 = 1;
                break;
            case 's':
                relay_ssl = 1;
                break;
            case 'd':
                relay_debug++;
                break;
            case 'h':
                display_usage ();
                exit (0);
                break;
            case 'l':
                display_copyright ();
                display_license ();
                exit (0);
                break;
            case 'p':
                relay_port = optarg;
                break;
            case 'v':
                printf ("%s (%s)\n",
                        WEECHAT_RELAY_VERSION, WEECHAT_RELAY_VERSION_GIT);
                exit (0);
                break;
            default:
                display_usage ();
                exit (0);
                break;
        }
    }

    if (relay_force_ipv4 && relay_force_ipv6)
        exit_arg_error ("ipv4 and ipv6 are incompatible options");

    if (optind >= argc)
        exit_arg_error ("missing hostname");

    relay_hostname = argv[optind];
}

/*
 * Sends a command to WeeChat.
 */

void
relay_send_command (const char *command)
{
    char *buffer;
    int size;
    ssize_t num_sent;

    size = strlen (command) + 1 + 1;
    buffer = malloc (size);
    if (!buffer)
        return;

    printf ("<-- %s ", command);
    snprintf (buffer, size, "%s\n", command);

    if (relay_ssl)
        num_sent = gnutls_record_send (gnutls_sess, buffer, size);
    else
        num_sent = send (relay_socket, buffer, size, 0);

    if (num_sent < 0)
    {
        printf ("\r\n");
        if (relay_ssl)
            gnutls_perror (num_sent);
        else
            perror ("ERROR: send");
    }
    else
    {
        if (relay_debug)
            printf("  (%ld bytes)", num_sent);
        printf ("\n");
    }

    if (strcmp (command, "quit") == 0)
        relay_quit = 1;

    free (buffer);
}

/*
 * Receives a message from WeeChat.
 */

void
relay_recv_message ()
{
    ssize_t num_recv;
    char buffer[4096];

    if (relay_ssl)
        num_recv = gnutls_record_recv (gnutls_sess, buffer, sizeof (buffer));
    else
        num_recv = recv (relay_socket, buffer, sizeof (buffer), 0);

    if (num_recv < 0)
    {
        printf ("\r\n");
        if (relay_ssl)
            gnutls_perror (num_recv);
        else
            perror ("ERROR: recv");
        relay_quit = 1;
    }
    else if (num_recv == 0)
    {
        printf ("\r\nDisconnected\n");
        relay_quit = 1;
    }
    else
    {
        printf ("\r--> %ld bytes received\n", num_recv);
        rl_forced_update_display ();
    }
}

/*
 * Readline line handler.
 */

void
relay_line_handler (char *line)
{
    if (!line)
    {
        printf ("\r\n");
        relay_send_command ("quit");
        rl_callback_handler_remove ();
        relay_quit = 1;
    }
    else if (line && line[0])
    {
        add_history (line);
        relay_send_command (line);
    }

    if (line)
        free (line);
}

/*
 * Main loop.
 */

void
relay_main_loop ()
{
    int stdin_fd, max_fd, rc;
    fd_set read_fds;
    struct timeval tv;

    relay_quit = 0;

    /* connect to WeeChat */
    relay_socket = relay_network_connect (
        relay_hostname,
        (relay_port) ? relay_port : RELAY_CLI_DEFAULT_PORT,
        relay_force_ipv4,
        relay_force_ipv6,
        (relay_ssl) ? &gnutls_sess : NULL);
    if (relay_socket < 0)
        return;

    rl_callback_handler_install ("weechat-relay> ", relay_line_handler);

    stdin_fd = fileno (stdin);

    /* main loop */
    while (!relay_quit)
    {
        FD_ZERO(&read_fds);
        FD_SET(stdin_fd, &read_fds);
        FD_SET(relay_socket, &read_fds);
        max_fd = (relay_socket > stdin_fd) ? relay_socket : stdin_fd;
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
            if (FD_ISSET(relay_socket, &read_fds))
                relay_recv_message ();
        }
    }

    relay_network_disconnect ((relay_ssl) ? &gnutls_sess : NULL);

    rl_callback_handler_remove ();
}

/*
 * Main function.
 */

int
main (int argc, char *argv[])
{
    relay_network_init ();
    relay_parse_args (argc, argv);
    relay_main_loop ();
    relay_network_end ();

    return 0;
}
