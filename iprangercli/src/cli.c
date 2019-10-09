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

  bool TEST_REGIME = false;

  if (argc != 3) {
    help(argv);
    return RET_ERROR;
  }

  if (strncasecmp(argv[1], "-h", 2) == 0 ||
      strncasecmp(argv[1], "--help", 6) == 0) {
    help(argv);
    return RET_SUCCESS;
  }

  if (strncasecmp(argv[2], "-c", 2) == 0) {
    printf("Processing config file.\n");
    TEST_REGIME = false;
  } else if (strncasecmp(argv[2], "-t", 2) == 0) {
    TEST_REGIME = true;
    printf("Processing test file.\n");
  } else {
    help(argv);
    return RET_ERROR;
  }

  bool read_only = (TEST_REGIME) ? true : false;

  T(RC_SUCCESS == iprg_init_DB_env(DEFAULT_DB_DIR, read_only),
    "DB init failed.");

  FILE *stream = fopen(argv[1], "r");
  char line[LINE_LENGTH];

  if (stream == NULL) {
    printf("There was an error opening %s\n", argv[1]);
    return RET_ERROR;
  }

  int linecount = 0;
  while (fgets(line, LINE_LENGTH, stream)) {
    linecount++;
  }

  if (linecount < 2) {
    printf("Error: %s seems to contain no IP address Identity pair.\n",
           argv[1]);
    return RET_ERROR;
  }
  if (linecount % 2 != 0) {
    printf("Error: %s seems to contain odd number of lines. That is not "
           "expected.\n",
           argv[1]);
    return RET_ERROR;
  }

  fseek(stream, 0, SEEK_SET);
  memset(line, 0, LINE_LENGTH);

  char CIDR[INET6_ADDRSTRLEN];
  char IDENTITY[IPRANGER_MAX_IDENTITY_LENGTH];
  char ADDRESS[INET6_ADDRSTRLEN];

  for (linecount = 1; fgets(line, LINE_LENGTH, stream); linecount++) {
    if (TEST_REGIME) {
      if (linecount % 2 == 1) {
        memset(ADDRESS, 0, INET6_ADDRSTRLEN);
        int len = strlen(line);
        strncpy(ADDRESS, line,
                (len <= INET6_ADDRSTRLEN) ? len - 1 : INET6_ADDRSTRLEN - 1);
      } else {
        memset(IDENTITY, 0, IPRANGER_MAX_IDENTITY_LENGTH);
        int len = strlen(line);
        strncpy(IDENTITY, line,
                (len <= IPRANGER_MAX_IDENTITY_LENGTH)
                    ? len - 1
                    : IPRANGER_MAX_IDENTITY_LENGTH - 1);

        ///// READ ONE BEGIN

        char *not_found_msg = "";
        char identity[IPRANGER_MAX_IDENTITY_LENGTH] = {0};

        T(RC_SUCCESS == iprg_get_identity_str(ADDRESS, identity),
          "Failed while getting identity for address %s", ADDRESS);

        if (identity == NULL) {
          not_found_msg = "NOT FOUND";
        }

        char *match_msg = " ";
        if (strncmp(identity, IDENTITY, IPRANGER_MAX_IDENTITY_LENGTH) != 0) {
          match_msg = "!";
        }

        printf("\n%s Address: %s Found data: %s%s expected data: %s\n",
               match_msg, ADDRESS, identity, not_found_msg, IDENTITY);

        printf("\n");

        ///// READ ONE END
      }
    } else {
      if (linecount % 2 == 1) {
        // Process Address and Identity
        memset(IDENTITY, 0, IPRANGER_MAX_IDENTITY_LENGTH);
        int len = strlen(line);
        strncpy(IDENTITY, line,
                (len <= IPRANGER_MAX_IDENTITY_LENGTH)
                    ? len - 1
                    : IPRANGER_MAX_IDENTITY_LENGTH - 1);
      } else {
        // Load address
        memset(CIDR, 0, INET6_ADDRSTRLEN);
        memset(CIDR, 0, INET6_ADDRSTRLEN);
        int len = strlen(line);
        strncpy(CIDR, line,
                (len <= INET6_ADDRSTRLEN) ? len - 1 : INET6_ADDRSTRLEN - 1);
        T(RC_SUCCESS == iprg_insert_cidr_identity_pair(CIDR, IDENTITY),
          "Failed to insert CIDR %s, IDENTITY %s pair.", CIDR, IDENTITY);
      }
    }
  }
  // Dump
  iprg_printf_db_dump();
  // Close DB env
  iprg_close_DB_env();
}
