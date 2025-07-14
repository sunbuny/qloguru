add_executable(qloguru_sample sample.cpp)

target_link_libraries(qloguru_sample Qt5::Core Qt5::Gui Qt5::Widgets qloguru::lib)
