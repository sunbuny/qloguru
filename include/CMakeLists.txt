add_library(
  qloguru_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/qloguru/qloguru.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qloguru/qabstract_loguru_toolbar.hpp)
add_library(qloguru::interface ALIAS qloguru_interface)

target_include_directories(qloguru_interface
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
