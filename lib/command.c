/*
 * command.c - build and send commands from client to WeeChat
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
    int length;
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
 * Sends a command to WeeChat (raw data as input).
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_raw (struct t_weechat_relay_session *session,
                       const char *buffer, size_t size)
{
    int rc;

    if (!session || (size == 0))
        return -1;

    if (session->ssl)
    {
        rc = gnutls_record_send (*((gnutls_session_t *)session->gnutls_session),
                                 buffer, size);
    }
    else
    {
        rc = write (session->sock, buffer, size);
    }

    if (rc < 0)
        return -1;

    return rc;
}

/*
 * Sends a command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd (struct t_weechat_relay_session *session,
                   const char *msg_id,
                   const char *command,
                   const char *arguments[])
{
    char *message;
    int length, i, rc;

    if (!session || !command)
        return -1;

    length = 1 + strlen ((msg_id) ? msg_id : "") + 2 +
        strlen ((command) ? command : "") + 1;

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

    rc = weechat_relay_cmd_raw (session, message, strlen (message));

    free (message);

    return rc;
}

/*
 * Sends the relay "init" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_init (struct t_weechat_relay_session *session,
                        const char *password,
                        enum t_weechat_relay_compression compression)
{
    char *password2, *options;
    const char *args[2], *ptr_compression;
    int rc, length;

    rc = -1;

    password2 = NULL;
    options = NULL;

    password2 = weechat_relay_cmd_escape (password, ",");

    ptr_compression = weechat_relay_compression_string[compression];

    length = 128 + strlen ((password2) ? password2 : "") +
        strlen (ptr_compression) + 1;
    options = malloc (length);
    if (!options)
        goto end;

    snprintf (options, length,
              "%s%s%scompression=%s",
              (password2) ? "password=" : "",
              (password2) ? password2 : "",
              (password2) ? "," : "",
              ptr_compression);

    args[0] = options;
    args[1] = NULL;

    rc = weechat_relay_cmd (session, NULL, "init", args);

end:
    if (password2)
        free (password2);
    if (options)
        free (options);

    return rc;
}

/*
 * Sends the relay "hdata" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
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

int
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

int
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

int
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

int
weechat_relay_cmd_input (struct t_weechat_relay_session *session,
                         const char *buffer,
                         const char *data)
{
    const char *args[3];

    args[0] = buffer;
    args[1] = data;
    args[2] = NULL;

    return weechat_relay_cmd (session, NULL, "input", args);
}

/*
 * Sends the relay "sync" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_sync (struct t_weechat_relay_session *session,
                        const char *buffers,
                        const char *options)
{
    const char *args[3];

    args[0] = buffers;
    args[1] = options;
    args[2] = NULL;

    return weechat_relay_cmd (session, NULL, "sync", args);
}

/*
 * Sends the relay "desync" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_desync (struct t_weechat_relay_session *session,
                          const char *buffers,
                          const char *options)
{
    const char *args[3];

    args[0] = buffers;
    args[1] = options;
    args[2] = NULL;

    return weechat_relay_cmd (session, NULL, "desync", args);
}

/*
 * Sends the relay "test" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_test (struct t_weechat_relay_session *session)
{
    return weechat_relay_cmd (session, NULL, "test", NULL);
}

/*
 * Sends the relay "ping" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_ping (struct t_weechat_relay_session *session,
                        const char *arguments)
{
    const char *args[2];

    args[0] = arguments;
    args[1] = NULL;

    return weechat_relay_cmd (session, NULL, "ping", args);
}

/*
 * Sends the relay "quit" command to WeeChat.
 *
 * Returns the number of bytes sent, -1 if error.
 */

int
weechat_relay_cmd_quit (struct t_weechat_relay_session *session)
{
    return weechat_relay_cmd (session, NULL, "quit", NULL);
}
