# - Find FFMPEG
# Find the FFMPEG includes and libraries
#
# Following variables are provided:
# FFMPEG_FOUND
# True if FFMPEG has been found
# FFMPEG_INCLUDE_DIR
# The include directories of FFMPEG
# FFMPEG_LIB_DIR
# The libs directories of FFMPEG

# Try to find headers
find_path(TMP_FFMPEG_INCLUDE_DIR
			libavcodec/avcodec.h
			/usr/include /usr/local/include /opt/local/include /sw/include
			/external ./external/include ./external/ffmpeg/include
		)

# Try to find libs
#avcodec
find_library(TMP_FFMPEG_AVCODEC_LIB_DIR
				avcodec
				/usr/lib /usr/local/lib /opt/local/lib /sw/lib
				./external ./external/lib ./external/ffmpeg/lib
			)

#avformat
find_library(TMP_FFMPEG_AVFORMAT_LIB_DIR
				avformat
				/usr/lib /usr/local/lib /opt/local/lib /sw/lib
				./external ./external/lib ./external/ffmpeg/lib
			)

#avutil
find_library(TMP_FFMPEG_AVUTIL_LIB_DIR
				avutil
				/usr/lib /usr/local/lib /opt/local/lib /sw/lib
				./external ./external/lib ./external/ffmpeg/lib
			)

#swscale
find_library(TMP_FFMPEG_SWSCALE_LIB_DIR
				swscale
				/usr/lib /usr/local/lib /opt/local/lib /sw/lib
				./external ./external/lib ./external/ffmpeg/lib
			)

# Test if we have found all required stuff
if (TMP_FFMPEG_INCLUDE_DIR AND TMP_FFMPEG_AVCODEC_LIB_DIR AND TMP_FFMPEG_AVFORMAT_LIB_DIR AND TMP_FFMPEG_AVUTIL_LIB_DIR AND TMP_FFMPEG_SWSCALE_LIB_DIR)
   set(FFMPEG_FOUND TRUE)
   set(FFMPEG_INCLUDE_DIR ${TMP_FFMPEG_INCLUDE_DIR})
   set(FFMPEG_LIB_DIR ${TMP_FFMPEG_AVCODEC_LIB_DIR} ${TMP_FFMPEG_AVFORMAT_LIB_DIR} ${TMP_FFMPEG_AVUTIL_LIB_DIR} ${TMP_FFMPEG_SWSCALE_LIB_DIR})
endif()


if (FFMPEG_FOUND)
	if (NOT FFMPEG_FIND_QUIETLY)
		message(STATUS "Found FFMPEG or Libav: ${FFMPEG_LIB_DIR}, ${FFMPEG_INCLUDE_DIR}")
	endif (NOT FFMPEG_FIND_QUIETLY)
else(FFMPEG_FOUND)
	if (FFMPEG_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find FFMPEG")
	endif (FFMPEG_FIND_REQUIRED)
endif (FFMPEG_FOUND)
