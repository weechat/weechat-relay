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

#ifndef WEECHAT_RELAY_TESTS_H
#define WEECHAT_RELAY_TESTS_H

#define MESSAGE_INVALID_SIZE                                            \
    0x00, 0x00, 0x00, 0x06,                                             \
    0x00,                                                               \
    0xFF, 0xFF, 0xFF, 0xFF

#define MESSAGE_INVALID_ID                                              \
    0x00, 0x00, 0x00, 0x07,                                             \
    0x00,                                                               \
    0xFF, 0xFF

#define MESSAGE_INVALID_COMPRESSED_DATA                                 \
    0x00, 0x00, 0x00, 0x08,                                             \
    0x01,                                                               \
    0x01, 0x02, 0x03

#define LOREM_IPSUM_4096                                                      \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam dictum" \
    "dolor non egestas mollis. Duis condimentum ipsum eu metus aliquet, id f" \
    "ringilla sem lobortis. Integer tellus nisi, pretium ut orci eu, pulvina" \
    "r dapibus leo. Quisque dapibus urna rutrum, sagittis lorem eget, dapibu" \
    "s odio. Ut ultrices, orci maximus dapibus pharetra, tortor libero volut" \
    "pat dui, nec bibendum lectus dolor vitae erat. In hac habitasse platea " \
    "dictumst. Aliquam elementum sem ac augue bibendum, ac scelerisque liber" \
    "o consequat. Aliquam ultrices mauris vel ipsum finibus elementum. In co" \
    "nvallis sit amet nunc in blandit. Maecenas fringilla elit sed elit cons" \
    "ectetur, in ultrices lacus vulputate. Pellentesque eget auctor lectus. " \
    "Ut vitae lacus facilisis, lacinia velit id, interdum nisl. Morbi vitae " \
    "finibus justo, sit amet dictum diam. Curabitur commodo mauris quis sapi" \
    "en eleifend rutrum. Morbi accumsan ut tellus ut placerat. Integer ante " \
    "sapien, maximus eget ex sed, rutrum dignissim mauris. Maecenas ut posue" \
    "re tortor. Nunc ut elit neque. Fusce vulputate leo justo, et tempus eni" \
    "m tincidunt eu. Sed vel maximus nisi, sit amet gravida turpis. Suspendi" \
    "sse potenti. Cras arcu magna, egestas quis pellentesque ac, tincidunt u" \
    "t felis. Sed non nibh vitae enim ornare dignissim sed ac sem. In in mat" \
    "tis est. Integer sem tellus, auctor eu varius non, facilisis eget quam." \
    "Cras ac purus non odio sollicitudin tincidunt vitae ultrices nunc. Phas" \
    "ellus aliquam leo vitae egestas vulputate. Nulla facilisi. Fusce enim o" \
    "rci, dapibus eu purus vel, commodo porttitor turpis. Nunc sodales, ligu" \
    "la tincidunt malesuada euismod, felis ligula imperdiet dui, at cursus i" \
    "psum lacus sed sapien. Vestibulum vehicula risus eu mi cursus imperdiet" \
    "quis vel odio. Nulla facilisi. Integer vitae eros turpis. Nulla a augue" \
    "lacus. Quisque nec egestas orci. Suspendisse aliquet ipsum non nisl eui" \
    "smod, eget auctor odio lobortis. Morbi et mattis augue. Maecenas alique" \
    "t ornare lorem, sit amet molestie neque suscipit in. Praesent vel dui o" \
    "rci. Nunc felis risus, interdum ac placerat sed, rhoncus in ipsum. Pell" \
    "entesque in diam ut nisi scelerisque ullamcorper. Nunc lacinia accumsan" \
    "urna, vitae aliquam nunc eleifend et. Aliquam et fermentum neque, et tr" \
    "istique justo. Integer massa orci, consequat vel libero ut, dictum mole" \
    "stie sem. Vestibulum eget metus id mi lobortis aliquam. Orci varius nat" \
    "oque penatibus et magnis dis parturient montes, nascetur ridiculus mus." \
    "Phasellus maximus dignissim suscipit. Aenean nec sollicitudin ex. Donec" \
    "auctor sapien condimentum diam egestas scelerisque. Fusce accumsan elem" \
    "entum felis id rutrum. Vestibulum orci nunc, interdum non purus vestibu" \
    "lum, sodales varius enim. Mauris nec pellentesque lacus, pulvinar molli" \
    "s lacus. Nullam at luctus mauris, sit amet finibus nibh. Mauris at lobo" \
    "rtis erat, sit amet euismod neque. Integer hendrerit est nec est commod" \
    "o, ac pellentesque metus aliquam. Nam quam nunc, posuere ut neque vel, " \
    "egestas commodo mauris. Curabitur aliquam fringilla nisl, sed viverra n" \
    "unc tristique eu. Fusce non eleifend leo. Pellentesque malesuada lorem " \
    "finibus felis vulputate convallis. Nunc semper justo non ipsum consequa" \
    "t pulvinar. Proin mi urna, congue eget nunc in, condimentum feugiat nib" \
    "h. Vivamus rhoncus varius bibendum. Aliquam sit amet neque accumsan, fr" \
    "ingilla est eu, fermentum magna. Nulla sit amet nunc in nisl sodales ph" \
    "aretra. Donec condimentum lacinia metus sit amet iaculis. Vivamus quis " \
    "odio lacus. Nunc eleifend enim et velit pretium porta. Pellentesque pel" \
    "lentesque lacus mi, venenatis hendrerit sem egestas vel. Cras vehicula " \
    "facilisis lacus ac faucibus. Aenean efficitur velit felis, ac faucibus " \
    "augue tempor sit amet. Maecenas varius mattis condimentum. Quisque nec " \
    "ornare lectus. Donec id tristique sapien. Duis eget ante consectetur, d" \
    "ictum libero sed, iaculis sapien. Donec nec tempus magna. Sed lacinia m" \
    "agna sed arcu maximus, quis auctor ante aliquet. Nam vitae hendrerit le" \
    "ctus, blandit condimentum odio. Nullam euismod lacus a erat semper, ut " \
    "molestie risus consectetur. Nam mattis dui risus, nec tincidunt sem sem" \
    "per ut. Quisque hendrerit mattis urna ida."

#define MESSAGE_BUILD_FAKE(msg)                                         \
    msg = weechat_relay_msg_new ("test");                               \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_CHAR);      \
    weechat_relay_msg_add_char (msg, 'A');                              \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_CHAR);      \
    weechat_relay_msg_add_char (msg, 'Z');                              \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_INTEGER);   \
    weechat_relay_msg_add_integer (msg, 123456);                        \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_INTEGER);   \
    weechat_relay_msg_add_integer (msg, -987654);                       \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_LONG);      \
    weechat_relay_msg_add_long (msg, 1234567890L);                      \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_LONG);      \
    weechat_relay_msg_add_long (msg, -9876543210L);                     \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_STRING);    \
    weechat_relay_msg_add_string (msg, LOREM_IPSUM_4096);               \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_POINTER);   \
    weechat_relay_msg_add_pointer (msg, (void *)0x1234abcd);            \
    weechat_relay_msg_add_type (msg, WEECHAT_RELAY_OBJ_TYPE_TIME);      \
    weechat_relay_msg_add_time (msg, 1640091762);

#define OBJ_CHAR 'a'
#define MESSAGE_CHAR                                                    \
    0x00, 0x00, 0x00, 14 + 1,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'c', 'h', 'r',                                                      \
    OBJ_CHAR

#define OBJ_INTEGER 0x00, 0x01, 0xE2, 0x40
#define MESSAGE_INTEGER                                                 \
    0x00, 0x00, 0x00, 14 + 4,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'i', 'n', 't',                                                      \
    OBJ_INTEGER

#define OBJ_INTEGER_2 0xFF, 0xFE, 0x1D, 0xC0
#define MESSAGE_INTEGER_2                                               \
    0x00, 0x00, 0x00, 14 + 4,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'i', 'n', 't',                                                      \
    OBJ_INTEGER_2

#define OBJ_LONG_INVALID 0x01, 0x02
#define MESSAGE_LONG_INVALID                                            \
    0x00, 0x00, 0x00, 14 + 2,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'l', 'o', 'n',                                                      \
    OBJ_LONG_INVALID

#define OBJ_LONG                                                        \
    0x0A, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30
#define MESSAGE_LONG                                                    \
    0x00, 0x00, 0x00, 14 + 11,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'l', 'o', 'n',                                                      \
    OBJ_LONG

#define OBJ_LONG_2                                                      \
    0x0B, 0x2D, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,   \
    0x30
#define MESSAGE_LONG_2                                                  \
    0x00, 0x00, 0x00, 14 + 12,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'l', 'o', 'n',                                                      \
    OBJ_LONG_2

#define OBJ_STRING_NULL 0xFF, 0xFF, 0xFF, 0xFF
#define MESSAGE_STRING_NULL                                             \
    0x00, 0x00, 0x00, 14 + 4,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    's', 't', 'r',                                                      \
    OBJ_STRING_NULL

#define OBJ_STRING_EMPTY 0x00, 0x00, 0x00, 0x00
#define MESSAGE_STRING_EMPTY                                            \
    0x00, 0x00, 0x00, 14 + 4,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    's', 't', 'r',                                                      \
    OBJ_STRING_EMPTY

#define OBJ_STRING 0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c'
#define MESSAGE_STRING                                                  \
    0x00, 0x00, 0x00, 14 + 7,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    's', 't', 'r',                                                      \
    OBJ_STRING

#define OBJ_BUFFER_NULL 0xFF, 0xFF, 0xFF, 0xFF
#define MESSAGE_BUFFER_NULL                                             \
    0x00, 0x00, 0x00, 14 + 4,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'b', 'u', 'f',                                                      \
    OBJ_BUFFER_NULL

#define OBJ_BUFFER_NULL_2 0x00, 0x00, 0x00, 0x00
#define MESSAGE_BUFFER_NULL_2                                           \
    0x00, 0x00, 0x00, 14 + 4,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'b', 'u', 'f',                                                      \
    OBJ_BUFFER_NULL_2

#define OBJ_BUFFER 0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c'
#define MESSAGE_BUFFER                                                  \
    0x00, 0x00, 0x00, 14 + 7,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'b', 'u', 'f',                                                      \
    OBJ_BUFFER

#define OBJ_POINTER_NULL 0x01, '0'
#define MESSAGE_POINTER_NULL                                            \
    0x00, 0x00, 0x00, 14 + 2,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'p', 't', 'r',                                                      \
    OBJ_POINTER_NULL

#define OBJ_POINTER_INVALID 0x01, 0x02
#define MESSAGE_POINTER_INVALID                                         \
    0x00, 0x00, 0x00, 14 + 2,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'p', 't', 'r',                                                      \
    OBJ_POINTER_INVALID

#define OBJ_POINTER                                                     \
    0x09, 0x31, 0x61, 0x32, 0x62, 0x33, 0x63, 0x34, 0x64, 0x35
#define MESSAGE_POINTER                                                 \
    0x00, 0x00, 0x00, 14 + 10,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'p', 't', 'r',                                                      \
    OBJ_POINTER

#define OBJ_TIME_INVALID 0x01, 0x02
#define MESSAGE_TIME_INVALID                                            \
    0x00, 0x00, 0x00, 14 + 2,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    't', 'i', 'm',                                                      \
    OBJ_TIME_INVALID

#define OBJ_TIME                                                        \
    0x0A, 0x31, 0x33, 0x32, 0x31, 0x39, 0x39, 0x33, 0x34, 0x35, 0x36
#define MESSAGE_TIME                                                    \
    0x00, 0x00, 0x00, 14 + 11,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    't', 'i', 'm',                                                      \
    OBJ_TIME

#define OBJ_HASHTABLE_INVALID_COUNT                                     \
    's', 't', 'r',                          /* str       */             \
    'i', 'n', 't',                          /* int       */             \
    0xFF, 0xFF, 0xFF, 0xFF,                 /* count: -1 */             \
    0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',  /* "abc"     */             \
    0x00, 0x00, 0x00, 0x01,                 /* 1         */             \
    0x00, 0x00, 0x00, 0x03, 'd', 'e', 'f',  /* "def"     */             \
    0x00, 0x00, 0x00, 0x02                  /* 2         */
#define MESSAGE_HASHTABLE_INVALID_COUNT                                 \
    0x00, 0x00, 0x00, 14 + 32,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'h', 't', 'b',                                                      \
    OBJ_HASHTABLE_INVALID_COUNT

#define OBJ_HASHTABLE                                                   \
    's', 't', 'r',                          /* str       */             \
    'i', 'n', 't',                          /* int       */             \
    0x00, 0x00, 0x00, 0x02,                 /* count: 2  */             \
    0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',  /* "abc"     */             \
    0x00, 0x00, 0x00, 0x01,                 /* 1         */             \
    0x00, 0x00, 0x00, 0x03, 'd', 'e', 'f',  /* "def"     */             \
    0x00, 0x00, 0x00, 0x02                  /* 2         */
#define MESSAGE_HASHTABLE                                               \
    0x00, 0x00, 0x00, 14 + 32,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'h', 't', 'b',                                                      \
    OBJ_HASHTABLE

#define OBJ_HDATA_INVALID_COUNT \
    0x00, 0x00, 0x00, 0x05, 'p', '1', '/', 'p', '2',  /* "p1/p2"   */   \
    0x00, 0x00, 0x00, 0x14,                                             \
    'k', '1', ':', 's', 't', 'r', ',',                /* k1:str,   */   \
    'k', '2', ':', 'i', 'n', 't', ',',                /* k2:int,   */   \
    'k', '3', ':', 'c', 'h', 'r',                     /* k3:chr    */   \
    0xFF, 0xFF, 0xFF, 0xFF                            /* count: -1 */
#define MESSAGE_HDATA_INVALID_COUNT                                     \
    0x00, 0x00, 0x00, 14 + 37,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'h', 'd', 'a',                                                      \
    OBJ_HDATA_INVALID_COUNT

#define OBJ_HDATA                                                       \
    0x00, 0x00, 0x00, 0x05, 'p', '1', '/', 'p', '2',  /* "p1/p2"  */    \
    0x00, 0x00, 0x00, 0x14,                                             \
    'k', '1', ':', 's', 't', 'r', ',',                /* k1:str,  */    \
    'k', '2', ':', 'i', 'n', 't', ',',                /* k2:int,  */    \
    'k', '3', ':', 'c', 'h', 'r',                     /* k3:chr   */    \
    0x00, 0x00, 0x00, 0x02,                           /* count: 2 */    \
    /* obj 1 */                                                         \
    0x03, '1', '2', '3',                              /* 0x123    */    \
    0x03, '4', '5', '6',                              /* 0x456    */    \
    0x00, 0x00, 0x00, 0x02, 'a', 'b',                 /* "ab"     */    \
    0x00, 0x00, 0x00, 0x05,                           /* 5        */    \
    'F',                                              /* 'F'      */    \
    /* obj 2 */                                                         \
    0x03, 'a', 'b', 'c',                              /* 0xabc    */    \
    0x03, 'd', 'e', 'f',                              /* 0xdef    */    \
    0x00, 0x00, 0x00, 0x02, 'x', 'y',                 /* "xy"     */    \
    0x00, 0x00, 0x00, 0x09,                           /* 9        */    \
    'X'                                               /* 'X'      */
#define MESSAGE_HDATA                                                   \
    0x00, 0x00, 0x00, 14 + 75,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'h', 'd', 'a',                                                      \
    OBJ_HDATA

#define OBJ_INFO                                                        \
    0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',  /* "abc" */                 \
    0x00, 0x00, 0x00, 0x03, 'd', 'e', 'f',  /* "def" */
#define MESSAGE_INFO                                                    \
    0x00, 0x00, 0x00, 14 + 14,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'i', 'n', 'f',                                                      \
    OBJ_INFO

#define OBJ_INFOLIST_INVALID_COUNT                                      \
    0x00, 0x00, 0x00, 0x04, 't', 'e', 's', 't',  /* name = "test" */    \
    0xFF, 0xFF, 0xFF, 0xFF,                      /* count = -1    */
#define MESSAGE_INFOLIST_INVALID_COUNT                                  \
    0x00, 0x00, 0x00, 14 + 12,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'i', 'n', 'l',                                                      \
    OBJ_INFOLIST_INVALID_COUNT

#define OBJ_INFOLIST_INVALID_ITEM_COUNT                                 \
    0x00, 0x00, 0x00, 0x04, 't', 'e', 's', 't',  /* name = "test" */    \
    0x00, 0x00, 0x00, 0x01,                      /* count = 1     */    \
    /* item 1 */                                                        \
    0xFF, 0xFF, 0xFF, 0xFF,                      /* count = -1    */
#define MESSAGE_INFOLIST_INVALID_ITEM_COUNT                             \
    0x00, 0x00, 0x00, 14 + 16,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'i', 'n', 'l',                                                      \
    OBJ_INFOLIST_INVALID_ITEM_COUNT

#define OBJ_INFOLIST                                                    \
    0x00, 0x00, 0x00, 0x04, 't', 'e', 's', 't',  /* name = "test" */    \
    0x00, 0x00, 0x00, 0x02,                      /* count = 2     */    \
    /* item 1 */                                                        \
    0x00, 0x00, 0x00, 0x01,                      /* count = 1     */    \
    0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',       /* name = "abc"  */    \
    'i', 'n', 't',                               /* int           */    \
    0x00, 0x00, 0x00, 0x08,                      /* 8             */    \
    /* item 2 */                                                        \
    0x00, 0x00, 0x00, 0x02,                      /* count = 2     */    \
    0x00, 0x00, 0x00, 0x03, 'd', 'e', 'f',       /* name = "def"  */    \
    'i', 'n', 't',                               /* int           */    \
    0x00, 0x00, 0x00, 0x04,                      /* 4             */    \
    0x00, 0x00, 0x00, 0x03, 'g', 'h', 'i',       /* name = "ghi"  */    \
    's', 't', 'r',                               /* str           */    \
    0xFF, 0xFF, 0xFF, 0xFF,                      /* NULL          */
#define MESSAGE_INFOLIST                                                \
    0x00, 0x00, 0x00, 14 + 62,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'i', 'n', 'l',                                                      \
    OBJ_INFOLIST

#define OBJ_ARRAY_INVALID_COUNT                                         \
    's', 't', 'r',           /* str       */                            \
    0xFF, 0xFF, 0xFF, 0xFF,  /* count: -1 */
#define MESSAGE_ARRAY_INVALID_COUNT                                     \
    0x00, 0x00, 0x00, 14 + 7,                                           \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'a', 'r', 'r',                                                      \
    OBJ_ARRAY_INVALID_COUNT

#define OBJ_ARRAY                                                       \
    's', 't', 'r',                          /* str       */             \
    0x00, 0x00, 0x00, 0x02,                 /* count: 2  */             \
    0x00, 0x00, 0x00, 0x03, 'a', 'b', 'c',  /* "abc"     */             \
    0x00, 0x00, 0x00, 0x03, 'd', 'e', 'f',  /* "def"     */
#define MESSAGE_ARRAY                                                   \
    0x00, 0x00, 0x00, 14 + 21,                                          \
    0x00,                                                               \
    0x00, 0x00, 0x00, 0x02, 'i', 'd',                                   \
    'a', 'r', 'r',                                                      \
    OBJ_ARRAY

#endif /* WEECHAT_RELAY_TESTS_H */
