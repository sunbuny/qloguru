set(SOURCES
    qloguru.cpp qabstract_loguru_toolbar.cpp qloguru_model.cpp
    qloguru_proxy_model.cpp qloguru_toolbar.cpp qloguru_style_dialog.cpp)
set(HEADERS qloguru_model.hpp qt_logger_sink_loguru.hpp qloguru_proxy_model.hpp
            qloguru_style_dialog.hpp)
set(RESOURCES qloguru_resources.qrc)

add_library(qloguru_lib STATIC ${HEADERS} ${SOURCES} ${RESOURCES})
add_library(qloguru::lib ALIAS qloguru_lib)

target_link_libraries(qloguru_lib PUBLIC qloguru::interface)
