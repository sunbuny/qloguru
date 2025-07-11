add_executable(qspdlog_sample sample.cpp)

target_link_libraries(qspdlog_sample Qt5::Core Qt5::Gui Qt5::Widgets qspdlog::lib)
