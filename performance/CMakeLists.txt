add_executable ( test_pthread_creation creation_pthread.cpp )
target_link_libraries ( test_pthread_creation pthread )
set ( SUBDIRECTORIES  
      nullfork
    )

foreach ( DIR ${SUBDIRECTORIES} )
  add_subdirectory ( ${DIR} )
endforeach ( DIR )
