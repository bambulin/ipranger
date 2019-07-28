# 
# Copyright (C) 2019 Contributors to ipranger project.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
# 
# See ACKNOWLEDGEMENTS.md for further detials on licenses.

include(FindPackageHandleStandardArgs)
find_library(LMDB_LIBRARIES NAMES lmdb PATHS "$ENV{LMDB_DIR}/lib")
find_path(LMDB_INCLUDE_DIR NAMES lmdb.h PATHS "$ENV{LMDB_DIR}/include")
find_package_handle_standard_args(LMDB DEFAULT_MSG LMDB_INCLUDE_DIR LMDB_LIBRARIES)
if(LMDB_FOUND)
  mark_as_advanced(LMDB_INCLUDE_DIR LMDB_LIBRARIES)
  message(STATUS "LMDB Found: library: ${LMDB_LIBRARIES} \t include: ${LMDB_INCLUDE_DIR}")
  set(LMDB_VERSION "${MDB_VERSION_MAJOR}.${MDB_VERSION_MINOR}.${MDB_VERSION_PATCH}")
  set(LMDB_VERSION_INT "${MDB_VERSION_FULL}")
else()
    message(STATUS "Fatal error: LMDB library was not found.")
endif()