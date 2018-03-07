cmake_minimum_required(VERSION 3.0)

project("paparl")

cmake_policy(SET CMP0003 NEW)

if(NOT VCPKG_ROOT AND DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

if(VCPKG_ROOT)
  include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()

add_subdirectory(glider)

add_executable(paparl paparl.cpp Console.cpp GameScreen.cpp)
if(MSVC)
  target_compile_definitions(paparl PRIVATE -DWIN32_LEAN_AND_MEAN -DVC_EXTRALEAN -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
  set_target_properties(paparl PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
endif()
target_link_libraries(paparl glider-core)

add_custom_command(
  TARGET paparl POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/glider/tests/data/FSEX300.ttf $<TARGET_FILE_DIR:paparl>)

add_executable(itertest itertest.cpp )
target_link_libraries(itertest glider-core)
