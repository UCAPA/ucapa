# - Find Asio
# Find the Asio includes and libraries
#
# Following variables are provided:
# ASIO_FOUND
# True if Asio has been found
# ASIO_INCLUDE_DIR
# The include directories of Asio

find_path(ASIO_INCLUDE_DIR asio.hpp /usr/include /usr/local/include ./external ./external/include ./external/asio/include)

if (ASIO_INCLUDE_DIR)
   set(ASIO_FOUND TRUE)
endif (ASIO_INCLUDE_DIR)


if (ASIO_FOUND)
   if (NOT ASIO_FIND_QUIETLY)
      message(STATUS "Found ASIO: ${ASIO_INCLUDE_DIRS}")
   endif (NOT ASIO_FIND_QUIETLY)
else (ASIO_FOUND)
   if (ASIO_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Asio-lib")
   endif (ASIO_FIND_REQUIRED)
endif (ASIO_FOUND)
