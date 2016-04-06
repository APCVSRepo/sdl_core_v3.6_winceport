
set(target TransportManager)

include_directories (
  ./include
  ../utils/include/
  ../protocol_handler/include
  ../connection_handler/include
  ../config_profile/include
  ../resumption/include
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/jsoncpp/include
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/libusbx-1.0.16
)
if (BUILD_BT_SUPPORT)
  include_directories(
  )
  if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    include_directories(
      ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/bluez-libs-3.36/include
    )
  endif()
endif()


set (SOURCES
  ./src/transport_manager_impl.cc
  ./src/transport_manager_default.cc
  ./src/transport_adapter/transport_adapter_listener_impl.cc
  ./src/transport_adapter/transport_adapter_impl.cc
  ./src/tcp/tcp_transport_adapter.cc
  ./src/transport_adapter/threaded_socket_connection.cc
  ./src/tcp/tcp_client_listener.cc
  ./src/tcp/tcp_device.cc
  ./src/transport_adapter/transport_adapter_event.cc
  ./src/tcp/tcp_socket_connection.cc
  ./src/tcp/tcp_connection_factory.cc
)

if (BUILD_AVAHI_SUPPORT)
  list (APPEND SOURCES
  ./src/tcp/dnssd_service_browser.cc
  )
endif()
if (BUILD_BT_SUPPORT)
  list (APPEND SOURCES
  ./src/bluetooth/bluetooth_device_scanner.cc
  ./src/bluetooth/bluetooth_transport_adapter.cc
  ./src/bluetooth/bluetooth_connection_factory.cc
  ./src/bluetooth/bluetooth_socket_connection.cc
  ./src/bluetooth/bluetooth_device.cc
  )
endif()

if (BUILD_USB_SUPPORT)
  list (APPEND SOURCES
    ./src/usb/usb_aoa_adapter.cc
    ./src/usb/usb_connection_factory.cc
    ./src/usb/usb_device_scanner.cc
  )
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list (APPEND SOURCES
    ./src/usb/libusb/usb_handler.cc
    ./src/usb/libusb/usb_connection.cc
    ./src/usb/libusb/platform_usb_device.cc
    )
  elseif(CMAKE_SYSTEM_NAME STREQUAL "QNX")
    list(APPEND SOURCES
    ./src/usb/qnx/usb_handler.cc
    ./src/usb/qnx/usb_connection.cc
    ./src/usb/qnx/platform_usb_device.cc
    )
  endif()
endif(BUILD_USB_SUPPORT)

add_library(${target} ${SOURCES})

if (BUILD_USB_SUPPORT)
  if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
    target_link_libraries(${target} usbdi)
  endif()
endif(BUILD_USB_SUPPORT)

if (BUILD_AVAHI_SUPPORT)
  target_link_libraries(${target} avahi-common)
endif()


if(ENABLE_LOG)
if(MODIFY_FUNCTION_FLAGS)
else()
  add_dependencies(${target} liblog4cxx)
  target_link_libraries(${target} log4cxx -L${LOG4CXX_LIBS_DIRECTORY})

endif()
endif()

if (BUILD_BT_SUPPORT)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
  target_link_libraries(${target} bluetooth)
endif()
endif()
