# select all files with correct c++ extensions
file(GLOB_RECURSE
  ALL_CXX_SOURCE_FILES
  *.[ch] *.[chi]pp *.[chi]xx *.cppm *.cc *.hh *.ii *.[CHI]
)

# exclude the docs **for now**
set(EXCLUDE_REGEX "${PCL_SOURCE_DIR}[\\|/]doc[\\|/].*")
if(CMAKE_VERSION VERSION_LESS "3.6.0")
  foreach(PROPOSED_FILE ${ALL_CXX_SOURCE_FILES})
    string(REGEX MATCH ${EXCLUDE_REGEX} regex_found ${PROPOSED_FILE})
    if(regex_found)
      list(REMOVE_ITEM ALL_CXX_SOURCE_FILES ${PROPOSED_FILE})
    endif()
  endforeach()
else()
  list(FILTER ALL_CXX_SOURCE_FILES EXCLUDE REGEX ${EXCLUDE_REGEX})
endif()

find_package(ClangFormat 6)
# search for version number in clang-format without version number
if(ClangFormat_FOUND)
  message(STATUS "Adding target 'format'")
  add_custom_target(
  format
  COMMAND ${ClangFormat_EXECUTABLE}
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
  )
endif()
