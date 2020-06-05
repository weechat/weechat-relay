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

#define MESSAGE_NOT_COMPRESSED                                          \
    0x00, 0x00, 0x00, 0xB5,                                             \
    0x00,                                                               \
    0xFF, 0xFF, 0xFF, 0xFF, 0x63, 0x68, 0x72, 0x41, 0x69, 0x6E,         \
    0x74, 0x00, 0x01, 0xE2, 0x40, 0x69, 0x6E, 0x74, 0xFF, 0xFE,         \
    0x1D, 0xC0, 0x6C, 0x6F, 0x6E, 0x0A, 0x31, 0x32, 0x33, 0x34,         \
    0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x6C, 0x6F, 0x6E, 0x0B,         \
    0x2D, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,         \
    0x30, 0x73, 0x74, 0x72, 0x00, 0x00, 0x00, 0x08, 0x61, 0x20,         \
    0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x73, 0x74, 0x72, 0x00,         \
    0x00, 0x00, 0x00, 0x73, 0x74, 0x72, 0xFF, 0xFF, 0xFF, 0xFF,         \
    0x62, 0x75, 0x66, 0x00, 0x00, 0x00, 0x06, 0x62, 0x75, 0x66,         \
    0x66, 0x65, 0x72, 0x62, 0x75, 0x66, 0xFF, 0xFF, 0xFF, 0xFF,         \
    0x70, 0x74, 0x72, 0x08, 0x31, 0x32, 0x33, 0x34, 0x61, 0x62,         \
    0x63, 0x64, 0x70, 0x74, 0x72, 0x01, 0x30, 0x74, 0x69, 0x6D,         \
    0x0A, 0x31, 0x33, 0x32, 0x31, 0x39, 0x39, 0x33, 0x34, 0x35,         \
    0x36, 0x61, 0x72, 0x72, 0x73, 0x74, 0x72, 0x00, 0x00, 0x00,         \
    0x02, 0x00, 0x00, 0x00, 0x03, 0x61, 0x62, 0x63, 0x00, 0x00,         \
    0x00, 0x02, 0x64, 0x65, 0x61, 0x72, 0x72, 0x69, 0x6E, 0x74,         \
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x7B, 0x00, 0x00,         \
    0x01, 0xC8, 0x00, 0x00, 0x03, 0x15

#define MESSAGE_COMPRESSED_ZLIB                                         \
    0x00, 0x00, 0x00, 0x91,                                             \
    0x01,                                                               \
    0x78, 0x9C, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xE4, 0x8C, 0x22,         \
    0xC7, 0xCC, 0xBC, 0x12, 0x06, 0xC6, 0x47, 0x0E, 0x40, 0xEA,         \
    0xFF, 0x3F, 0xD9, 0x03, 0x39, 0xF9, 0x79, 0x5C, 0x86, 0x46,         \
    0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x06, 0x40, 0x1E,         \
    0xB7, 0x2E, 0x82, 0x5B, 0x5C, 0x52, 0xC4, 0xC0, 0xC0, 0xC0,         \
    0x91, 0xA8, 0x00, 0x64, 0x64, 0xE6, 0xA5, 0x43, 0xB8, 0x0C,         \
    0x40, 0x0A, 0x68, 0xD4, 0xFF, 0xA4, 0xD2, 0x34, 0x20, 0x87,         \
    0x0D, 0x48, 0xA5, 0xA5, 0x16, 0x01, 0x49, 0x90, 0x58, 0x41,         \
    0x49, 0x11, 0x07, 0x48, 0x7B, 0x62, 0x52, 0x72, 0x0A, 0x90,         \
    0xCD, 0x68, 0x50, 0x92, 0x99, 0xCB, 0x65, 0x68, 0x6C, 0x64,         \
    0x68, 0x69, 0x09, 0x32, 0x33, 0xB1, 0xA8, 0x08, 0x62, 0x06,         \
    0x13, 0x10, 0x33, 0x03, 0x15, 0x81, 0x98, 0x29, 0xA9, 0x40,         \
    0x61, 0x90, 0xA3, 0x80, 0x42, 0x40, 0x5C, 0xCD, 0xC0, 0xC0,         \
    0x78, 0x02, 0xC8, 0x14, 0x05, 0x00, 0x3B, 0xD4, 0x38, 0x34

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
