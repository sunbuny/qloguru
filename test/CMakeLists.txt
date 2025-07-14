find_package(Qt${QT_VERSION} REQUIRED COMPONENTS Test)

add_executable(qloguru_test_ui test_qloguru.cpp)
add_executable(qloguru::test::ui ALIAS qloguru_test_ui)

target_link_libraries(qloguru_test_ui PUBLIC Qt5::Test qloguru::lib)

add_test(NAME qloguru_test_ui COMMAND qloguru_test_ui)
