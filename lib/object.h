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

#ifndef WEECHAT_RELAY_OBJECT_H
#define WEECHAT_RELAY_OBJECT_H

extern int weechat_relay_obj_search_type (const char *obj_type);
extern struct t_weechat_relay_obj *weechat_relay_obj_alloc (enum t_weechat_relay_obj_type type);
extern void weechat_relay_obj_free (struct t_weechat_relay_obj *obj);

#endif /* WEECHAT_RELAY_OBJECT_H */
