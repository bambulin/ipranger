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

#include "cli.h"
#include "ipranger.h"

#define DEFAULT_DB_DIR "./testdb"

void help(char *argv[]) {
  printf("IPRanger Usage: %s filename -c|-t <-l NUMBER>\n", argv[0]);
  printf("  or              \n");
  printf("                %s -h|--help\n", argv[0]);
  printf("filename: See README.md for details on structure.\n");
  printf("-c        Configures, writes data from text file to LMDB.\n");
  printf("-t        Tests, looks up data in LMDB and compares to expected "
         "values.\n");
  printf("-l NUMBER Optionally sets a loop of NUMBER repeats that does the -c "
         "or -t task again and again.\n");
  printf("Blame: <karm@email.cz>\n");
}

int main(int argc, char *argv[]) {

  apr_pool_t *pool;
  apr_pool_t *pool_data;
  apr_file_t *fp;

  char TEST_REGIME = 0;

  if (argc != 3) {
    help(argv);
    return APR_EGENERAL;
  }

  if (strncasecmp(argv[1], "-h", 2) == 0 ||
      strncasecmp(argv[1], "--help", 6) == 0) {
    help(argv);
    return APR_SUCCESS;
  }

  if (strncasecmp(argv[2], "-c", 2) == 0) {
    printf("Processing config file.\n");
    TEST_REGIME = 0;
  } else if (strncasecmp(argv[2], "-t", 2) == 0) {
    TEST_REGIME = 1;
    printf("Processing test file.\n");
  } else {
    help(argv);
    return APR_EGENERAL;
  }

  bool read_only = (TEST_REGIME) ? true : false;

  T(RC_SUCCESS == iprg_init_DB_env(DEFAULT_DB_DIR, read_only),
    "DB init failed.");

  T(APR_SUCCESS == apr_initialize(), "APR apr_initialize failed.");

  apr_pool_create(&pool_data, NULL);
  apr_pool_create(&pool, NULL);

  T(APR_SUCCESS == apr_file_open(&fp, argv[1], APR_READ | APR_BUFFERED,
                                 APR_FPROT_OS_DEFAULT, pool),
    "Failed to open file %s", argv[1]);

  char line[BUFSIZ + 1];
  int lineno;
  char *CIDR = NULL;
  char *IDENTITY = NULL;
  char *ADDRESS = NULL;

  for (lineno = 1; apr_file_gets(line, BUFSIZ, fp) == APR_SUCCESS; lineno++) {
    int ws_offset;
    char *last = line + strlen(line) - 1;

    while (last >= line && apr_isspace(*last)) {
      *last = '\0';
      --last;
    }

    ws_offset = 0;
    while (line[ws_offset] && apr_isspace(line[ws_offset])) {
      ws_offset++;
    }

    if (line[ws_offset] == 0) {
      printf("Warning: There was an empty line in your file. It might have "
             "broken the expected order of lines...\n");
      continue;
    }

    if (TEST_REGIME) {

      if (lineno % 2 == 1) {
        ADDRESS = apr_pstrndup(pool_data, line + ws_offset, 40);
      } else {
        IDENTITY = apr_pstrndup(pool_data, line + ws_offset, 32);

        ///// READ ONE BEGIN

        char *not_found_msg = "";
        char identity[32] = {0};

        T(RC_SUCCESS == iprg_get_identity_str(ADDRESS, identity),
          "Failed while getting identity for address %s", ADDRESS);

        if (identity == NULL) {
          not_found_msg = "NOT FOUND";
        }

        char *match_msg = " ";
        if (strncmp(identity, IDENTITY, 32) != 0) {
          match_msg = "!";
        }

        printf("\n%s Address: %s Found data: %s%s expected data: %s\n",
               match_msg, ADDRESS, identity, not_found_msg, IDENTITY);

        printf("\n");

        ///// READ ONE END
      }
    } else {

      // Load address
      if (lineno % 2 == 1) {
        CIDR = apr_pstrndup(pool_data, line + ws_offset, 45);

        // Process Address and Identity
      } else {
        IDENTITY = apr_pstrndup(pool_data, line + ws_offset, 32);
        T(RC_SUCCESS == iprg_insert_cidr_identity_pair(CIDR, IDENTITY),
          "Failed to insert CIDR %s, IDENTITY %s pair.", CIDR, IDENTITY);
      }
    }
  }

  apr_file_close(fp);
  apr_pool_destroy(pool);
  apr_pool_destroy(pool_data);
  apr_terminate();

  // Dump

  iprg_printf_db_dump();

  // Close DB env
  iprg_close_DB_env();
}
