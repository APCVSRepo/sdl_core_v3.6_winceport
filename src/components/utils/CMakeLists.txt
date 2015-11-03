include_directories (
  ./include
  ../config_profile/include
  ../media_manager/include/
  ../protocol_handler/include/
)

if(MODIFY_FUNCTION_FLAGS)
set (SOURCES
    ./src/bitstream.cc
    ./src/conditional_variable_posix.cc
    ./src/file_system.cc
    ./src/threads/posix_thread.cc
    ./src/threads/thread_manager.cc
    ./src/threads/thread_validator.cc
    ./src/lock_posix.cc
    ./src/date_time.cc
    ./src/signals_linux.cc
    ./src/system.cc
)
else()
set (SOURCES
    ./src/bitstream.cc
    ./src/conditional_variable_posix.cc
    ./src/file_system.cc
    ./src/threads/posix_thread.cc
    ./src/threads/thread_manager.cc
    ./src/threads/thread_validator.cc
    ./src/lock_posix.cc
    ./src/date_time.cc
    ./src/signals_linux.cc
    ./src/system.cc
    ./src/resource_usage.cc
)
endif()

if (BUILD_BACKTRACE_SUPPORT)
    list(APPEND SOURCES
    ./src/back_trace.cc
    )
endif()

if(MODIFY_FUNCTION_FLAGS)
		list(APPEND SOURCES
		./src/global.cc
		)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
    list(APPEND SOURCES
    ./src/threads/pulse_thread_delegate.cc
    )
endif()

add_library("Utils" ${SOURCES})

IF(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
  target_link_libraries("Utils" apr-1 aprutil-1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_link_libraries ("Utils")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	#target_link_libraries ("Utils")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "WindowsCE")
	target_link_libraries ("Utils" log4cxx)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
else()
  target_link_libraries("Utils" apr-1 aprutil-1 rt)
endif()

if(ENABLE_LOG)
if(MODIFY_FUNCTION_FLAGS)
else()
  add_dependencies("Utils" liblog4cxx)
  target_link_libraries("Utils" log4cxx -L${LOG4CXX_LIBS_DIRECTORY})
endif()
endif()
