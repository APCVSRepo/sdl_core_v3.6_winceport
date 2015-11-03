# Copyright (c) 2013, Ford Motor Company
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

cmake_minimum_required(VERSION 2.8.0)

set (PROJECT smartDeviceLinkCore)

#compiler set
#message(STATUS ${CMAKE_SYSTEM_NAME})
if(SYSTEM_NAME STREQUAL "Android")
SET(CMAKE_SYSTEM_NAME Android)
endif()

#when build on android plat,you should set -DCOMPILER_PATH=your compiler root path
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
if(NOT COMPILER_PATH)
SET(COMPILER_PATH "/opt/android-9-toolchain")
endif()
message(STATUS "Current cross compiler path is ${COMPILER_PATH}")
SET(CMAKE_CXX_COMPILER "${COMPILER_PATH}/bin/arm-linux-androideabi-g++")
SET(CMAKE_C_COMPILER "${COMPILER_PATH}/bin/arm-linux-androideabi-gcc")
endif()

message(STATUS "Current build system name is ${CMAKE_SYSTEM_NAME}")
message(STATUS "Current CMAKE_BUIDL_TYPE is ${CMAKE_BUIDL_TYPE}")

#Jenkins integration section
#dont modify this section if you don't know details about integration with Jenkins!!!
set (HMI "web" CACHE STRING "HMI type")
option(HMI2 "Use Qt HMI" OFF)
option(EXTENDED_MEDIA_MODE "Turn on and off extended Madia Manager features relates to PulseAudio A2DP and GStreamer" OFF)
option(BUILD_SHARED_LIBS "Build all libraries as shared (if ON) or static (if OFF)" OFF)
option(BUILD_BT_SUPPORT "Bluetooth support" ON)
option(BUILD_USB_SUPPORT "libusb support" ON)
option(BUILD_AVAHI_SUPPORT "libavahi support" ON)
option(BUILD_RWLOCK_SUPPORT "rwlocks support" OFF)
option(BUILD_BACKTRACE_SUPPORT "backtrace support" ON)
option(BUILD_TESTS "Possibility to build and run tests" OFF)
option(TIME_TESTER "Enable profiling time test util" OFF)
option(BUILD_TESTS_WITH_HMI "Possibility to build and run HMI tests" OFF)
option(ENABLE_LOG "Logging feature" ON)
option(ENABLE_GCOV "gcov code coverage feature" OFF)
option(EXTENDED_POLICY_FLAG "Build with specific features and extended functionality" OFF)
option(MODIFY_FUNCTION_FLAGS "modify function" ON)
if(MODIFY_FUNCTION_FLAGS)
option(BUILD_MANAGESDL_SUPPORT "manage sdl support" OFF)
endif()
if(MODIFY_FUNCTION_FLAGS)
option(BUILD_TARGET_LIBRARY    "build target as library" OFF)
endif()

set(OS_TYPE_OPTION 	"$ENV{OS_TYPE}")
set(DEBUG_OPTION 	"$ENV{DEBUG}")
set(HMI_TYPE_OPTION 	"$ENV{HMI_TYPE}")
set(TARGET_OPTION 	"$ENV{TARGET}")
set(MEDIA_MODE_OPTION 	"$ENV{MEDIA_MODE}")
set(HMI_ADAPTER_OPTION 	"$ENV{HMI_ADAPTER}")
set(ENABLE_LOG_OPTION 	"$ENV{ENABLE_LOG}")

if (OS_TYPE_OPTION)
  if (${OS_TYPE_OPTION} STREQUAL "QNX")
    message(STATUS "Jenkins integration: set build process for QNX")
    #do not use include after project() command.
    #Such usecase results in infinite cycle of reinitialization of compiler and other variables
    INCLUDE("./qnx_6.5.0_linux_x86.cmake")
    #tests are not supported yet for QNX build
    set (BUILD_TESTS OFF)
  endif()
endif()

if (HMI_TYPE_OPTION)
  if (${HMI_TYPE_OPTION} STREQUAL "HTML5")
    message(STATUS "Jenkins integration: select HTML5 HMI")
    set (HMI "web")
  elseif (${HMI_TYPE_OPTION} STREQUAL "NONE")
    message(STATUS "Jenkins integration: select HMI none")
    set (HMI "no")
  else ()
    message(STATUS "Jenkins integration: select QML HMI none")
    set (HMI "qt")
  endif()
endif()

if (MEDIA_MODE_OPTION)
  if (${MEDIA_MODE_OPTION} STREQUAL "EXTENDED_MEDIA")
    message(STATUS "Jenkins integration: select extended media mode")
    set (EXTENDED_MEDIA_MODE ON)
  endif()
endif()

if (DEBUG_OPTION)
  if (${DEBUG_OPTION} STREQUAL "DBG_OFF")
    message(STATUS "Jenkins integration: build release version")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

if (HMI_ADAPTER_OPTION)
  if (${HMI_ADAPTER_OPTION} STREQUAL "MESSAGEBROKER")
    message(STATUS "Jenkins integration: selected HMI adapter MESSAGEBROKER")
    set (HMIADAPTER "messagebroker")
  elseif (${HMI_ADAPTER_OPTION} STREQUAL "DBUS")
    message(STATUS "Jenkins integration: selected HMI adapter DBUS")
    set (HMIADAPTER "dbus")
  elseif (${HMI_ADAPTER_OPTION} STREQUAL "MQUEUE")
    message(STATUS "Jenkins integration: selected HMI adapter MQUEUE")
    set (HMIADAPTER "mqueue")
  endif()
endif()

if (ENABLE_LOG_OPTION)
  if (${ENABLE_LOG_OPTION} STREQUAL "LOG_OFF")
    message(STATUS "Jenkins integration: Log is turned off")
    set (ENABLE_LOG OFF)
  endif()
endif()

#Jenkins integration section end

project (${PROJECT})

#ADD_DEPENDENCIES(${PROJECT} Policy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Please do not change compiler/linker flags if You do not know how particular
# flag is handled by CMake
set(CMAKE_INSTALL_PREFIX .)
set(ARCHIVE_OUTPUT_DIRECTORY ./bin)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL  "Android")
set(CMAKE_CXX_FLAGS "-fPIC -g3 -ggdb3 -std=gnu++0x -Werror=return-type -Wuninitialized")
if (ENABLE_GCOV)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  add_definitions(-DGCOV_ENABLED)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -s -O2") #It will be appended to CMAKE_CXX_FLAGS in release
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
set(CMAKE_CXX_FLAGS "-g3 -ggdb3 -std=gnu++0x -Werror=return-type -Wuninitialized --coverage -std=c++0x") # -stdlib=libc++ -m32
set(CMAKE_CXX_FLAGS_RELEASE "-s -O2") #It will be appended to CMAKE_CXX_FLAGS in release
#set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(CMAKE_CXX_FLAGS "/wd\"4819\" /wd\"4244\" /wd\"4390\" /EHsc")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG") #It will be appended to CMAKE_CXX_FLAGS in release
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
set(CMAKE_CXX_FLAGS "/wd\"4819\" /wd\"4244\" /wd\"4390\" /wd\"4996\" /EHsc")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG") #It will be appended to CMAKE_CXX_FLAGS in release
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_definitions(-DOS_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "QNX")
  add_definitions(-DOS_QNX)
  SET(BUILD_BT_SUPPORT OFF)
  SET(BUILD_AVAHI_SUPPORT OFF)
  SET(BUILD_BACKTRACE_SUPPORT OFF)
  SET(EXTENDED_MEDIA_MODE OFF)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_definitions(-DOS_WIN32)
  add_definitions(-DWIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-DASR_HAS_VR)
  SET(BUILD_AVAHI_SUPPORT OFF)
  SET(BUILD_USB_SUPPORT OFF)
  SET(BUILD_BT_SUPPORT OFF)
  SET(EXTENDED_MEDIA_MODE OFF)
  SET(ENABLE_LOG ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
  add_definitions(-D_WIN32_WCE=$(CEVER))
  add_definitions(-D_WINDOWS)
  add_definitions(-DUNDER_CE)
  add_definitions(-D$(ARCHFAM))
  add_definitions(-D$(_ARCHFAM_))
  add_definitions(-D$(PLATFORMDEFINES))
  add_definitions(-DOS_WINCE)
  add_definitions(-DOS_WIN32)
  add_definitions(-DHAVE_PTW32_CONFIG_H)
  add_definitions(-D_UNICODE)
  add_definitions(-DUNICODE)
  add_definitions(-DWINCE)
  add_definitions(-D_WIN32)
  add_definitions(-DLOG4CXX_STATIC)
add_definitions(-DAPR_DECLARE_STATIC)
  SET(ENABLE_LOG ON)
  SET(BUILD_BT_SUPPORT ON)
  SET(BUILD_USB_SUPPORT ON)
  SET(BUILD_AVAHI_SUPPORT OFF)
  SET(BUILD_BACKTRACE_SUPPORT OFF)
  SET(EXTENDED_MEDIA_MODE OFF)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  add_definitions(-DOS_MAC)
  SET(BUILD_BT_SUPPORT OFF)
  SET(BUILD_USB_SUPPORT OFF)
  SET(BUILD_AVAHI_SUPPORT OFF)
  SET(BUILD_BACKTRACE_SUPPORT OFF)
  SET(EXTENDED_MEDIA_MODE OFF)
elseif(CMAKE_SYSTEM_NAME STREQUAL  "Android")
  add_definitions(-DOS_LINUX)
  add_definitions(-DOS_ANDROID)
  SET(BUILD_BT_SUPPORT OFF)
  SET(BUILD_USB_SUPPORT ON)
  SET(BUILD_AVAHI_SUPPORT OFF)
  SET(BUILD_BACKTRACE_SUPPORT OFF)
endif()
if(MODIFY_FUNCTION_FLAGS)
add_definitions(-DMODIFY_FUNCTION_SIGN)
endif()

if(BUILD_TARGET_LIBRARY)
add_definitions(-DBUILD_TARGET_LIB)
endif()

if (BUILD_USB_SUPPORT)
  add_definitions(-DUSB_SUPPORT)
endif()

if (BUILD_BT_SUPPORT)
  add_definitions(-DBLUETOOTH_SUPPORT)
endif()

if (BUILD_MME_SUPPORT)
  add_definitions(-DMME_SUPPORT)
  if (MME_MQ)
    add_definitions(-DMME_MQ)
  endif(MME_MQ)
endif(BUILD_MME_SUPPORT)

if (BUILD_AVAHI_SUPPORT)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL  "Android")
  add_definitions(-DAVAHI_SUPPORT)
# --- Check libavahi-common, libavahi-client availability
  find_package(Libavahi)
endif()
endif()

if (BUILD_RWLOCK_SUPPORT)
  add_definitions(-DRWLOCK_SUPPORT)
endif()

if (BUILD_BACKTRACE_SUPPORT)
  add_definitions(-DBACKTRACE_SUPPORT)
endif()

if(ENABLE_LOG)
  add_definitions(-DENABLE_LOG)
endif()

if (TIME_TESTER)
    add_definitions(-DTIME_TESTER)
endif()

# TODO(AK): check current OS here
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX")
add_definitions(-DOS_POSIX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
add_definitions(-DOS_POSIX)
elseif(CMAKE_SYSTEM_NAME STREQUAL  "Android")
add_definitions(-DOS_POSIX)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#option(CMAKE_BUILD_TYPE "build type" "Debug")
#set(CMAKE_BUILD_TYPE "Release")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG" )
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRELEASE" )
    add_definitions(-DRELEASE)
else()
    set(CMAKE_BUILD_TYPE "Debug")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG" )
    add_definitions(-DDEBUG)
endif()
else()
# FIXME(DC): weird logic
IF(CMAKE_C_FLAGS_DEBUG)
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG" )
    add_definitions(-DDEBUG)
ELSE (CMAKE_C_FLAGS_DEBUG)
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRELEASE" )
    add_definitions(-DRELEASE)
ENDIF(CMAKE_C_FLAGS_DEBUG)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
if (EXTENDED_MEDIA_MODE)
add_definitions(-DEXTENDED_MEDIA_MODE)
# required to find 'glibconfig.h'
find_package(PkgConfig)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
add_definitions(${GLIB2_CFLAGS})
endif()
endif()

# --- Interface generator

find_package(PythonInterp)
if(NOT PYTHONINTERP_FOUND)
    message(ERROR "Python interpreter is not found")
    message(ERROR "To install it type in the command line:")
    message(ERROR "sudo apt-get install python")
endif(NOT PYTHONINTERP_FOUND)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL  "Android")
if (HMI STREQUAL "qt")
    cmake_minimum_required(VERSION 2.8.11)
    if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
        set(qt_version "4.8.5")
    else ()
        set(qt_version "5.1.0")
    endif ()

    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/FindQt.sh -v ${qt_version}
            OUTPUT_VARIABLE qt_bin_dir
    )
    message(STATUS "Binary directory Qt ${qt_version} is ${qt_bin_dir}")
    set(ENV{PATH} ${qt_bin_dir}:$ENV{PATH})

    if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
        find_package(Qt4 ${qt_version} REQUIRED QtCore QtGui QtDBus QtDeclarative)
    else ()
        find_package(Qt5Core REQUIRED)
        find_package(Qt5DBus REQUIRED)
        find_package(Qt5Qml REQUIRED)
        find_package(Qt5Quick REQUIRED)
        set(qmlplugindump_binary ${qt_bin_dir}/qmlplugindump)
    endif ()
endif()
endif()

set(INTEFRACE_GENERATOR "${PROJECT_SOURCE_DIR}/tools/InterfaceGenerator/Generator.py")
set(INTEFRACE_GENERATOR_CMD ${PYTHON_EXECUTABLE} -B ${INTEFRACE_GENERATOR})
file(GLOB_RECURSE INTERFACE_GENERATOR_DEPENDENCIES "${PROJECT_SOURCE_DIR}/tools/InterfaceGenerator/*.*")

macro (GenerateInterface arg_xml_name arg_namespace parser_type)
    string(REGEX MATCH "^[a-zA-Z_0-9]*[^.]" file_name ${arg_xml_name})     # TODO: make expression more robust

    set(hpp_file
      "${CMAKE_CURRENT_BINARY_DIR}/${file_name}.h"
      "${CMAKE_CURRENT_BINARY_DIR}/${file_name}_schema.h"
    )

    set(cpp_file "${CMAKE_CURRENT_BINARY_DIR}/${file_name}_schema.cc")
    set(full_xml_name "${CMAKE_CURRENT_SOURCE_DIR}/${arg_xml_name}")

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_custom_command( OUTPUT ${hpp_file} ${cpp_file}
                        COMMAND ${INTEFRACE_GENERATOR_CMD} ${full_xml_name} ${arg_namespace} ${CMAKE_CURRENT_BINARY_DIR} "--parser-type" "${parser_type}"
                        DEPENDS ${INTERFACE_GENERATOR_DEPENDENCIES} ${full_xml_name}
                        COMMENT "Generating files:\n   ${hpp_file}\n   ${cpp_file}\nfrom:\n   ${arg_xml_name} ..."
                        VERBATIM
                       )
else()
    add_custom_command( OUTPUT ${hpp_file} ${cpp_file}
                        COMMAND ${INTEFRACE_GENERATOR_CMD} ${full_xml_name} ${arg_namespace} ${CMAKE_CURRENT_BINARY_DIR} "--parser-type" "${parser_type}"
                        DEPENDS ${INTERFACE_GENERATOR_DEPENDENCIES} ${full_xml_name}
                        COMMENT ""
                        VERBATIM
                       )
endif()

    include_directories (
        ../../../src/components/smart_objects/include
        ../../../src/components/formatters/include/
        ${CMAKE_SOURCE_DIR}/src/components/formatters/include/
        ${CMAKE_BINARY_DIR}
    )

    add_library("${file_name}" ${cpp_file})
endmacro(GenerateInterface)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
# --- Useful macro
macro(create_test NAME SOURCES LIBS)
    add_executable("${NAME}" ${SOURCES})
    target_link_libraries("${NAME}" ${LIBS})
    if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
        add_test(${NAME} ${CMAKE_SOURCE_DIR}/qnx/remote_run_test.sh ${NAME})
    else()
        add_test(${NAME} ${NAME})
    endif()
endmacro(create_test)
endif()

# Include direction and link library

if(MODIFY_FUNCTION_FLAGS)
include_directories (
${CMAKE_SOURCE_DIR}/include/all
)
endif()
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
include_directories (
${CMAKE_SOURCE_DIR}/include/win32
)
LINK_DIRECTORIES(
${CMAKE_SOURCE_DIR}/lib/win32
)
elseif (CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
include_directories (
${CMAKE_SOURCE_DIR}/include/wince
)
LINK_DIRECTORIES(
${CMAKE_SOURCE_DIR}/lib/wince
)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
include_directories (
/usr/local/Cellar/log4cxx/0.10.0/include
)
LINK_DIRECTORIES(
/usr/local/Cellar/log4cxx/0.10.0/lib
)
endif()

# Building application

# --- Type HMI
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
if (HMI STREQUAL "qt")
    set(QT_HMI ON)
    add_definitions(-DQT_HMI)
elseif (HMI STREQUAL "web")
    set(WEB_HMI ON)
    add_definitions(-DWEB_HMI)
else ()
    set(HMI "no")
    add_definitions(-DNO_HMI)
endif ()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
    set(WEB_HMI ON)
    add_definitions(-DWEB_HMI)
endif()

if (HMI STREQUAL "qt" AND NOT DEFINED HMIADAPTER)
    set(HMIADAPTER "dbus")
endif()
if (HMI STREQUAL "web" AND NOT DEFINED HMIADAPTER)
    set(HMIADAPTER "messagebroker")
endif()
if (HMI STREQUAL "no" AND NOT DEFINED HMIADAPTER)
    set(HMIADAPTER "mqueue")
endif()

if (HMIADAPTER STREQUAL "dbus")
    set(HMI_DBUS_API ON)
    add_definitions(-DDBUS_HMIADAPTER)
    add_definitions(-DHMI_DBUS_API)
endif()
if (HMIADAPTER STREQUAL "messagebroker")
    set(HMI_JSON_API ON)
    add_definitions(-DMESSAGEBROKER_HMIADAPTER)
    add_definitions(-DHMI_JSON_API)
endif()
if (HMIADAPTER STREQUAL "mqueue")
    set(HMI_JSON_API ON)
    add_definitions(-DMQUEUE_HMIADAPTER)
    add_definitions(-DHMI_JSON_API)
endif()

# --- ThirdPartyLibs
add_subdirectory(./src/thirdPartyLibs)

# Include direction and link library for log4cxx
if(MODIFY_FUNCTION_FLAGS)
if(ENABLE_LOG)
	MESSAGE(STATUS "smartDeviceLinkCore LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_INCLUDE_DIRECTORY}")
	MESSAGE(STATUS "smartDeviceLinkCore LOG4CXX_BUILD_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_INCLUDE_DIRECTORY}")
	
	if(CMAKE_SYSTEM_NAME STREQUAL  "WindowsCE"  OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	include_directories (
		${LOG4CXX_INCLUDE_DIRECTORY}
	)
	else()
	include_directories (
		${LOG4CXX_INCLUDE_DIRECTORY}
		${LOG4CXX_BUILD_INCLUDE_DIRECTORY}
	)
	endif()
	MESSAGE(STATUS "smartDeviceLinkCore APR_LIBS_DIRECTORY ${LOG4CXX_LIBS_DIRECTORY}")
	MESSAGE(STATUS "smartDeviceLinkCore APR_UTIL_LIBS_DIRECTORY ${LOG4CXX_LIBS_DIRECTORY}")
	MESSAGE(STATUS "smartDeviceLinkCore LOG4CXX_LIBS_DIRECTORY ${LOG4CXX_LIBS_DIRECTORY}")
	LINK_DIRECTORIES(
		${APR_LIBS_DIRECTORY}
		${APR_UTIL_LIBS_DIRECTORY}
		${LOG4CXX_LIBS_DIRECTORY}
	)
endif()
endif()

# --- Tools
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
add_subdirectory(./tools)
endif()

# --- Components
add_subdirectory(./src/components)

#---LibVrMsp
if(MODIFY_FUNCTION_FLAGS)
add_subdirectory(./src/LibVrMsp)
endif()

# --- Main application
add_subdirectory(./src/appMain)

if (${BUILD_MANAGESDL_SUPPORT})
# --- Manage SDL application
add_subdirectory(./src/manageSDL)
endif()


# Building tests
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL  "Android")
if(BUILD_TESTS)
    if(BUILD_TESTS_WITH_HMI)
      add_definitions(-DTESTS_WITH_HMI)
    endif()
    enable_testing()
    include(Dart)
    # Framework GoogleTest is also integrated together gmock
    # and must not be added separately
    add_subdirectory(./src/thirdPartyLibs/gmock-1.7.0)
    add_subdirectory(./test)
endif()
endif()

# Building documentation

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL  "Android")
# At first creating directory for generated documentation. Unfortunately doxygen
# cannot generate it byself
FIND_PACKAGE(Doxygen)
  IF(DOXYGEN_FOUND)
    option(DOXYGEN_ENABLE_DIAGRAMS "Enable graphical diagram generation" ON)

    if(DOXYGEN_ENABLE_DIAGRAMS)
      set(DOXYGEN_ENABLE_DIAGRAMS_PARAM "YES")
    else(DOXYGEN_ENABLE_DIAGRAMS)
      set(DOXYGEN_ENABLE_DIAGRAMS_PARAM "NO")
    endif()
    configure_file("${PROJECT_SOURCE_DIR}/Doxyfile" "${PROJECT_BINARY_DIR}/Doxyfile")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doc/doxygen")
    ADD_CUSTOM_TARGET(doxygen COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_BINARY_DIR}/Doxyfile")
  ELSE(DOXYGEN_FOUND)
    MESSAGE(STATUS "Doxygen not found. Documentation will not be generated")
    MESSAGE(STATUS "To enable documentation generation please install doxygen and graphviz packages")
    MESSAGE(STATUS "sudo apt-get install doxygen graphviz")
    MESSAGE(STATUS "To enable processing of MscGen comments please install mscgen")
    MESSAGE(STATUS "sudo apt-get install mscgen")
ENDIF(DOXYGEN_FOUND)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
set(CMAKE_BUILD_TYPE OFF)
MESSAGE(STATUS "set CMAKE_BUILD_TYPE OFF")
endif()
