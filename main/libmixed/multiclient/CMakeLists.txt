add_executable ( multi_mixed_server server.cpp )

target_link_libraries ( multi_mixed_server pthread )
target_link_libraries ( multi_mixed_server libmixed )

add_executable ( multi_mixed_client client.cpp )

target_link_libraries ( multi_mixed_client pthread )
target_link_libraries ( multi_mixed_client libmixed )
