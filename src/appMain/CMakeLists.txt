IF (HMIADAPTER STREQUAL "messagebroker")
set (BROKER_LIBRARIES
  MessageBrokerClient
  MessageBrokerServer
)
ENDIF ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
cmake_policy(PUSH)
# make link_directories() treat paths relative to the source dir
# info: cmake --help-policy CMP0015
cmake_policy(SET CMP0015 NEW)
link_directories(../thirdPartyLibs/libusbx)
cmake_policy(POP)
endif()

if (EXTENDED_MEDIA_MODE)
set(default_media_inc
${GSTREAMER_gst_INCLUDE_DIR}
)
else(EXTENDED_MEDIA_MODE)
set(default_media_inc
)
endif()

if (TIME_TESTER)
    set(TIME_TESTER_LIB
        TimeTester)
endif()
set(LIBRARIES
  ApplicationManager
  HMI_API
  MOBILE_API
  v4_protocol_v1_2_no_extra
  SmartObjects
  ${TIME_TESTER_LIB}
  formatters
  ProtocolHandler
  connectionHandler
  TransportManager
  HMIMessageHandler
  MessageBroker  
  ${BROKER_LIBRARIES}
  Utils
  encryption
  jsoncpp
  ConfigProfile
  RequestWatchdog
  MediaManager
  Resumption
)

if(MODIFY_FUNCTION_FLAGS)
  list(APPEND LIBRARIES  lib_msp_vr)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(APPEND LIBRARIES pthreadVC2 ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
  list(APPEND LIBRARIES pthreadVC2 ws2)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  #list(APPEND LIBRARIES pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND LIBRARIES apr-1 aprutil-1)
  list(APPEND LIBRARIES pthread dl)
elseif(CMAKE_SYSTEM_NAME STREQUAL "QNX")
  list(APPEND LIBRARIES apr-1 aprutil-1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
  if(ENABLE_LOG)
    list(APPEND LIBRARIES dl apr-1 aprutil-1)
  else()
    list(APPEND LIBRARIES dl)
  endif()
else()
  list(APPEND LIBRARIES pthread)
endif()

if(MODIFY_FUNCTION_FLAGS)
if(ENABLE_LOG)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Windows"  OR CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
  list(APPEND LIBRARIES log4cxx)
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
  list(APPEND LIBRARIES log4cxx apr-1 aprutil-1 expat winsock)
endif()
endif()
endif()

if (BUILD_BT_SUPPORT)
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
	list(APPEND LIBRARIES bluetooth)
  endif()
endif()
if (BUILD_AVAHI_SUPPORT)
	list(APPEND LIBRARIES avahi-client avahi-common)
endif()
if (BUILD_USB_SUPPORT)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(APPEND LIBRARIES Libusb-1.0.16)
endif()
endif()

include_directories (
  ./
  ../components/protocol_handler/include/
  ../thirdPartyLibs/jsoncpp/include/
  ../components/application_manager/include
  ../components/formatters/include
  ../components/transport_manager/include
  ../components/config_profile/include
  ../components/utils/include/
  ../components/connection_handler/include/
  ../components/hmi_message_handler/include
  ../components/request_watchdog/include
  ../components/smart_objects/include/
  ../components/media_manager/include/
  ${CMAKE_SOURCE_DIR}/src/components/time_tester/include
  ${CMAKE_SOURCE_DIR}/src/components/policy/src/policy/include/
  ../components/resumption/include/
  ../thirdPartyLibs/MessageBroker/include
  ../thirdPartyLibs/encryption/include
  ${CMAKE_SOURCE_DIR}/src/components/
  ${CMAKE_BINARY_DIR}/src/components/
  ${CMAKE_SOURCE_DIR}/src/components/dbus/include/
  ${CMAKE_SOURCE_DIR}
  ${default_media_inc}
)
if(MODIFY_FUNCTION_FLAGS)
include_directories (
  ${CMAKE_SOURCE_DIR}/src/LibVrMsp/
)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
include_directories (
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/pthread-win32-2.9.1
)
endif()

set (SOURCES
  main.cc
  life_cycle.cc
)

if(BUILD_TARGET_LIBRARY)
set (HEADERS
  main.h
)
endif()

if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build. Options are: None, Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()

if(BUILD_TARGET_LIBRARY)
add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
else()
add_executable(${PROJECT} ${SOURCES})
endif()
target_link_libraries(${PROJECT} ${LIBRARIES})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}")
else()
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}")
endif()
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx.properties DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/audio.8bit.wav DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/smartDeviceLink.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/policy_table.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hmi_capabilities.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sdl_preloaded_pt.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/init_policy.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	file(COPY ${CMAKE_SOURCE_DIR}/lib/win32/log4cxx.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	file(COPY ${CMAKE_SOURCE_DIR}/lib/win32/pthreadVC2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	file(COPY ${CMAKE_SOURCE_DIR}/lib/win32/msc.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif (CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
	file(COPY ${CMAKE_SOURCE_DIR}/lib/wince/msvcr90.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
endif()

if (BUILD_TESTS)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx.properties DESTINATION ${CMAKE_BINARY_DIR}/test/)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/smartDeviceLink.ini DESTINATION ${CMAKE_BINARY_DIR}/test/)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hmi_capabilities.json DESTINATION ${CMAKE_BINARY_DIR}/test/)
endif()

if (${QT_HMI})
  set(main_qml "hmi/MainWindow.qml")
  set(plugins_dir "hmi/plugins")

  if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
    set(start_command "./start-qml.sh")
    set(command_arguments "${main_qml} ${plugins_dir}")
  else ()
    execute_process(
      COMMAND ${CMAKE_SOURCE_DIR}/FindQt.sh -v ${qt_version} -b qmlscene
      OUTPUT_VARIABLE start_command
    )
    set(command_arguments "${main_qml} -I ${plugins_dir}")
  endif ()

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/start_hmi.sh
    "#!/bin/sh\n${start_command} ${command_arguments}\n")
elseif (${WEB_HMI})
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/hmi_link
    "${CMAKE_HOME_DIRECTORY}/src/components/HMI/index.html")
endif ()

# Install rules
if(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
if(BUILD_TARGET_LIBRARY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/smartDeviceLinkCore.dll" DESTINATION bin)
else()
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/smartDeviceLinkCore.exe" DESTINATION bin)
endif()
else()
install(TARGETS ${PROJECT} DESTINATION bin)
endif()
install(
  FILES log4cxx.properties audio.8bit.wav test.txt smartDeviceLink.ini policy_table.json
    hmi_capabilities.json sdl_preloaded_pt.json
  DESTINATION bin
)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
install(
  FILES ${CMAKE_SOURCE_DIR}/lib/win32/pthreadVC2.dll ${CMAKE_SOURCE_DIR}/lib/win32/log4cxx.dll ${CMAKE_SOURCE_DIR}/lib/win32/msc.dll ${CMAKE_SOURCE_DIR}/src/appMain/msp_vr.ini
  DESTINATION bin
)
elseif (CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
install(
  FILES ${CMAKE_SOURCE_DIR}/lib/wince/msvcr90.dll DESTINATION bin
)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
install(
	CODE "
execute_process(
    COMMAND install_name_tool -change /usr/local/lib/liblog4cxx.10.dylib @executable_path/liblog4cxx.10.dylib ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT}
    COMMAND chmod 644 /usr/local/lib/liblog4cxx.10.dylib
    COMMAND cp /usr/local/lib/liblog4cxx.10.dylib ${CMAKE_INSTALL_PREFIX}/bin
)
	"
)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	
endif()
if (${WEB_HMI})
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hmi_link DESTINATION bin)
  endif ()
elseif (${QT_HMI})
  if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
    install(FILES start-qml.sh DESTINATION bin
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  endif ()
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/start_hmi.sh DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
      GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
  install(FILES init_policy.sh DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
      GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif ()
