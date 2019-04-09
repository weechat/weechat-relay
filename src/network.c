/*
 * relay-network.c - network functions for WeeChat relay command line tool
 *
 * Copyright (C) 2019 Sébastien Helleu <flashcode@flashtux.org>
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
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>


/*
 * Connection to WeeChat.
 */

void
relay_network_connect (const char *hostname, int port, int use_ipv6)
{
    int sock;
    struct hostent* host;
    struct sockaddr_in addr;

    /* TODO: implement connection with IPv6 */
    (void) use_ipv6;

    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf ("ERROR: failed to create socket\n");
        exit (EXIT_FAILURE);
    }

    if ((host = gethostbyname(hostname)) == NULL)
    {
        //gethostbyname failed
        printf ("Failed to resolve %s\n", hostname);
        exit(EXIT_FAILURE);
    }
    addr.sin_addr.s_addr = *(long*) host->h_addr_list[0];
    addr.sin_family = AF_INET;
    addr.sin_port = htons (port);

    if (connect (sock, (struct sockaddr *)&addr, sizeof (addr)) < 0)
    {
        close (sock);
        printf ("ERROR: connection failed on %s port %d: %s\n",
                hostname, port, strerror (errno));
        exit (EXIT_FAILURE);
    }
    printf ("Connected to %s\n", hostname);

    close (sock);
}
