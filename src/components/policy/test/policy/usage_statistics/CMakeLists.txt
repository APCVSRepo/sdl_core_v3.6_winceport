include_directories(
  include
  ${CMAKE_SOURCE_DIR}/src/components/policy/usage_statistics/include
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/gmock-1.7.0/include
)

set(LIBRARIES
  gmock_main
  UsageStatistics
)

set(SOURCES
  src/test_counter.cc
)

create_test("test_counter" "${SOURCES}" "${LIBRARIES}")
