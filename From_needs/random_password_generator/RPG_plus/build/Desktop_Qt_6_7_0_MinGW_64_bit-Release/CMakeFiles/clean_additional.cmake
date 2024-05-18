# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\RPG_plus_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\RPG_plus_autogen.dir\\ParseCache.txt"
  "RPG_plus_autogen"
  )
endif()
