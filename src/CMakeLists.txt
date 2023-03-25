include_directories("./net")

aux_source_directory("./net" net_srcs)

add_library(${PROJECT_NAME} SHARED ${net_srcs})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${xfnet_VERSION_MAJOR}.${xfnet_VERSION_MINOR} SOVERSION ${xfnet_VERSION_MAJOR})

target_link_libraries(${PROJECT_NAME} pthread)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

