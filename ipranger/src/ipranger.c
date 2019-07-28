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

#include "ipranger.h"
#include "util.h"

MDB_env *env;

extern void init_DB_env() {
  int rc = 0;
  E(mdb_env_create(&env));
  E(mdb_env_set_mapsize(env, 4 * 1024 * 1024));
  E(mdb_env_set_maxdbs(env, 2));
  E(mdb_env_open(env, "./testdb", MDB_FIXEDMAP | MDB_NOSYNC, 0664));
}

extern void close_DB_env() { mdb_env_close(env); }

extern int insert_cidr_identity_pair(const char *CIDR, const char *IDENTITY) {
  int rc = 0;
  MDB_dbi dbi_ipv6;
  MDB_dbi dbi_ipv6_masks;
  MDB_val key, data, key_mask, data_mask;
  MDB_txn *txn;
  MDB_stat mst;

  char *start_ip = NULL;
  char *end_ip = NULL;
  char *mask = NULL;
  ip_range_t ip_range;

  ipnacstun_cidr_to_ip(CIDR, &start_ip, &end_ip, &mask, &ip_range, NULL);
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

  struct in6_addr k_data;
  char v_data[32];

  memset(v_data, 0, sizeof(v_data));

  // This is the place where we decide whether we store first or last
  // address
  // k_data = ip_range.start6;
  k_data = ip_range.stop6;

  memcpy(v_data, IDENTITY, strlen(IDENTITY) + 1);

  E(mdb_txn_begin(env, NULL, 0, &txn));
  E(mdb_dbi_open(txn, "IPv6", MDB_CREATE | MDB_DUPSORT, &dbi_ipv6));

  key.mv_size = sizeof(k_data);
  key.mv_data = &k_data;
  data.mv_size = sizeof(v_data);
  data.mv_data = v_data;

  // TODO: Change to:
  ///** For mdb_cursor_put: overwrite the current key/data pair */
  //#define MDB_CURRENT	0x40

  if (RES(MDB_KEYEXIST, mdb_put(txn, dbi_ipv6, &key, &data, MDB_NODUPDATA))) {
    printf("Updating key: ");
    ipv6_to_str_unexpanded((const struct in6_addr *)key.mv_data);
    printf(" with data: %.*s\n", (int)data.mv_size, (char *)data.mv_data);
    if (RES(MDB_NOTFOUND, mdb_del(txn, dbi_ipv6, &key, &data))) {
      mdb_txn_abort(txn);
      // TODO something?  What is this state? Somebody delted the key in
      // the meantime?
      exit(666);
    }
    if (RES(MDB_KEYEXIST, mdb_put(txn, dbi_ipv6, &key, &data, MDB_NODUPDATA))) {
      mdb_txn_abort(txn);
      // TODO something?  What is this state? Somebody delted the key in
      // the meantime?
      exit(666);
    }
  } else {
    printf("Inserting key: ");
    ipv6_to_str_unexpanded((const struct in6_addr *)key.mv_data);
    printf(" with data: %.*s\n", (int)data.mv_size, (char *)data.mv_data);
  }
  printf("-------------------------------------------------------------"
         "----"
         "\n");
  printf("-------------------------------------------------------------"
         "----"
         "\n");

  E(mdb_txn_commit(txn));
  // TODO: Do we want to read anything from stats...?

  // Remember the mask so as we know what our mask search space is
  MDB_txn *txn_masks;
  char key_mask_k[4];
  char key_mask_d[4];
  memset(key_mask_k, 0, sizeof(key_mask_k));
  memcpy(key_mask_k, mask, strlen(mask) + 1);
  memset(key_mask_d, 0, sizeof(key_mask_d));
  memcpy(key_mask_d, mask, strlen(mask) + 1);
  E(mdb_txn_begin(env, NULL, 0, &txn_masks));
  E(mdb_dbi_open(txn_masks, "IPv6_masks", MDB_CREATE | MDB_DUPSORT,
                 &dbi_ipv6_masks));
  key_mask.mv_size = sizeof(key_mask_k);
  key_mask.mv_data = &key_mask_k;
  // See TODo below...
  data_mask.mv_size = sizeof(key_mask_d);
  data_mask.mv_data = &key_mask_d;

  // TODO: Ad &key_mask used as data: we just need to know the masks, perhaps we
  // could have 1 key and multiple data items?
  if (RES(MDB_KEYEXIST, mdb_put(txn_masks, dbi_ipv6_masks, &key_mask,
                                &data_mask, MDB_NODUPDATA))) {
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

  E(mdb_env_stat(env, &mst));
  mdb_dbi_close(env, dbi_ipv6);
  mdb_dbi_close(env, dbi_ipv6_masks);
  return rc;
}

/*
extern int insert_cidr_identity_pairs(const char *CIDRS[],
                                      const char *IDENTITIES[], int length) {
  int rc = 0;
  for (int i = 0; i <= length; i++) {
    int r = insert_cidr_identity_pair(CIDRS[i], IDENTITIES[i]);
    if (r > rc) {
      rc = r;
    }
  }
  return rc;
}
*/

extern char *get_identity_str(const char *ADDRESS) {
  int rc = 0;
  MDB_dbi dbi_ipv6;
  MDB_txn *txn;
  MDB_cursor *cursor;
  MDB_cursor *cursor_masks;

  char k_mask_data_r[4];
  char v_mask_data_r[4];
  MDB_dbi dbi_ipv6_masks;
  MDB_txn *txn_masks;
  MDB_val key_mask_r, data_mask_r;
  key_mask_r.mv_size = sizeof(k_mask_data_r);
  key_mask_r.mv_data = &k_mask_data_r;
  data_mask_r.mv_size = sizeof(v_mask_data_r);
  data_mask_r.mv_data = v_mask_data_r;
  cursor = NULL;
  cursor_masks = NULL;

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn_masks));
  E(mdb_dbi_open(txn_masks, "IPv6_masks", MDB_DUPSORT, &dbi_ipv6_masks));
  E(mdb_cursor_open(txn_masks, dbi_ipv6_masks, &cursor_masks));

  // char *found = "NOT FOUND";
  struct in6_addr k_data_rr;
  char v_data_rr[32];
  MDB_val key_rr, data_rr;

  int i = 0;
  char *masks[33];

  while ((rc = mdb_cursor_get(cursor_masks, &key_mask_r, &data_mask_r,
                              MDB_NEXT)) == 0) {
    masks[i] = strdup(key_mask_r.mv_data);
    i++;
  }

  mdb_cursor_close(cursor_masks);
  mdb_txn_abort(txn_masks);
  mdb_dbi_close(env, dbi_ipv6_masks);

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
  E(mdb_dbi_open(txn, "IPv6", MDB_DUPSORT, &dbi_ipv6));

  for (int j = 0; j < i; j++) {

    char *start_ip_n = NULL;
    char *end_ip_n = NULL;
    char *mask_n = NULL;

    cursor = NULL;
    ip_range_t ip_range_n;

    ipnacstun_cidr_to_ip(ADDRESS, &start_ip_n, &end_ip_n, &mask_n, &ip_range_n,
                         masks[j]);
    memset(v_data_rr, 0, sizeof(v_data_rr));
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
      ipv6_to_str_unexpanded((const struct in6_addr *)key_rr.mv_data);
      printf(" key in vain (%s/%s)\n", ADDRESS, masks[j]);
    }
  }

  printf("  Used key: %s Hit key: ", ADDRESS);
  ipv6_to_str_unexpanded((const struct in6_addr *)key_rr.mv_data);
  // printf("\nFound data: %.*s%s\n", (int)data_rr.mv_size,
  //       (char *)data_rr.mv_data);

  char *ret_identity = NULL;

  if (rc == MDB_SUCCESS) {
    ret_identity = strdup((char *)data_rr.mv_data);
  }

  if (ret_identity == NULL) {
    ret_identity = "NULL";
  }

  mdb_txn_abort(txn);
  mdb_dbi_close(env, dbi_ipv6);
  return ret_identity;
}

/*
extern char **get_identity_strs(const char *ADDRESS[], int length) {
  char *identities[length];
  for (int i = 0; i < length; i++) {
    identities[i] = get_identity_str(ADDRESS[i]);
  }
  return identities;
}*/

extern void printf_db_dump() {
  int rc = 0;
  MDB_dbi dbi_ipv6;
  MDB_txn *txn;
  MDB_cursor *cursor;
  struct in6_addr k_data_r;
  char v_data_r[32];

  MDB_val key_r, data_r;
  key_r.mv_size = sizeof(k_data_r);
  key_r.mv_data = &k_data_r;
  data_r.mv_size = sizeof(v_data_r);
  data_r.mv_data = v_data_r;
  cursor = NULL;

  E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
  E(mdb_dbi_open(txn, "IPv6", MDB_DUPSORT, &dbi_ipv6));
  E(mdb_cursor_open(txn, dbi_ipv6, &cursor));

  printf("----------------DB DUMP IN THE ORDER AS STORED-------------------"
         "\n");
  while ((rc = mdb_cursor_get(cursor, &key_r, &data_r, MDB_NEXT)) == 0) {
    printf("key: ");
    ipv6_to_str_unexpanded((const struct in6_addr *)key_r.mv_data);
    printf(" data: %.*s\n", (int)data_r.mv_size, (char *)data_r.mv_data);
  }
  CHECK(rc == MDB_NOTFOUND, "mdb_cursor_get");
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);
  mdb_dbi_close(env, dbi_ipv6);
}