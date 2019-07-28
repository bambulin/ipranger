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

# Note that APR and APR Util projects are merging into a one project called APR
# as of APR 2.x. The undermentioned macro is known to work only with APR and PAR util i.e. APR 1.x
# APR_INCLUDE_DIR: headers
# APR_LIBRARIES and APRUTIL_LIBRARIES: libs
# APR_FOUND and APRUTIL_FOUND: can / can not be used
# APR_LIBRARY and APRUTIL_LIBRARY: module local lib locations
find_path(APR_INCLUDE_DIR apr.h /usr/local/include/apr-1 /usr/local/include/apr-1.0 /usr/include/apr-1 /usr/include/apr-1.0)
set(APR_NAMES ${APR_NAMES} apr-1)
find_library(APR_LIBRARY NAMES ${APR_NAMES} PATHS /usr/lib /usr/local/lib)
if (APR_LIBRARY AND APR_INCLUDE_DIR)
set(APR_FOUND "YES")    
set(APR_LIBRARIES ${APR_LIBRARY})
else (APR_LIBRARY AND APR_INCLUDE_DIR)
    SET(APR_FOUND "NO")
endif (APR_LIBRARY AND APR_INCLUDE_DIR)
if (APR_FOUND)
    if (NOT APR_FIND_QUIETLY)
        MESSAGE(STATUS "APR lib found: ${APR_LIBRARIES}")
    endif (NOT APR_FIND_QUIETLY)
else (APR_FOUND)
    if (APR_FIND_REQUIRED)
        message(FATAL_ERROR "APR library could not be found")
    endif (APR_FIND_REQUIRED)
endif (APR_FOUND)
mark_as_advanced(APR_LIBRARY APR_INCLUDE_DIR)
find_path(APRUTIL_INCLUDE_DIR apu.h /usr/local/include/apr-1 /usr/local/include/apr-1.0 /usr/include/apr-1 /usr/include/apr-1.0)
set(APRUTIL_NAMES ${APRUTIL_NAMES} aprutil-1)
find_library(APRUTIL_LIBRARY
        NAMES ${APRUTIL_NAMES}
        PATHS /usr/lib /usr/local/lib
)
if (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
    set(APRUTIL_LIBRARIES ${APRUTIL_LIBRARY})
    set(APRUTIL_FOUND "YES")
else (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
    set(APRUTIL_FOUND "NO")
endif (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
if (APRUTIL_FOUND)
    if (NOT APRUTIL_FIND_QUIETLY)
        message(STATUS "Found APRUTIL: ${APRUTIL_LIBRARIES}")
    endif (NOT APRUTIL_FIND_QUIETLY)
else (APRUTIL_FOUND)
    if (APRUTIL_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find APRUTIL library")
    endif (APRUTIL_FIND_REQUIRED)
endif (APRUTIL_FOUND)
mark_as_advanced(APRUTIL_LIBRARY APRUTIL_INCLUDE_DIR)
