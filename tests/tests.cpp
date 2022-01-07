/*
 * tests.cpp - run WeeChat Relay tests
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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "CppUTest/CommandLineTestRunner.h"

/* import tests from libs */

/* library */
IMPORT_TEST_GROUP(LibCommand);
IMPORT_TEST_GROUP(LibMessage);
IMPORT_TEST_GROUP(LibObject);
IMPORT_TEST_GROUP(LibParse);
IMPORT_TEST_GROUP(LibSession);

/* cli */
IMPORT_TEST_GROUP(SrcCli);
IMPORT_TEST_GROUP(SrcMessage);
IMPORT_TEST_GROUP(SrcNetwork);
IMPORT_TEST_GROUP(SrcUtil);


int
main (int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests (argc, argv);
}
