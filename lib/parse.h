/*
 * Copyright (C) 2019-2023 Sébastien Helleu <flashcode@flashtux.org>
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

#ifndef WEECHAT_RELAY_PARSE_H
#define WEECHAT_RELAY_PARSE_H

extern int weechat_relay_parse_read_bytes (
    struct t_weechat_relay_parsed_msg *parsed_msg, void *output, size_t count);
extern int weechat_relay_parse_read_type (
    struct t_weechat_relay_parsed_msg *parsed_msg,
    enum t_weechat_relay_obj_type *type);
extern int weechat_relay_parse_read_integer (
    struct t_weechat_relay_parsed_msg *parsed_msg, int *value);
extern int weechat_relay_parse_read_string (
    struct t_weechat_relay_parsed_msg *parsed_msg, char **string);
extern int weechat_relay_parse_read_buffer (
    struct t_weechat_relay_parsed_msg *parsed_msg, void **buffer, int *length);
extern int weechat_relay_parse_read_pointer (
    struct t_weechat_relay_parsed_msg *parsed_msg, const void **pointer);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_char (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_integer (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_long (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_string (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_buffer (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_pointer (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_time (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_hashtable (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern int weechat_relay_parse_hdata_split_hpath (
    const char *hpath, char ***hpaths, int *num_hpaths);
extern int weechat_relay_parse_hdata_split_keys (
    const char *keys, char ***keys_names,
    enum t_weechat_relay_obj_type **keys_types, int *num_keys);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_hdata (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_info (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_infolist (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_obj_array (
    struct t_weechat_relay_parsed_msg *parsed_msg);
extern struct t_weechat_relay_obj *weechat_relay_parse_read_object (
    struct t_weechat_relay_parsed_msg *parsed_msg,
    enum t_weechat_relay_obj_type type);
extern void *weechat_relay_parse_decompress_zlib (const void *data,
                                                  size_t size,
                                                  size_t initial_output_size,
                                                  size_t *size_decompressed);
extern void *weechat_relay_parse_decompress_zstd (const void *data,
                                                  size_t size,
                                                  size_t initial_output_size,
                                                  size_t *size_decompressed);
extern struct t_weechat_relay_parsed_msg *weechat_relay_parse_msg_alloc (
    const void *buffer, size_t size);
extern void weechat_relay_parse_msg_free (
    struct t_weechat_relay_parsed_msg *parsed_msg);

#endif /* WEECHAT_RELAY_PARSE_H */
