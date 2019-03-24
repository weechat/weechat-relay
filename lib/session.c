/*
 * session.c - session functions of WeeChat Relay lib
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

#include "weechat-relay.h"


/*
 * Initializes a relay session.
 *
 * Note: the session must be freed by weechat_relay_session_free.
 *
 * Returns the pointer to the new session, NULL if error.
 *
 */

struct t_weechat_relay_session *
weechat_relay_session_init (int sock, gnutls_session_t *gnutls_sess)
{
    struct t_weechat_relay_session *session;

    session = malloc (sizeof (*session));
    if (!session)
        return NULL;

    session->sock = sock;
    session->ssl = (gnutls_sess != NULL) ? 1 : 0;
    session->gnutls_sess = gnutls_sess;

    return session;
}

/*
 * Frees a relay session.
 */

void
weechat_relay_session_free (struct t_weechat_relay_session *session)
{
    free (session);
}
