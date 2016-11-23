add_custom_target(DynamicEvents_HEADERS SOURCES
    Event.hpp
    EventT.hpp
    IPort.hpp
    IEventHandler.hpp
)

add_library(DynamicEvents INTERFACE)
add_dependencies(DynamicEvents ${LIBRARY_NAME}_HEADERS)
target_include_directories(DynamicEvents INTERFACE .)
