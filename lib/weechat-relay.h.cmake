/*
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

#ifndef WEECHAT_RELAY_H
#define WEECHAT_RELAY_H

/* WeeChat Relay version */

#define WEECHAT_RELAY_VERSION "@VERSION@"

#define WEECHAT_RELAY_VERSION_MAJOR @VERSION_MAJOR@
#define WEECHAT_RELAY_VERSION_MINOR @VERSION_MINOR@
#define WEECHAT_RELAY_VERSION_PATCH @VERSION_PATCH@

#define WEECHAT_RELAY_VERSION_NUMBER @VERSION_NUMBER@

/* Compression */

enum t_weechat_relay_compression
{
    WEECHAT_RELAY_COMPRESSION_OFF = 0, /* no compression of binary objects  */
    WEECHAT_RELAY_COMPRESSION_ZLIB,    /* zlib compression                  */
    /* number of compressions */
    WEECHAT_RELAY_NUM_COMPRESSIONS,
};

/* Messages */

#define WEECHAT_RELAY_MSG_INITIAL_ALLOC 4096

/* object ids in binary messages */
#define WEECHAT_RELAY_MSG_OBJ_CHAR      "chr"
#define WEECHAT_RELAY_MSG_OBJ_INT       "int"
#define WEECHAT_RELAY_MSG_OBJ_LONG      "lon"
#define WEECHAT_RELAY_MSG_OBJ_STRING    "str"
#define WEECHAT_RELAY_MSG_OBJ_BUFFER    "buf"
#define WEECHAT_RELAY_MSG_OBJ_POINTER   "ptr"
#define WEECHAT_RELAY_MSG_OBJ_TIME      "tim"
#define WEECHAT_RELAY_MSG_OBJ_HASHTABLE "htb"
#define WEECHAT_RELAY_MSG_OBJ_HDATA     "hda"
#define WEECHAT_RELAY_MSG_OBJ_INFO      "inf"
#define WEECHAT_RELAY_MSG_OBJ_INFOLIST  "inl"
#define WEECHAT_RELAY_MSG_OBJ_ARRAY     "arr"

struct t_weechat_relay_msg
{
    char *id;                          /* message id                        */
    char *data;                        /* binary buffer                     */
    int data_alloc;                    /* currently allocated size          */
    int data_size;                     /* current size of buffer            */
};

/* Relay sessions (client -> WeeChat and WeeChat -> client) */

struct t_weechat_relay_session
{
    int sock;                          /* socket for I/O with peer          */
    int ssl;                           /* connection with SSL               */
    void *gnutls_session;              /* GnuTLS session (only if SSL)      */
};

extern struct t_weechat_relay_session *weechat_relay_session_init (int sock,
                                                                   void *gnutls_session);
extern void weechat_relay_session_free (struct t_weechat_relay_session *session);

/* Relay commands (client -> WeeChat) */

extern int weechat_relay_cmd_raw (struct t_weechat_relay_session *session,
                                  const char *buffer,
                                  size_t size);
extern int weechat_relay_cmd (struct t_weechat_relay_session *session,
                              const char *msg_id,
                              const char *command,
                              const char *arguments[]);
extern int weechat_relay_cmd_handshake (struct t_weechat_relay_session *session,
                                        const char *msg_id,
                                        const char *password_hash_algo,
                                        enum t_weechat_relay_compression compression);
extern int weechat_relay_cmd_init (struct t_weechat_relay_session *session,
                                   const char *msg_id,
                                   const char *password,
                                   enum t_weechat_relay_compression compression);
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
                                    const char *msg_id,
                                    const char *buffer,
                                    const char *data);
extern int weechat_relay_cmd_completion (struct t_weechat_relay_session *session,
                                         const char *msg_id,
                                         const char *buffer,
                                         int position,
                                         const char *data);
extern int weechat_relay_cmd_sync (struct t_weechat_relay_session *session,
                                   const char *msg_id,
                                   const char *buffers,
                                   const char *options);
extern int weechat_relay_cmd_desync (struct t_weechat_relay_session *session,
                                     const char *msg_id,
                                     const char *buffers,
                                     const char *options);
extern int weechat_relay_cmd_test (struct t_weechat_relay_session *session,
                                   const char *msg_id);
extern int weechat_relay_cmd_ping (struct t_weechat_relay_session *session,
                                   const char *msg_id,
                                   const char *arguments);
extern int weechat_relay_cmd_quit (struct t_weechat_relay_session *session,
                                   const char *msg_id);

/* Relay messages (WeeChat -> client) */

extern struct t_weechat_relay_msg *weechat_relay_msg_new (const char *id);
extern int weechat_relay_msg_add_bytes (struct t_weechat_relay_msg *msg,
                                        const void *buffer, int size);
extern int weechat_relay_msg_set_bytes (struct t_weechat_relay_msg *msg,
                                        int position, const void *buffer,
                                        int size);
extern int weechat_relay_msg_add_type (struct t_weechat_relay_msg *msg,
                                       const char *string);
extern int weechat_relay_msg_add_char (struct t_weechat_relay_msg *msg,
                                       char c);
extern int weechat_relay_msg_add_int (struct t_weechat_relay_msg *msg,
                                      int value);
extern int weechat_relay_msg_add_long (struct t_weechat_relay_msg *msg,
                                       long value);
extern int weechat_relay_msg_add_string (struct t_weechat_relay_msg *msg,
                                         const char *string);
extern int weechat_relay_msg_add_buffer (struct t_weechat_relay_msg *msg,
                                         const void *data, int length);
extern int weechat_relay_msg_add_pointer (struct t_weechat_relay_msg *msg,
                                          void *pointer);
extern int weechat_relay_msg_add_time (struct t_weechat_relay_msg *msg,
                                       time_t time);
extern void weechat_relay_msg_free (struct t_weechat_relay_msg *msg);

#endif /* WEECHAT_RELAY_H */
