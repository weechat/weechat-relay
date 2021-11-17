/*
 * command.c - build and send commands from client to WeeChat
 *
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

#include <gnutls/gnutls.h>

#include "weechat-relay.h"


const char *weechat_relay_compression_string[WEECHAT_RELAY_NUM_COMPRESSIONS] =
{ "off", "zlib" };


/*
 * Escapes chars in a string.
 *
 * Note: the value returned must be freed after use.
 */

char *
weechat_relay_cmd_escape (const char *string, const char *chars_to_escape)
{
    size_t length;
    char *escaped, *ptr_escaped;
    const char *ptr_string;

    if (!string || !chars_to_escape)
        return NULL;

    length = (strlen (string) * 2) + 1;
    escaped = malloc (length);
    if (!escaped)
        return NULL;

    ptr_string = string;
    ptr_escaped = escaped;
    while (ptr_string[0])
    {
        if (strchr (chars_to_escape, ptr_string[0]))
        {
            ptr_escaped[0] = '\\';
            ptr_escaped[1] = ptr_string[0];
            ptr_escaped += 2;
        }
        else
        {
            ptr_escaped[0] = ptr_string[0];
            ptr_escaped++;
        }
        ptr_string++;
    }
    ptr_escaped[0] = '\0';

    return escaped;
}

/*
 * Sends a command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd (struct t_weechat_relay_session *session,
                   const char *msg_id,
                   const char *command,
                   const char *arguments[])
{
    char *message;
    size_t length;
    ssize_t num_sent;
    int i;

    if (!session || !command)
        return -1;

    length = 1  /* "(" */
        + strlen ((msg_id) ? msg_id : "")
        + 2  /* ") " */
        + strlen ((command) ? command : "")
        + 1;  /* \0 */

    if (arguments)
    {
        for (i = 0; arguments[i]; i++)
        {
            length += strlen (arguments[i]) + 1;
        }
    }
    length++;

    message = malloc (length);
    if (!message)
        return -1;

    /* format message: "(id) command" */
    snprintf (message, length,
              "%s%s%s%s",
              (msg_id && msg_id[0]) ? "(" : "",
              (msg_id && msg_id[0]) ? msg_id : "",
              (msg_id && msg_id[0]) ? ") " : "",
              command);

    /* add arguments in message (if any) */
    if (arguments)
    {
        for (i = 0; arguments[i]; i++)
        {
            strcat (message, " ");
            strcat (message, arguments[i]);
        }
    }

    strcat (message, "\n");

    num_sent = weechat_relay_session_send (session, message, strlen (message));

    free (message);

    return num_sent;
}

/*
 * Sends the relay "handshake" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_handshake (struct t_weechat_relay_session *session,
                             const char *msg_id,
                             const char *password_hash_algo,
                             enum t_weechat_relay_compression compression)
{
    char *options;
    const char *args[2], *ptr_compression;
    size_t length;
    ssize_t num_sent;

    num_sent = -1;

    options = NULL;

    ptr_compression = weechat_relay_compression_string[compression];

    length = 128 + strlen ((password_hash_algo) ? password_hash_algo : "")
        + strlen (ptr_compression) + 1;
    options = malloc (length);
    if (!options)
        goto end;

    options[0] = '\0';
    if (password_hash_algo)
    {
        strcat (options, "password_hash_algo=");
        strcat (options, password_hash_algo);
    }
    if (options[0])
        strcat (options, ",");
    strcat (options, "compression=");
    strcat (options, ptr_compression);

    args[0] = options;
    args[1] = NULL;

    num_sent = weechat_relay_cmd (session, msg_id, "handshake", args);

end:
    if (options)
        free (options);

    return num_sent;
}

/*
 * Sends the relay "init" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_init (struct t_weechat_relay_session *session,
                        const char *msg_id,
                        const char *password,
                        enum t_weechat_relay_compression compression)
{
    char *password2, *options;
    const char *args[2], *ptr_compression;
    size_t length;
    ssize_t num_sent;

    num_sent = -1;

    password2 = NULL;
    options = NULL;

    password2 = weechat_relay_cmd_escape (password, ",");

    ptr_compression = weechat_relay_compression_string[compression];

    length = 128 + strlen ((password2) ? password2 : "")
        + strlen (ptr_compression) + 1;
    options = malloc (length);
    if (!options)
        goto end;

    options[0] = '\0';
    if (password2)
    {
        strcat (options, "password=");
        strcat (options, password2);
    }
    if (options[0])
        strcat (options, ",");
    strcat (options, "compression=");
    strcat (options, ptr_compression);

    args[0] = options;
    args[1] = NULL;

    num_sent = weechat_relay_cmd (session, msg_id, "init", args);

end:
    if (password2)
        free (password2);
    if (options)
        free (options);

    return num_sent;
}

/*
 * Sends the relay "hdata" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_hdata (struct t_weechat_relay_session *session,
                         const char *msg_id,
                         const char *path,
                         const char *keys)
{
    const char *args[3];

    args[0] = path;
    args[1] = keys;
    args[2] = NULL;

    return weechat_relay_cmd (session, msg_id, "hdata", args);
}

/*
 * Sends the relay "info" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_info (struct t_weechat_relay_session *session,
                        const char *msg_id,
                        const char *name)
{
    const char *args[2];

    args[0] = name;
    args[1] = NULL;

    return weechat_relay_cmd (session, msg_id, "info", args);
}

/*
 * Sends the relay "infolist" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_infolist (struct t_weechat_relay_session *session,
                            const char *msg_id,
                            const char *name,
                            const char *pointer,
                            const char *arguments)
{
    const char *args[4];

    args[0] = name;
    args[1] = pointer;
    args[2] = arguments;
    args[3] = NULL;

    return weechat_relay_cmd (session, msg_id, "infolist", args);
}

/*
 * Sends the relay "nicklist" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_nicklist (struct t_weechat_relay_session *session,
                            const char *msg_id,
                            const char *buffer)
{
    const char *args[2];

    args[0] = buffer;
    args[1] = NULL;

    return weechat_relay_cmd (session, msg_id, "nicklist", args);
}

/*
 * Sends the relay "input" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_input (struct t_weechat_relay_session *session,
                         const char *msg_id,
                         const char *buffer,
                         const char *data)
{
    const char *args[3];

    args[0] = buffer;
    args[1] = data;
    args[2] = NULL;

    return weechat_relay_cmd (session, msg_id, "input", args);
}

/*
 * Sends the relay "completion" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_completion (struct t_weechat_relay_session *session,
                              const char *msg_id,
                              const char *buffer,
                              int position,
                              const char *data)
{
    const char *args[4];
    char str_position[128];

    snprintf (str_position, sizeof (str_position), "%d", position);

    args[0] = buffer;
    args[1] = str_position;
    args[2] = data;
    args[3] = NULL;

    return weechat_relay_cmd (session, msg_id, "completion", args);
}

/*
 * Sends the relay "sync" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_sync (struct t_weechat_relay_session *session,
                        const char *msg_id,
                        const char *buffers,
                        const char *options)
{
    const char *args[3];

    args[0] = buffers;
    args[1] = options;
    args[2] = NULL;

    return weechat_relay_cmd (session, msg_id, "sync", args);
}

/*
 * Sends the relay "desync" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_desync (struct t_weechat_relay_session *session,
                          const char *msg_id,
                          const char *buffers,
                          const char *options)
{
    const char *args[3];

    args[0] = buffers;
    args[1] = options;
    args[2] = NULL;

    return weechat_relay_cmd (session, msg_id, "desync", args);
}

/*
 * Sends the relay "test" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_test (struct t_weechat_relay_session *session,
                        const char *msg_id)
{
    return weechat_relay_cmd (session, msg_id, "test", NULL);
}

/*
 * Sends the relay "ping" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_ping (struct t_weechat_relay_session *session,
                        const char *msg_id,
                        const char *arguments)
{
    const char *args[2];

    args[0] = arguments;
    args[1] = NULL;

    return weechat_relay_cmd (session, msg_id, "ping", args);
}

/*
 * Sends the relay "quit" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

ssize_t
weechat_relay_cmd_quit (struct t_weechat_relay_session *session,
                        const char *msg_id)
{
    return weechat_relay_cmd (session, msg_id, "quit", NULL);
}
