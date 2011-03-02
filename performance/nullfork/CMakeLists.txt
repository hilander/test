add_executable ( libmixed_null_fork libmixed_null_fork.cpp )
target_link_libraries ( libmixed_null_fork libmixed )
target_link_libraries ( libmixed_null_fork -pthread )

add_executable ( pthread_nullfork pthread_nullfork.cpp )
target_link_libraries ( pthread_nullfork libmixed )
target_link_libraries ( pthread_nullfork -pthread )
