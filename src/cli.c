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

#include "lib/weechat-relay.h"
#include "config-git.h"

#include "cli.h"
#include "network.h"


const char *ptr_argv0;                 /* pointer to first argv             */
int relay_debug = 0;                   /* debug level                       */
int relay_use_ipv6 = 0;                /* IPv6 connection                   */
int relay_port = 9000;                 /* port to connect to                */
const char *relay_ptr_hostname = NULL; /* pointer to hostname argument      */


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
    printf ("  -6, --ipv6         connect using IPv6\n"
            "  -d, --debug        debug mode: long objects view; "
            "with two -d: display raw messages\n"
            "  -h, --help         display help and exit\n"
            "  -l, --license      display license and exit\n"
            "  -p, --port <port>  the port to connect to "
            "(default: 9000)\n"
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
    char *error;
    struct option long_options[] = {
        { "ipv6",    no_argument,       NULL, '6' },
        { "debug",   no_argument,       NULL, 'd' },
        { "help",    no_argument,       NULL, 'h' },
        { "license", no_argument,       NULL, 'l' },
        { "port",    required_argument, NULL, 'p' },
        { "version", no_argument,       NULL, 'v' },
        { NULL,      0,                 NULL, 0   },
    };

    ptr_argv0 = argv[0];
    long_index = 0;

    while ((opt = getopt_long (argc, argv, "6dhlp:v",
                               long_options, &long_index)) != -1)
    {
        switch (opt)
        {
            case '6':
                relay_use_ipv6 = 1;
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
                error = NULL;
                relay_port = (int)strtol (optarg, &error, 10);
                if (!error || error[0])
                    exit_arg_error ("wrong value for port");
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

    if (optind >= argc)
        exit_arg_error ("missing hostname");

    relay_ptr_hostname = argv[optind];
}

/*
 * Main function.
 */

int
main (int argc, char *argv[])
{
    relay_parse_args (argc, argv);
    relay_network_connect (relay_ptr_hostname, relay_port, relay_use_ipv6);
    return 0;
}
