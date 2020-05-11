/*
 * relay-network.c - network functions for WeeChat relay command line tool
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
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>

#include <gnutls/gnutls.h>


gnutls_certificate_credentials_t gnutls_xcred;


/*
 * Initialized network.
 */

void
relay_network_init ()
{
    gnutls_global_init ();
    gnutls_certificate_allocate_credentials (&gnutls_xcred);
}

/*
 * Connection to WeeChat.
 *
 * Returns socket number, -1 if error.
 */

int
relay_network_connect (const char *hostname, const char *port,
                       int force_ipv4, int force_ipv6,
                       gnutls_session_t *gnutls_sess)
{
    struct addrinfo hints, *res_remote, *ptr_res;
    char str_ip[NI_MAXHOST];
    int rc, sock, flags;
    gnutls_anon_client_credentials_t anoncred;

    res_remote = NULL;
    sock = -1;

    if (gnutls_sess)
    {
        gnutls_init (gnutls_sess, GNUTLS_CLIENT);
        gnutls_anon_allocate_client_credentials (&anoncred);
    }

    res_init ();
    memset (&hints, 0, sizeof (hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = (force_ipv4) ?
        AF_INET : ((force_ipv6) ? AF_INET6 : AF_UNSPEC);
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    res_remote = NULL;
    rc = getaddrinfo (hostname, port, &hints, &res_remote);
    if ((rc != 0) || !res_remote)
        goto error;

    for (ptr_res = res_remote; ptr_res; ptr_res = ptr_res->ai_next)
    {
        sock = socket (ptr_res->ai_family, ptr_res->ai_socktype,
                       ptr_res->ai_protocol);
        if (sock >= 0)
            break;
    }

    if (!ptr_res || (sock < 0))
        goto error;

    /* set TCP_NODELAY to send data immediately */
    flags = 1;
    setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof (flags));

    if (connect (sock, ptr_res->ai_addr, ptr_res->ai_addrlen) != 0)
        goto error;

    if (getnameinfo (res_remote->ai_addr, res_remote->ai_addrlen,
                     str_ip, sizeof (str_ip), NULL, 0, NI_NUMERICHOST) != 0)
    {
        goto error;
    }

    if (gnutls_sess)
    {
        gnutls_set_default_priority (*gnutls_sess);
        gnutls_credentials_set (*gnutls_sess, GNUTLS_CRD_CERTIFICATE,
                                gnutls_xcred);
        gnutls_transport_set_int (*gnutls_sess, sock);
        do {
            rc = gnutls_handshake (*gnutls_sess);
        }
        while ((rc < 0) && (gnutls_error_is_fatal (rc) == 0));
        if (rc < 0)
        {
            gnutls_perror (rc);
            goto error2;
        }
    }

    printf ("Connected to %s%s\n",
            str_ip,
            (gnutls_sess) ? " (SSL)" : "");

    freeaddrinfo (res_remote);

    return sock;

error:
    perror ("ERROR: connection");
error2:
    if (gnutls_sess)
        gnutls_deinit (*gnutls_sess);
    if (res_remote)
        freeaddrinfo(res_remote);
    if (sock >= 0)
        close (sock);
    return -1;
}

/*
 * Disconnects from WeeChat
 */

void
relay_network_disconnect (gnutls_session_t *gnutls_sess)
{
    if (gnutls_sess)
    {
        gnutls_bye (*gnutls_sess, GNUTLS_SHUT_RDWR);
        gnutls_deinit (*gnutls_sess);
    }
}

/*
 * Ends network.
 */

void
relay_network_end ()
{
    gnutls_certificate_free_credentials (gnutls_xcred);
    gnutls_global_deinit ();
}
