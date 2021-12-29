/*
 * object.c - create objects for messages sent to client and parsed messages
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

#include <stdlib.h>
#include <string.h>

#include "weechat-relay.h"
#include "object.h"


const char *weechat_relay_obj_types_str[WEECHAT_RELAY_NUM_OBJ_TYPES] = {
    "chr", "int", "lon", "str", "buf", "ptr", "tim", "htb",
    "hda", "inf", "inl", "arr",
};


/*
 * Searches object type.
 *
 * Returns index of type found (enum t_weechat_relay_obj_type),
 * -1 if not found.
 */

int
weechat_relay_obj_search_type (const char *obj_type)
{
    int i;

    if (!obj_type)
        return -1;

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        if (strcmp (obj_type, weechat_relay_obj_types_str[i]) == 0)
            return i;
    }

    return -1;
}

/*
 * Allocates an object structure.
 *
 * Returns the new object, NULL if error.
 */

struct t_weechat_relay_obj *
weechat_relay_obj_alloc (enum t_weechat_relay_obj_type type)
{
    struct t_weechat_relay_obj *obj;

    obj = calloc (1, sizeof (*obj));
    if (!obj)
        return NULL;

    obj->type = type;

    return obj;
}

/*
 * Frees an object.
 */

void
weechat_relay_obj_free (struct t_weechat_relay_obj *obj)
{
    int i, j;

    if (!obj)
        return;

    switch (obj->type)
    {
        case WEECHAT_RELAY_OBJ_TYPE_CHAR:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INTEGER:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_LONG:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_STRING:
            if (obj->value_string)
                free (obj->value_string);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_BUFFER:
            if (obj->value_buffer.buffer)
                free (obj->value_buffer.buffer);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_POINTER:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_TIME:
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HASHTABLE:
            for (i = 0; i < obj->value_hashtable.count; i++)
            {
                if (obj->value_hashtable.keys)
                    weechat_relay_obj_free (obj->value_hashtable.keys[i]);
                if (obj->value_hashtable.values)
                    weechat_relay_obj_free (obj->value_hashtable.values[i]);
            }
            if (obj->value_hashtable.keys)
                free (obj->value_hashtable.keys);
            if (obj->value_hashtable.values)
                free (obj->value_hashtable.values);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_HDATA:
            if (obj->value_hdata.hpath)
                free (obj->value_hdata.hpath);
            if (obj->value_hdata.keys)
                free (obj->value_hdata.keys);
            if (obj->value_hdata.hpaths)
            {
                for (i = 0; i < obj->value_hdata.num_hpaths; i++)
                {
                    if (obj->value_hdata.hpaths[i])
                        free (obj->value_hdata.hpaths[i]);
                }
                free (obj->value_hdata.hpaths);
            }
            if (obj->value_hdata.keys_names)
            {
                for (i = 0; i < obj->value_hdata.num_keys; i++)
                {
                    if (obj->value_hdata.keys_names[i])
                        free (obj->value_hdata.keys_names[i]);
                }
                free (obj->value_hdata.keys_names);
            }
            if (obj->value_hdata.keys_types)
                free (obj->value_hdata.keys_types);
            for (i = 0; i < obj->value_hdata.count; i++)
            {
                if (obj->value_hdata.ppath && obj->value_hdata.ppath[i])
                {
                    for (j = 0; j < obj->value_hdata.num_hpaths; j++)
                    {
                        weechat_relay_obj_free (obj->value_hdata.ppath[i][j]);
                    }
                    free (obj->value_hdata.ppath[i]);
                }
                if (obj->value_hdata.values && obj->value_hdata.values[i])
                {
                    for (j = 0; j < obj->value_hdata.num_keys; j++)
                    {
                        weechat_relay_obj_free (obj->value_hdata.values[i][j]);
                    }
                    free (obj->value_hdata.values[i]);
                }
            }
            if (obj->value_hdata.ppath)
                free (obj->value_hdata.ppath);
            if (obj->value_hdata.values)
                free (obj->value_hdata.values);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFO:
            if (obj->value_info.name)
                free (obj->value_info.name);
            if (obj->value_info.value)
                free (obj->value_info.value);
            break;
        case WEECHAT_RELAY_OBJ_TYPE_INFOLIST:
            if (obj->value_infolist.name)
                free (obj->value_infolist.name);
            if (obj->value_infolist.items)
            {
                for (i = 0; i < obj->value_infolist.count; i++)
                {
                    if (obj->value_infolist.items[i])
                    {
                        if (obj->value_infolist.items[i]->variables)
                        {
                            for (j = 0; j < obj->value_infolist.items[i]->count; j++)
                            {
                                if (obj->value_infolist.items[i]->variables[j])
                                {
                                    if (obj->value_infolist.items[i]->variables[j]->name)
                                        free (obj->value_infolist.items[i]->variables[j]->name);
                                    weechat_relay_obj_free (obj->value_infolist.items[i]->variables[j]->obj);
                                    free (obj->value_infolist.items[i]->variables[j]);
                                }
                            }
                            free (obj->value_infolist.items[i]->variables);
                        }
                        free (obj->value_infolist.items[i]);
                    }
                }
                free (obj->value_infolist.items);
            }
            break;
        case WEECHAT_RELAY_OBJ_TYPE_ARRAY:
            if (obj->value_array.values)
            {
                for (i = 0; i < obj->value_array.count; i++)
                {
                    if (obj->value_array.values[i])
                        weechat_relay_obj_free (obj->value_array.values[i]);
                }
                free (obj->value_array.values);
            }
            break;
        case WEECHAT_RELAY_NUM_OBJ_TYPES:
            break;
    }

    free (obj);
}
