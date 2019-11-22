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

#include "ipranger.h"
#include "util.h"

MDB_env *env;

extern iprg_stat_t iprg_init_DB_env(const char *path_to_db_dir,
                                    bool read_only) {
  int rc = 0;
  E(mdb_env_create(&env));
  E(mdb_env_set_mapsize(env, sysconf(_SC_PAGESIZE) *
                                 IPRANGER_MAX_MAP_SIZE_IN_PAGES));
  // 1 DB holds IPv6 ranges, 1 IPv6 masks, 1 IPv4 ranges and 1 IPv4 masks
  E(mdb_env_set_maxdbs(env, 4));
  int flags = MDB_FIXEDMAP | MDB_NOSYNC;
  if (read_only) {
    flags |= MDB_RDONLY;
  }
  E(mdb_env_open(env, path_to_db_dir, flags, 0664));
  return (rc == MDB_SUCCESS) ? RC_SUCCESS : RC_FAILURE;
}

extern void iprg_close_DB_env() { mdb_env_close(env); }

extern iprg_stat_t iprg_insert_cidr_identity_pair(const char *CIDR,
                                                  const char *IDENTITY) {

  char *start_ip = NULL;
  char *end_ip = NULL;
  char *mask = NULL;
  ip_range_t ip_range;

  cidr_to_ip(CIDR, &start_ip, &end_ip, &mask, &ip_range, NULL);

#ifdef DEBUG
  printf("CIDR:     %s\n", CIDR);
  printf("Start:    %s\n", start_ip);
  printf("End:      %s\n", end_ip);
  printf("Mask:     %s\n", mask);
  if (ip_range.type == IPv4) {
    printf("Type:     IPv4\n");
  } else {
    printf("Type:     IPv6\n");
  }
  printf("Identity: %s\n", IDENTITY);
#endif

  int rc = 0;

  MDB_dbi dbi_ipv6;
  MDB_dbi dbi_ipv6_masks;
  MDB_dbi dbi_ipv4;
  MDB_dbi dbi_ipv4_masks;
  MDB_txn *txn;

  if (ip_range.type == IPv6) {
    MDB_val key, data;

    struct in6_addr k_data;
    // This is the place where we decide whether we store first or last
    // address
    // k_data = ip_range.start6;
    k_data = ip_range.stop6;

    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, IPRANGER_IPv6_DB_NAME, MDB_CREATE | MDB_DUPSORT,
                   &dbi_ipv6));

    key.mv_size = sizeof(k_data);
    key.mv_data = &k_data;

    char v_data[IPRANGER_MAX_IDENTITY_LENGTH];
    memset(v_data, 0, sizeof(v_data));
    memcpy(v_data, IDENTITY, strlen(IDENTITY) + 1);
    data.mv_size = sizeof(v_data);
    data.mv_data = v_data;

    if (RES(MDB_KEYEXIST,
            mdb_put(txn, dbi_ipv6, &key, &data, MDB_NOOVERWRITE))) {
      memset(v_data, 0, sizeof(v_data));
      memcpy(v_data, IDENTITY, strlen(IDENTITY) + 1);
      data.mv_data = v_data;
      printf("Updating key: ");
      ipv6_to_str((const struct in6_addr *)key.mv_data);
      printf(" with data: %.*s\n", (int)data.mv_size, (char *)data.mv_data);
      // TODO Error handling
      mdb_del(txn, dbi_ipv6, &key, NULL);
      mdb_put(txn, dbi_ipv6, &key, &data, MDB_NODUPDATA);
    } else {
      printf("Inserting key: ");
      ipv6_to_str((const struct in6_addr *)key.mv_data);
      printf(" with data: %.*s\n", (int)data.mv_size, (char *)data.mv_data);
    }
    printf("-------------------------------------------------------------"
           "----"
           "\n");
    printf("-------------------------------------------------------------"
           "----"
           "\n");

    E(mdb_txn_commit(txn));
    mdb_dbi_close(env, dbi_ipv6);

  } else if (ip_range.type == IPv4) {
    MDB_val key, data;

    struct in_addr k_data;
    // This is the place where we decide whether we store first or last
    // address
    // k_data = ip_range.start;
    k_data = ip_range.stop;

    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, IPRANGER_IPv4_DB_NAME, MDB_CREATE | MDB_DUPSORT,
                   &dbi_ipv4));

    key.mv_size = sizeof(k_data);
    key.mv_data = &k_data;

    char v_data[IPRANGER_MAX_IDENTITY_LENGTH];
    memset(v_data, 0, sizeof(v_data));
    memcpy(v_data, IDENTITY, strlen(IDENTITY) + 1);
    data.mv_size = sizeof(v_data);
    data.mv_data = v_data;

    if (RES(MDB_KEYEXIST,
            mdb_put(txn, dbi_ipv4, &key, &data, MDB_NOOVERWRITE))) {
      memset(v_data, 0, sizeof(v_data));
      memcpy(v_data, IDENTITY, strlen(IDENTITY) + 1);
      data.mv_data = v_data;
      printf("Updating key: ");
      ipv4_to_str((const struct in_addr *)key.mv_data);
      printf(" with data: %.*s\n", (int)data.mv_size, (char *)data.mv_data);
      // TODO Error handling
      mdb_del(txn, dbi_ipv4, &key, NULL);
      mdb_put(txn, dbi_ipv4, &key, &data, MDB_NODUPDATA);
    } else {
      printf("Inserting key: ");
      ipv4_to_str((const struct in_addr *)key.mv_data);
      printf(" with data: %.*s\n", (int)data.mv_size, (char *)data.mv_data);
    }
    printf("-------------------------------------------------------------"
           "----"
           "\n");
    printf("-------------------------------------------------------------"
           "----"
           "\n");

    E(mdb_txn_commit(txn));
    mdb_dbi_close(env, dbi_ipv4);

  } else {
    CHECK(1, "Unexpected address type.");
  }

  // Remember the mask so as we know what our mask search space is
  MDB_txn *txn_masks;
  MDB_val key_mask, data_mask;
  char key_mask_k[4];
  char key_mask_d[4];
  memset(key_mask_k, 0, sizeof(key_mask_k));
  memcpy(key_mask_k, mask, strlen(mask) + 1);
  memset(key_mask_d, 0, sizeof(key_mask_d));
  memcpy(key_mask_d, mask, strlen(mask) + 1);
  E(mdb_txn_begin(env, NULL, 0, &txn_masks));

  if (ip_range.type == IPv6) {
    E(mdb_dbi_open(txn_masks, IPRANGER_IPv6_MASKS_DB_NAME,
                   MDB_CREATE | MDB_DUPSORT, &dbi_ipv6_masks));
  } else if (ip_range.type == IPv4) {
    E(mdb_dbi_open(txn_masks, IPRANGER_IPv4_MASKS_DB_NAME,
                   MDB_CREATE | MDB_DUPSORT, &dbi_ipv4_masks));
  } else {
    CHECK(1, "Unexpected address type.");
  }

  key_mask.mv_size = sizeof(key_mask_k);
  key_mask.mv_data = &key_mask_k;
  // See TODO below...
  data_mask.mv_size = sizeof(key_mask_d);
  data_mask.mv_data = &key_mask_d;

  if (ip_range.type == IPv6) {
    // TODO: Ad &key_mask used as data: we just need to know the masks, perhaps
    // we could have 1 key and multiple data items?
    if (RES(MDB_KEYEXIST, mdb_put(txn_masks, dbi_ipv6_masks, &key_mask,
                                  &data_mask, MDB_NODUPDATA))) {
      // Do nothing?
      // if (RES(MDB_SUCCESS, mdb_put(txn_masks, dbi_ipv6_masks, &key_mask,
      //                            &data_mask, MDB_CURRENT))) {
      // Silence
      // } else {
      // mdb_txn_abort(txn_masks);
      // mdb_dbi_close(env, dbi_ipv6);
      // mdb_dbi_close(env, dbi_ipv6_masks);
      // printf("Fatal DB error on storing used mask. Aborting the program.");
      // exit(666);
      //}
    }
    E(mdb_txn_commit(txn_masks));

    // If we want to read stats...
    // E(mdb_env_stat(env, &mst));
    mdb_dbi_close(env, dbi_ipv6_masks);
  } else if (ip_range.type == IPv4) {
    // TODO: Ad &key_mask used as data: we just need to know the masks, perhaps
    // we could have 1 key and multiple data items?
    if (RES(MDB_KEYEXIST, mdb_put(txn_masks, dbi_ipv4_masks, &key_mask,
                                  &data_mask, MDB_NODUPDATA))) {
      // Do nothing?
      // if (RES(MDB_SUCCESS, mdb_put(txn_masks, dbi_ipv4_masks, &key_mask,
      //                            &data_mask, MDB_CURRENT))) {
      // Silence
      // } else {
      // mdb_txn_abort(txn_masks);
      // mdb_dbi_close(env, dbi_ipv4);
      // mdb_dbi_close(env, dbi_ipv4_masks);
      // printf("Fatal DB error on storing used mask. Aborting the program.");
      // exit(666);
      //}
    }
    E(mdb_txn_commit(txn_masks));

    // If we want to read stats...
    // E(mdb_env_stat(env, &mst));
    mdb_dbi_close(env, dbi_ipv4_masks);
  } else {
    CHECK(1, "Unexpected address type.");
  }

  return (rc == MDB_SUCCESS) ? RC_SUCCESS : RC_FAILURE;
}

extern iprg_stat_t iprg_insert_cidr_identity_pairs(const char *cidrs[],
                                                   const char *identities[],
                                                   int length) {
  int rc = 0;
  // TODO this is really stupid. We should probably do multiple writes within
  // a single transaction in iprg_insert_cidr_identity_pair;
  for (int i = 0; i <= length; i++) {
    int r = iprg_insert_cidr_identity_pair(cidrs[i], identities[i]);
    if (r > rc) {
      rc = r;
    }
  }
  return rc;
}

extern iprg_stat_t iprg_get_identity_str(const char *address, char *identity) {
  int rc = 0;
  MDB_dbi dbi_ipv6;
  MDB_dbi dbi_ipv4;
  MDB_txn *txn;
  MDB_cursor *cursor;
  MDB_cursor *cursor_masks;

  char k_mask_data_r[4];
  char v_mask_data_r[4];
  MDB_dbi dbi_ipv6_masks;
  MDB_dbi dbi_ipv4_masks;
  MDB_txn *txn_masks;
  MDB_val key_mask_r, data_mask_r;
  key_mask_r.mv_size = sizeof(k_mask_data_r);
  key_mask_r.mv_data = &k_mask_data_r;
  data_mask_r.mv_size = sizeof(v_mask_data_r);
  data_mask_r.mv_data = v_mask_data_r;
  cursor = NULL;
  cursor_masks = NULL;

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn_masks));

  int family = strchr(address, ':') ? IPv6 : IPv4;

  if (family == IPv6) {
    E(mdb_dbi_open(txn_masks, IPRANGER_IPv6_MASKS_DB_NAME, MDB_DUPSORT,
                   &dbi_ipv6_masks));
    E(mdb_cursor_open(txn_masks, dbi_ipv6_masks, &cursor_masks));
  } else {
    E(mdb_dbi_open(txn_masks, IPRANGER_IPv4_MASKS_DB_NAME, MDB_DUPSORT,
                   &dbi_ipv4_masks));
    E(mdb_cursor_open(txn_masks, dbi_ipv4_masks, &cursor_masks));
  }

  char v_data_rr[IPRANGER_MAX_IDENTITY_LENGTH];
  MDB_val key_rr, data_rr;

  int i = 0;
  char *masks[IPRANGER_MAX_MASKS];

  while (((rc = mdb_cursor_get(cursor_masks, &key_mask_r, &data_mask_r,
                               MDB_NEXT)) == 0) &&
         i < IPRANGER_MAX_MASKS) {
    masks[i] = strdup(key_mask_r.mv_data);
    i++;
  }

  mdb_cursor_close(cursor_masks);
  mdb_txn_abort(txn_masks);
  if (family == IPv6) {
    mdb_dbi_close(env, dbi_ipv6_masks);
  } else {
    mdb_dbi_close(env, dbi_ipv4_masks);
  }

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
  if (family == IPv6) {
    E(mdb_dbi_open(txn, IPRANGER_IPv6_DB_NAME, MDB_DUPSORT, &dbi_ipv6));
  } else {
    E(mdb_dbi_open(txn, IPRANGER_IPv4_DB_NAME, MDB_DUPSORT, &dbi_ipv4));
  }
  // char *found = "NOT FOUND";

  if (family == IPv6) {
    struct in6_addr k_data_rr;
    for (int j = 0; j < i; j++) {

      char *start_ip_n = NULL;
      char *end_ip_n = NULL;
      char *mask_n = NULL;

      cursor = NULL;
      ip_range_t ip_range_n;

      cidr_to_ip(address, &start_ip_n, &end_ip_n, &mask_n, &ip_range_n,
                 masks[j]);
      memset(v_data_rr, 0, sizeof(v_data_rr));
      // This is the place where we decide whether we look for start or end
      k_data_rr = ip_range_n.stop6;

      key_rr.mv_size = sizeof(k_data_rr);
      key_rr.mv_data = &k_data_rr;
      data_rr.mv_size = sizeof(v_data_rr);
      data_rr.mv_data = v_data_rr;

      E(mdb_cursor_open(txn, dbi_ipv6, &cursor));

      // Exact match
      rc = mdb_cursor_get(cursor, &key_rr, &data_rr, MDB_SET_KEY);
      // Greater or equal than key given
      // rc = mdb_cursor_get(cursor, &key_rr, &data_rr, MDB_SET_RANGE);
      mdb_cursor_close(cursor);

      // CHECK(rc == MDB_SUCCESS, "mdb_cursor_get");
      if (rc == MDB_SUCCESS) {
        //  found = "";
        break;
      } else {
        printf("  Tried to match ");
        ipv6_to_str((const struct in6_addr *)key_rr.mv_data);
        printf(" key in vain (%s/%s)\n", address, masks[j]);
      }
    }

    printf("  Used key: %s Hit key: ", address);
    ipv6_to_str((const struct in6_addr *)key_rr.mv_data);
    printf("\n");
    // printf("\nFound data: %.*s%s\n", (int)data_rr.mv_size,
    //       (char *)data_rr.mv_data);
  } else if (family == IPv4) {

    struct in_addr k_data_rr;
    for (int j = 0; j < i; j++) {

      char *start_ip_n = NULL;
      char *end_ip_n = NULL;
      char *mask_n = NULL;

      cursor = NULL;
      ip_range_t ip_range_n;

      cidr_to_ip(address, &start_ip_n, &end_ip_n, &mask_n, &ip_range_n,
                 masks[j]);
      memset(v_data_rr, 0, sizeof(v_data_rr));
      // This is the place where we decide whether we look for start or end
      k_data_rr = ip_range_n.stop;

      key_rr.mv_size = sizeof(k_data_rr);
      key_rr.mv_data = &k_data_rr;
      data_rr.mv_size = sizeof(v_data_rr);
      data_rr.mv_data = v_data_rr;

      E(mdb_cursor_open(txn, dbi_ipv4, &cursor));

      // Exact match
      rc = mdb_cursor_get(cursor, &key_rr, &data_rr, MDB_SET_KEY);
      // Greater or equal than key given
      // rc = mdb_cursor_get(cursor, &key_rr, &data_rr, MDB_SET_RANGE);
      mdb_cursor_close(cursor);

      // CHECK(rc == MDB_SUCCESS, "mdb_cursor_get");
      if (rc == MDB_SUCCESS) {
        //  found = "";
        break;
      } else {
        printf("  Tried to match ");
        ipv4_to_str((const struct in_addr *)key_rr.mv_data);
        printf(" key in vain (%s/%s)\n", address, masks[j]);
      }
    }

    printf("  Used key: %s Hit key: ", address);
    ipv4_to_str((const struct in_addr *)key_rr.mv_data);
    printf("\n");
    // printf("\nFound data: %.*s%s\n", (int)data_rr.mv_size,
    //       (char *)data_rr.mv_data);
  } else {
    CHECK(1, "Unexpected address type.");
  }

  // TODO: This is clumsy way of handling the IPRANGER_NO_MATCH_STR. Make it
  // better.
  char *ret_identity = NULL;

  if (rc == MDB_SUCCESS) {
    ret_identity = strdup((char *)data_rr.mv_data);
  }

  if (ret_identity == NULL) {
    ret_identity = IPRANGER_NO_MATCH_STR;
  }

  strncpy(identity, ret_identity, IPRANGER_MAX_IDENTITY_LENGTH);

  mdb_txn_abort(txn);
  if (family == IPv6) {
    mdb_dbi_close(env, dbi_ipv6);
  } else {
    mdb_dbi_close(env, dbi_ipv4);
  }
  return RC_SUCCESS;
}

extern iprg_stat_t iprg_get_identity_strs(const char *addresses[],
                                          char *identities[], int length) {

  int rc = 0;
  // TODO this is really stupid. We should probably do multiple writes within
  // a single transaction in iprg_insert_cidr_identity_pair;
  for (int i = 0; i <= length; i++) {
    int r = iprg_get_identity_str(addresses[i], identities[i]);
    if (r > rc) {
      rc = r;
    }
  }

  return rc;
}

extern iprg_stat_t iprg_get_identity_ip_addr(struct ip_addr *address,
                                             char *identity) {
  if (address == NULL) {
    return RC_FAILURE;
  }

  if (address->family == AF_INET) {
    char iprg_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address->ipv4_sin_addr), iprg_address,
              INET_ADDRSTRLEN);
    return iprg_get_identity_str(iprg_address, identity);
  } else if (address->family == AF_INET6) {
    char iprg_address[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &(address->ipv6_sin_addr), iprg_address,
              INET6_ADDRSTRLEN);
    return iprg_get_identity_str(iprg_address, identity);
  } else {
    return RC_FAILURE;
  }
}

extern iprg_stat_t iprg_get_identity_ip_addrs(struct ip_addr *addresses[],
                                              char *identities[], int length) {
  int rc = 0;
  // TODO this is really stupid. We should probably do multiple writes within
  // a single transaction in iprg_insert_cidr_identity_pair;
  for (int i = 0; i <= length; i++) {
    int r = iprg_get_identity_ip_addr(addresses[i], identities[i]);
    if (r > rc) {
      rc = r;
    }
  }
  return rc;
}

extern iprg_stat_t iprg_check_ip_range(char *address, int *identity, ...) {
  int rc = 0;
  CHECK(1, "Not implemented.");
  return RC_FAILURE;
}

extern void iprg_printf_db_dump() {
  ipv6_db_dump();
  ipv4_db_dump();
}

void ipv6_db_dump() {
  int rc = 0;
  MDB_dbi dbi_ipv6;
  MDB_txn *txn;
  MDB_cursor *cursor;
  struct in6_addr k_data_r;
  char v_data_r[IPRANGER_MAX_IDENTITY_LENGTH];

  MDB_val key_r, data_r;
  key_r.mv_size = sizeof(k_data_r);
  key_r.mv_data = &k_data_r;
  data_r.mv_size = sizeof(v_data_r);
  data_r.mv_data = v_data_r;
  cursor = NULL;

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
  rc = mdb_dbi_open(txn, IPRANGER_IPv6_DB_NAME, MDB_DUPSORT, &dbi_ipv6);
  CHECK(rc != MDB_NOTFOUND, "No IPv6 DB configured.");
  CHECK(rc == MDB_SUCCESS, "Failed to open IPv6 DB.");
  E(mdb_cursor_open(txn, dbi_ipv6, &cursor));

  printf(
      "----------------IPv6 DB DUMP IN THE ORDER AS STORED-------------------"
      "\n");
  while ((rc = mdb_cursor_get(cursor, &key_r, &data_r, MDB_NEXT)) == 0) {
    printf("key: ");
    ipv6_to_str((const struct in6_addr *)key_r.mv_data);
    printf(" data: %.*s\n", (int)data_r.mv_size, (char *)data_r.mv_data);
  }
  CHECK(rc == MDB_NOTFOUND, "mdb_cursor_get");
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);
  mdb_dbi_close(env, dbi_ipv6);
}

void ipv4_db_dump() {
  int rc = 0;
  MDB_dbi dbi_ipv4;
  MDB_txn *txn;
  MDB_cursor *cursor;
  struct in_addr k_data_r;
  char v_data_r[IPRANGER_MAX_IDENTITY_LENGTH];

  MDB_val key_r, data_r;
  key_r.mv_size = sizeof(k_data_r);
  key_r.mv_data = &k_data_r;
  data_r.mv_size = sizeof(v_data_r);
  data_r.mv_data = v_data_r;
  cursor = NULL;

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
  rc = mdb_dbi_open(txn, IPRANGER_IPv4_DB_NAME, MDB_DUPSORT, &dbi_ipv4);
  CHECK(rc != MDB_NOTFOUND, "No IPv4 DB configured.");
  CHECK(rc == MDB_SUCCESS, "Failed to open IPv4 DB.");
  E(mdb_cursor_open(txn, dbi_ipv4, &cursor));

  printf(
      "----------------IPv4 DB DUMP IN THE ORDER AS STORED-------------------"
      "\n");
  while ((rc = mdb_cursor_get(cursor, &key_r, &data_r, MDB_NEXT)) == 0) {
    printf("key: ");
    ipv4_to_str((const struct in_addr *)key_r.mv_data);
    printf(" data: %.*s\n", (int)data_r.mv_size, (char *)data_r.mv_data);
  }
  CHECK(rc == MDB_NOTFOUND, "mdb_cursor_get");
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);
  mdb_dbi_close(env, dbi_ipv4);
}

#define IPRG_INIT_DB_ENV_JAVA(type, name) JNIEXPORT type JNICALL Java_io_whalebone_iprangerjava_IpRangerImpl_##name
IPRG_INIT_DB_ENV_JAVA(jint, initDb)(JNIEnv *e, jstring jpath_to_db_dir, jboolean read_only) {
    const char *path_to_db_dir = (*e)->GetStringUTFChars(e, jpath_to_db_dir, NULL);
    iprg_stat_t code = iprg_init_DB_env(path_to_db_dir, (bool) read_only);
    return (jint) code;
}
