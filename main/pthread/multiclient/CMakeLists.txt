add_executable ( pthread_mixed_server server.cpp )

target_link_libraries ( pthread_mixed_server pthread )
target_link_libraries ( pthread_mixed_server libmixed )


