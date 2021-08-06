  cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
  project(Sheet)

  set(CMAKE_CXX_STANDARD 17)
  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(
            CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} /JMC"
    )
  else()
    set(
            CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough"
    )
  endif()

  set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.9.2-complete.jar)

  include(${CMAKE_CURRENT_SOURCE_DIR}/Formula/FindANTLR.cmake)

  add_definitions(
          -DANTLR4CPP_STATIC
          -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
  )

  set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)
  add_subdirectory(antlr4_runtime)

  antlr_target(FormulaParser ${CMAKE_CURRENT_SOURCE_DIR}/Formula/Formula.g4 LEXER PARSER LISTENER)

  include_directories(
          ${ANTLR4_INCLUDE_DIRS}
          ${ANTLR_FormulaParser_OUTPUT_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/antlr4_runtime/runtime/src
  )

  aux_source_directory(source SOURCE_LIST)

  add_executable(
          ${PROJECT_NAME}
          ${ANTLR_FormulaParser_CXX_OUTPUTS}
          ${SOURCE_LIST}
          source/tests.h)

  target_link_libraries(${PROJECT_NAME} antlr4_static)

  install(
          TARGETS ${PROJECT_NAME}
          DESTINATION bin
          EXPORT ${PROJECT_NAME}
  )

  set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})
  