
set(SOURCES
    Url.cpp
    Message.cpp
    Request.cpp
    Response.cpp
    Parser.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)