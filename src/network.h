/*
 * Copyright (C) 2019-2021 Sébastien Helleu <flashcode@flashtux.org>
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

#ifndef RELAY_CLI_NETWORK_H
#define RELAY_CLI_NETWORK_H

#include <gnutls/gnutls.h>

extern void relay_network_init ();
extern int relay_network_connect (const char *hostname, const char *port,
                                  int force_ipv4, int force_ipv6,
                                  gnutls_session_t *gnutls_sess);
extern void relay_network_disconnect (gnutls_session_t *gnutls_sess);
extern void relay_network_end ();

#endif /* RELAY_CLI_NETWORK_H */
