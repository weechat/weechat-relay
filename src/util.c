/*
 * util.c - utility functions for WeeChat relay command line tool
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
#include <stdio.h>
#include <string.h>


/*
 * Dumps a data buffer as hexadecimal + ascii.
 *
 * Note: result must be freed after use.
 */

char *
string_hex_dump (const char *data, size_t size, int bytes_per_line,
                 const char *prefix, const char *suffix)
{
    char *buf, *str_hexa, *str_ascii, str_format_line[64], *str_line;
    int length_hexa, length_ascii, length_prefix, length_suffix, length_line;
    int hexa_pos, ascii_pos;
    size_t i;

    if (!data || (size < 1) || (bytes_per_line < 1))
        return NULL;

    str_hexa = NULL;
    str_ascii = NULL;
    str_line = NULL;
    buf = NULL;

    length_hexa = bytes_per_line * 3;
    str_hexa = malloc (length_hexa + 1);
    if (!str_hexa)
        goto end;

    length_ascii = bytes_per_line * 2;
    str_ascii = malloc (length_ascii + 1);
    if (!str_ascii)
        goto end;

    length_prefix = (prefix) ? strlen (prefix) : 0;
    length_suffix = (suffix) ? strlen (suffix) : 0;

    length_line = length_prefix + (bytes_per_line * 3) + 2 + length_ascii +
        length_suffix;
    str_line = malloc (length_line + 1);
    if (!str_line)
        goto end;

    buf = malloc ((((size / bytes_per_line) + 1) * (length_line + 1)) + 1);
    if (!buf)
        goto end;
    buf[0] = '\0';

    snprintf (str_format_line, sizeof (str_format_line),
              "%%s%%-%ds  %%-%ds%%s",
              length_hexa,
              length_ascii);

    hexa_pos = 0;
    ascii_pos = 0;
    for (i = 0; i < size; i++)
    {
        snprintf (str_hexa + hexa_pos, 4,
                  "%02X ", (unsigned char)(data[i]));
        hexa_pos += 3;
        snprintf (str_ascii + ascii_pos, 3, "%c ",
                  ((((unsigned char)data[i]) < 32)
                   || (((unsigned char)data[i]) > 127)) ?
                  '.' : (unsigned char)(data[i]));
        ascii_pos += 2;
        if (ascii_pos == bytes_per_line * 2)
        {
            if (buf[0])
                strcat (buf, "\n");
            str_ascii[ascii_pos - 1] = '\0';
            snprintf (str_line, length_line + 1,
                      str_format_line,
                      (prefix) ? prefix : "",
                      str_hexa,
                      str_ascii,
                      (suffix) ? suffix : "");
            strcat (buf, str_line);
            hexa_pos = 0;
            ascii_pos = 0;
        }
    }
    if (ascii_pos > 0)
    {
        if (buf[0])
            strcat (buf, "\n");
        str_ascii[ascii_pos - 1] = '\0';
        str_ascii[ascii_pos] = '\0';
        snprintf (str_line, length_line + 1,
                  str_format_line,
                  (prefix) ? prefix : "",
                  str_hexa,
                  str_ascii,
                  (suffix) ? suffix : "");
        strcat (buf, str_line);
    }

end:
    if (str_hexa)
        free (str_hexa);
    if (str_ascii)
        free (str_ascii);
    if (str_line)
        free (str_line);

    return buf;
}

/*
 * Displays hexadecimal dump of a message.
 */

void
display_hex_dump (const void *buffer, size_t size)
{
    char *str_dump;

    if (!buffer || (size == 0))
        return;

    str_dump = string_hex_dump (buffer, size, 16, "      ", NULL);
    if (str_dump)
    {
        printf ("%s\n", str_dump);
        free (str_dump);
    }
}
