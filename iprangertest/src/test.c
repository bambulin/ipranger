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

#include "test.h"
#include "ipranger.h"

void basic_ipv6_test() {
  const char *identity = "X123456789X";
  const char *cidr = "3eed:ec3e:33dd:7400::/56";
  const char *address = "3eed:ec3e:33dd:745c:311f:a399:4345:2941";

  T(RC_SUCCESS == iprg_insert_cidr_identity_pair(cidr, identity),
    "Inserting identity failed.");

  char retrieved_identity[32] = {0};

  T(RC_SUCCESS == iprg_get_identity_str(address, retrieved_identity),
    "Retrieving identity failed.");

  T(0 == strncmp(identity, retrieved_identity, 32),
    "Unexpected identity retrieved.");
}

void basic_ipv4_test() {
  const char *identity_v4 = "XaaaaaX";
  const char *cidr_v4 = "10.1.1.0/25";
  const char *address_v4 = "10.1.1.13";

  T(RC_SUCCESS == iprg_insert_cidr_identity_pair(cidr_v4, identity_v4),
    "Inserting identity failed.");

  char retrieved_identity[32] = {0};

  T(RC_SUCCESS == iprg_get_identity_str(address_v4, retrieved_identity),
    "Retrieving identity failed.");

  T(0 == strncmp(identity_v4, retrieved_identity, 32),
    "Unexpected identity retrieved.");
}

int main(void) {
  printf(DB_DIR_MSG "\n");
  remove(DEFAULT_DB_DIR "/data.mdb");
  remove(DEFAULT_DB_DIR "/lock.mdb");
  const bool read_only = false;

  //mkdir("testdb", 0);
  T(RC_SUCCESS == iprg_init_DB_env(DEFAULT_DB_DIR, read_only),
    "DB init failed.");

  char value[32] = {0};
  iprg_insert_string("test", "key", "value");
  iprg_select_string("test", "key", &value, sizeof(value));
  //printf("test %s", value);
  iprg_delete_by_value_string("test", "value");
  iprg_delete_by_key_string("test", "key");

  basic_ipv6_test();

  printf("\n");

  basic_ipv4_test();

  printf("\n\n");

  iprg_printf_db_dump();

  iprg_close_DB_env();
}
