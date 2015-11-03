find_package(Sqlite3 REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/gmock-1.7.0/include
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/gmock-1.7.0/gtest/include
  ${CMAKE_SOURCE_DIR}/src/components/policy/src/policy/qdb_wrapper/include
)

set(LIBRARIES
  gtest
  gtest_main
  gmock
  gmock_main
  dbms
)

set(DATABASE_SOURCES
  ./src/test_sql_database.cc
)

set(QUERY_SOURCES
  ./src/test_sql_query.cc
)

create_test("test_SQLDatabase" "${DATABASE_SOURCES}" "${LIBRARIES}")
create_test("test_SQLQuery" "${QUERY_SOURCES}" "${LIBRARIES}")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../qdbserver.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../test-qdb.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})