add_executable ( fiber_communication fiber_communication.cpp )

target_link_libraries ( fiber_communication pthread )
target_link_libraries ( fiber_communication libmixed )

add_executable ( poller_server poller_server.cpp )

target_link_libraries ( poller_server pthread )
target_link_libraries ( poller_server libmixed )

add_executable ( fiber_client fiber_client.cpp )

target_link_libraries ( fiber_client pthread )
target_link_libraries ( fiber_client libmixed )

add_executable ( fiber_server fiber_server.cpp )

target_link_libraries ( fiber_server pthread )
target_link_libraries ( fiber_server libmixed )

add_executable ( poller_client poller_client.cpp )

target_link_libraries ( poller_client pthread )
target_link_libraries ( poller_client libmixed )

