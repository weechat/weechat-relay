/*
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

#ifndef WEECHAT_RELAY_H
#define WEECHAT_RELAY_H

#include <gnutls/gnutls.h>

/* WeeChat Relay version */

#define WEECHAT_RELAY_VERSION "@VERSION@"

#define WEECHAT_RELAY_VERSION_MAJOR @VERSION_MAJOR@
#define WEECHAT_RELAY_VERSION_MINOR @VERSION_MINOR@
#define WEECHAT_RELAY_VERSION_PATCH @VERSION_PATCH@

#define WEECHAT_RELAY_VERSION_NUMBER @VERSION_NUMBER@

/* Compression */

#define WEECHAT_RELAY_COMPRESSION_OFF  "off"
#define WEECHAT_RELAY_COMPRESSION_ZLIB "zlib"

/* Relay sessions */

struct t_weechat_relay_session
{
    int sock;                          /* socket for I/O with WeeChat       */
    int ssl;                           /* connection with SSL               */
    gnutls_session_t *gnutls_sess;     /* gnutls session (only if SSL)      */
};

extern struct t_weechat_relay_session *weechat_relay_session_init (int sock,
                                                                   gnutls_session_t *gnutls_sess);
extern void weechat_relay_session_free (struct t_weechat_relay_session *session);

/* Relay commands (client -> WeeChat) */

extern int weechat_relay_cmd_raw (struct t_weechat_relay_session *session,
                                  const char *buffer,
                                  size_t size);
extern int weechat_relay_cmd (struct t_weechat_relay_session *session,
                              const char *msg_id,
                              const char *command,
                              const char *arguments[]);
extern int weechat_relay_cmd_init (struct t_weechat_relay_session *session,
                                   const char *password,
                                   const char *compression);
extern int weechat_relay_cmd_hdata (struct t_weechat_relay_session *session,
                                    const char *msg_id,
                                    const char *path,
                                    const char *keys);
extern int weechat_relay_cmd_info (struct t_weechat_relay_session *session,
                                   const char *msg_id,
                                   const char *name);
extern int weechat_relay_cmd_infolist (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *name,
                                       const char *pointer,
                                       const char *arguments);
extern int weechat_relay_cmd_nicklist (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *buffer);
extern int weechat_relay_cmd_input (struct t_weechat_relay_session *session,
                                    const char *buffer,
                                    const char *data);
extern int weechat_relay_cmd_sync (struct t_weechat_relay_session *session,
                                   const char *buffers,
                                   const char *options);
extern int weechat_relay_cmd_desync (struct t_weechat_relay_session *session,
                                     const char *buffers,
                                     const char *options);
extern int weechat_relay_cmd_test (struct t_weechat_relay_session *session);
extern int weechat_relay_cmd_ping (struct t_weechat_relay_session *session,
                                   const char *arguments);
extern int weechat_relay_cmd_quit (struct t_weechat_relay_session *session);

#endif /* WEECHAT_RELAY_H */
