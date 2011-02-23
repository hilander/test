add_executable ( test_pthread_creation creation_pthread.cpp )
add_executable ( communication communication.cpp )
target_link_libraries ( test_pthread_creation pthread )
target_link_libraries ( communication pthread )
target_link_libraries ( communication libmixed )
set ( SUBDIRECTORIES  
      nullfork
    )

foreach ( DIR ${SUBDIRECTORIES} )
  add_subdirectory ( ${DIR} )
endforeach ( DIR )
