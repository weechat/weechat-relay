/*
 * Copyright (C) 2019-2022 Sébastien Helleu <flashcode@flashtux.org>
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

#include <stdint.h>
#include <time.h>

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
    WEECHAT_RELAY_COMPRESSION_ZSTD,    /* zstd compression                  */
    /* number of compressions */
    WEECHAT_RELAY_NUM_COMPRESSIONS,
};

/* Messages */

#define WEECHAT_RELAY_MSG_INITIAL_ALLOC 4096

/* object ids in binary messages */
enum t_weechat_relay_obj_type
{
    WEECHAT_RELAY_OBJ_TYPE_CHAR = 0,
    WEECHAT_RELAY_OBJ_TYPE_INTEGER,
    WEECHAT_RELAY_OBJ_TYPE_LONG,
    WEECHAT_RELAY_OBJ_TYPE_STRING,
    WEECHAT_RELAY_OBJ_TYPE_BUFFER,
    WEECHAT_RELAY_OBJ_TYPE_POINTER,
    WEECHAT_RELAY_OBJ_TYPE_TIME,
    WEECHAT_RELAY_OBJ_TYPE_HASHTABLE,
    WEECHAT_RELAY_OBJ_TYPE_HDATA,
    WEECHAT_RELAY_OBJ_TYPE_INFO,
    WEECHAT_RELAY_OBJ_TYPE_INFOLIST,
    WEECHAT_RELAY_OBJ_TYPE_ARRAY,
    /* numger of object types */
    WEECHAT_RELAY_NUM_OBJ_TYPES,
};

/* structure to send a binary message (WeeChat -> client) */
struct t_weechat_relay_msg
{
    char *id;                          /* message id                        */
    char *data;                        /* binary buffer                     */
    size_t data_alloc;                 /* currently allocated size          */
    size_t data_size;                  /* current size of buffer            */
};

/* message parsing */
struct t_weechat_relay_obj;

struct t_weechat_relay_obj_buffer
{
    void *buffer;
    int length;
};

struct t_weechat_relay_obj_hashtable
{
    enum t_weechat_relay_obj_type type_keys;
    enum t_weechat_relay_obj_type type_values;
    int count;
    struct t_weechat_relay_obj **keys;
    struct t_weechat_relay_obj **values;
};

struct t_weechat_relay_obj_hdata
{
    char *hpath;
    int num_hpaths;
    char **hpaths;
    char *keys;
    int num_keys;
    char **keys_names;
    enum t_weechat_relay_obj_type *keys_types;
    int count;
    struct t_weechat_relay_obj ***ppath;
    struct t_weechat_relay_obj ***values;
};

struct t_weechat_relay_obj_info
{
    char *name;
    char *value;
};

struct t_weechat_relay_obj_infolist_var
{
    char *name;
    struct t_weechat_relay_obj *obj;
};

struct t_weechat_relay_obj_infolist_item
{
    int count;
    struct t_weechat_relay_obj_infolist_var **variables;
};

struct t_weechat_relay_obj_infolist
{
    char *name;
    int count;
    struct t_weechat_relay_obj_infolist_item **items;
};

struct t_weechat_relay_obj_array
{
    int count;
    struct t_weechat_relay_obj **values;
};

struct t_weechat_relay_obj
{
    enum t_weechat_relay_obj_type type;
    union
    {
        char value_char;
        int value_integer;
        long value_long;
        char *value_string;
        struct t_weechat_relay_obj_buffer value_buffer;
        const void *value_pointer;
        time_t value_time;
        struct t_weechat_relay_obj_hashtable value_hashtable;
        struct t_weechat_relay_obj_hdata value_hdata;
        struct t_weechat_relay_obj_info value_info;
        struct t_weechat_relay_obj_infolist value_infolist;
        struct t_weechat_relay_obj_array value_array;
    };
};

struct t_weechat_relay_parsed_msg
{
    void *message;                                /* message                */
    size_t length;                                /* message length         */
    size_t length_data;                           /* length - 5             */
    void *data_decompressed;                      /* decompressed data      */
                                                  /* (NULL if not compress.)*/
    size_t length_data_decompressed;              /* decompressed length    */

    enum t_weechat_relay_compression compression; /* compression type       */
    char *id;                                     /* message id             */

    int num_objects;                              /* number of objects      */
    struct t_weechat_relay_obj **objects;         /* parsed objects         */

    /* parser variables */
    const void *buffer;                /* pointer to data or data_decomp.   */
                                       /* (after the 5 first bytes)         */
    size_t size;                       /* size of buffer                    */
    size_t position;                   /* current position in buffer        */
};

/* Relay sessions (client -> WeeChat and WeeChat -> client) */

struct t_weechat_relay_session
{
    int sock;                          /* socket for I/O with peer          */
    int ssl;                           /* connection with SSL               */
    void *gnutls_sess;                 /* GnuTLS session (only if SSL)      */

    /* buffer for received data */
    void *buffer;                      /* buffer                            */
    size_t buffer_size;                /* size of buffer                    */
};

/* arrays */

extern const char *weechat_relay_compression_string[WEECHAT_RELAY_NUM_COMPRESSIONS];
extern const char *weechat_relay_obj_types_str[WEECHAT_RELAY_NUM_OBJ_TYPES];

/* Relay session */

extern struct t_weechat_relay_session *weechat_relay_session_init (int sock,
                                                                   void *gnutls_session);
extern ssize_t weechat_relay_session_send (struct t_weechat_relay_session *session,
                                           void *buffer, size_t size);
extern ssize_t weechat_relay_session_recv (struct t_weechat_relay_session *session,
                                           void *buffer, size_t size);
extern int weechat_relay_session_buffer_add_bytes (struct t_weechat_relay_session *session,
                                                   const void *buffer, size_t size);
extern void weechat_relay_session_buffer_pop (struct t_weechat_relay_session *session,
                                              void **buffer, size_t *size);
extern void weechat_relay_session_free (struct t_weechat_relay_session *session);

/* Relay commands (client -> WeeChat) */

extern ssize_t weechat_relay_cmd_send (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *command,
                                       const char *arguments[]);
extern ssize_t weechat_relay_cmd_handshake (struct t_weechat_relay_session *session,
                                            const char *msg_id,
                                            const char *password_hash_algo,
                                           enum t_weechat_relay_compression compression);
extern ssize_t weechat_relay_cmd_init (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *password,
                                       enum t_weechat_relay_compression compression);
extern ssize_t weechat_relay_cmd_hdata (struct t_weechat_relay_session *session,
                                        const char *msg_id,
                                        const char *path,
                                        const char *keys);
extern ssize_t weechat_relay_cmd_info (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *name);
extern ssize_t weechat_relay_cmd_infolist (struct t_weechat_relay_session *session,
                                           const char *msg_id,
                                           const char *name,
                                           const char *pointer,
                                           const char *arguments);
extern ssize_t weechat_relay_cmd_nicklist (struct t_weechat_relay_session *session,
                                           const char *msg_id,
                                           const char *buffer);
extern ssize_t weechat_relay_cmd_input (struct t_weechat_relay_session *session,
                                        const char *msg_id,
                                        const char *buffer,
                                        const char *data);
extern ssize_t weechat_relay_cmd_completion (struct t_weechat_relay_session *session,
                                             const char *msg_id,
                                             const char *buffer,
                                             int position,
                                             const char *data);
extern ssize_t weechat_relay_cmd_sync (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *buffers,
                                       const char *options);
extern ssize_t weechat_relay_cmd_desync (struct t_weechat_relay_session *session,
                                         const char *msg_id,
                                         const char *buffers,
                                         const char *options);
extern ssize_t weechat_relay_cmd_test (struct t_weechat_relay_session *session,
                                       const char *msg_id);
extern ssize_t weechat_relay_cmd_ping (struct t_weechat_relay_session *session,
                                       const char *msg_id,
                                       const char *arguments);
extern ssize_t weechat_relay_cmd_quit (struct t_weechat_relay_session *session,
                                       const char *msg_id);

/* Relay messages (WeeChat -> client) */

extern struct t_weechat_relay_msg *weechat_relay_msg_new (const char *id);
extern int weechat_relay_msg_add_bytes (struct t_weechat_relay_msg *msg,
                                        const void *buffer, size_t size);
extern int weechat_relay_msg_set_bytes (struct t_weechat_relay_msg *msg,
                                        size_t position, const void *buffer,
                                        size_t size);
extern int weechat_relay_msg_add_type (struct t_weechat_relay_msg *msg,
                                       enum t_weechat_relay_obj_type obj_type);
extern int weechat_relay_msg_add_char (struct t_weechat_relay_msg *msg,
                                       char c);
extern int weechat_relay_msg_add_int (struct t_weechat_relay_msg *msg,
                                      int value);
extern int weechat_relay_msg_add_long (struct t_weechat_relay_msg *msg,
                                       long value);
extern int weechat_relay_msg_add_string (struct t_weechat_relay_msg *msg,
                                         const char *string);
extern int weechat_relay_msg_add_buffer (struct t_weechat_relay_msg *msg,
                                         const void *data, size_t length);
extern int weechat_relay_msg_add_pointer (struct t_weechat_relay_msg *msg,
                                          void *pointer);
extern int weechat_relay_msg_add_time (struct t_weechat_relay_msg *msg,
                                       time_t time);
extern void *weechat_relay_msg_compress_zlib (struct t_weechat_relay_msg *msg,
                                              int compression_level,
                                              size_t *size);
extern void *weechat_relay_msg_compress_zstd (struct t_weechat_relay_msg *msg,
                                              int compression_level,
                                              size_t *size);
extern void weechat_relay_msg_free (struct t_weechat_relay_msg *msg);

/* Functions to parse binary messages sent by WeeChat (client side) */

extern struct t_weechat_relay_parsed_msg *weechat_relay_parse_message (const void *buffer,
                                                                       size_t size);
extern void weechat_relay_parse_msg_free (struct t_weechat_relay_parsed_msg *parsed_msg);

#endif /* WEECHAT_RELAY_H */
