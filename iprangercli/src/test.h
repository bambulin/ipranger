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
*/

#include <ipranger.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <apr_file_io.h>
#include <apr_lib.h>
#include <apr_pools.h>
#include <apr_strings.h>
#include <lmdb.h>

#ifndef IPRANGER_TEST
#define IPRANGER_TEST

// Moved to util.h
// void ipv6_to_str_unexpanded(const struct in6_addr *addr);

#endif