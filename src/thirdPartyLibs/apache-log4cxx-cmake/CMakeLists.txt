# Copyright (c) 2014, Ford Motor Company
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following
# disclaimer in the documentation and/or other materials provided with the
# distribution.
#
# Neither the name of the Ford Motor Company nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

set(LOG4CXX_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../apache-log4cxx-0.10.0)
set(LOG4CXX_BUILD_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../apache-log4cxx-build)
set(LOG4CXX_LIBS_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}/src/main/cpp/.libs)
set(LOG4CXX_LIBS_DIRECTORY ${LOG4CXX_LIBS_DIRECTORY} PARENT_SCOPE)
if(MODIFY_FUNCTION_FLAGS)
set(LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_SOURCE_DIRECTORY}/src/main/include)
set(LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_INCLUDE_DIRECTORY} PARENT_SCOPE)
set(LOG4CXX_BUILD_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}/src/main/include)
set(LOG4CXX_BUILD_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_INCLUDE_DIRECTORY} PARENT_SCOPE)
MESSAGE(STATUS "apache-log4cxx-0.10.0 LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_INCLUDE_DIRECTORY}")
MESSAGE(STATUS "apache-log4cxx-0.10.0 LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_INCLUDE_DIRECTORY}")
endif()

file(MAKE_DIRECTORY
  ${LOG4CXX_BUILD_DIRECTORY}
)

if(MODIFY_FUNCTION_FLAGS)
set(COMMON_CONFIGURE_FLAGS "--with-apr=../apr-build" "--with-apr-util=../apr-util-build" "--with-expat=${CMAKE_CURRENT_BINARY_DIR}/../expat-2.1.0")
else()
set(COMMON_CONFIGURE_FLAGS "--with-apr=../apr-build" "--with-apr-util=../apr-util-build")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
  if (CMAKE_SYSTEM_PROCESSOR MATCHES arm*)
    set (ARCHITECTURE arm) # log4cxx doesn't recognize armv7
  else ()
    set (ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
  endif ()
  set(CONFIGURE_FLAGS "--host=${ARCHITECTURE}-nto-qnx" ${COMMON_CONFIGURE_FLAGS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
  MESSAGE(STATUS "log4cxx-0.10.0 --host=arm")
  set(CONFIGURE_FLAGS "--host=arm" ${COMMON_CONFIGURE_FLAGS})
else()
  set(CONFIGURE_FLAGS ${COMMON_CONFIGURE_FLAGS})
endif()

add_custom_command(OUTPUT ${LOG4CXX_BUILD_DIRECTORY}/Makefile
  COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ${LOG4CXX_SOURCE_DIRECTORY}/configure ${CONFIGURE_FLAGS}
  DEPENDS libapr-1
  DEPENDS apr-util
  WORKING_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}
)

add_custom_target(liblog4cxx ALL make
  DEPENDS ${LOG4CXX_BUILD_DIRECTORY}/Makefile
  WORKING_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}
)

install(
  DIRECTORY ${LOG4CXX_LIBS_DIRECTORY}/
  DESTINATION ${3RD_PARTY_INSTALL_PREFIX}/lib
  USE_SOURCE_PERMISSIONS
  FILES_MATCHING PATTERN liblog4cxx.so*
)
