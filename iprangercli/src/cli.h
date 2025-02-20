/*************************************************************************
 * Copyright (C) 2019 Contributors to ipranger project.                   *
 *                                                                        *
 * This program is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. *
 *                                                                        *
 * See ACKNOWLEDGEMENTS.md for further details on licenses.               *
 *************************************************************************/

#include <ipranger.h>

#include <ipranger.h>
#include <lmdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#ifndef IPRANGER_TEST
#define IPRANGER_TEST

#define DEFAULT_DB_DIR "./testdb"

#define LINE_LENGTH INET6_ADDRSTRLEN + 10

#define RET_ERROR 1
#define RET_SUCCESS 0
#endif
