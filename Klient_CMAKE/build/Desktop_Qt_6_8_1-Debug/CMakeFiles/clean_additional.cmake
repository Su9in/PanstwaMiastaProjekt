# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Klient_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Klient_autogen.dir/ParseCache.txt"
  "Klient_autogen"
  )
endif()
