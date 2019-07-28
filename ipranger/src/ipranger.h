/*
Copyright (C) 2019 Contributors to ipranger project.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

See ACKNOWLEDGEMENTS.md for further detials on licenses.
*/

#include "util.h"
#include <lmdb.h>

#ifndef IPRANGER
#define IPRANGER

#define IPRANGER_VERSION_MAJOR 0
#define IPRANGER_VERSION_MINOR 1
#define IPRANGER_VERSION_MICRO 0
#define IPRANGER_VERSION_TO_INT(a, b, c) (((a) << 24) | ((b) << 16) | (c))
#define IPRANGER_VERSION_INT                                                   \
  IPRANGER_VERSION_TO_INT(IPRANGER_VERSION_MAJOR, IPRANGER_VERSION_MINOR,      \
                          IPRANGER_VERSION_MICRO)

/**
 * Must be called before any DB operations.
 */
extern void init_DB_env();

/**
 * CIDR 45 chars, e.g. 3eed:ec3e:33dd:745c::/64
 * IDENTITY 32 chars, e.g. XXX-1
 */
extern int insert_cidr_identity_pair(const char *CIDR, const char *IDENTITY);

// extern int insert_cidr_identity_pairs(const char *CIDRS[],
//                                     const char *IDENTITIES[], int length);

extern char *get_identity_str(const char *ADDRESS);

// extern char **get_identity_strs(const char *ADDRESS[], int length);

extern void printf_db_dump();

// TODO:

// extern char *get_identity_in6_addr(struct in6_addr);

// extern char **get_identity_in6_addrs(struct in6_addr[], int length);

// extern char *get_identity_in_addr(struct in_addr);

// extern char **get_identity_in_addrs(struct in_addr[], int length);

/**
 * Must be called after all DB operations.
 */
extern void close_DB_env();

#endif