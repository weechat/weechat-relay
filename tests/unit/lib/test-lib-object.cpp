/*
 * test-lib-object.cpp - test object functions
 *
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

#include "CppUTest/TestHarness.h"

extern "C"
{
#include "string.h"
#include "tests/tests.h"
#include "lib/weechat-relay.h"
#include "lib/object.h"
}

TEST_GROUP(LibObject)
{
};

/*
 * Tests functions:
 *   weechat_relay_obj_search_type
 */

TEST(LibObject, SearchType)
{
    int i;

    LONGS_EQUAL(-1, weechat_relay_obj_search_type (NULL));
    LONGS_EQUAL(-1, weechat_relay_obj_search_type (""));
    LONGS_EQUAL(-1, weechat_relay_obj_search_type ("xxx"));

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        LONGS_EQUAL(i, weechat_relay_obj_search_type (weechat_relay_obj_types_str[i]));
    }
}

/*
 * Tests functions:
 *   weechat_relay_obj_alloc
 *   weechat_relay_obj_free
 */

TEST(LibObject, AllocFree)
{
    struct t_weechat_relay_obj *obj;
    int i;

    weechat_relay_obj_free (NULL);

    for (i = 0; i < WEECHAT_RELAY_NUM_OBJ_TYPES; i++)
    {
        obj = weechat_relay_obj_alloc ((t_weechat_relay_obj_type)i);
        CHECK(obj);
        LONGS_EQUAL(i, obj->type);
        weechat_relay_obj_free (obj);
    }
}
