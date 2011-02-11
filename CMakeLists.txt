set ( SUBDIRECTORIES  
      performance
      unit
    )

foreach ( DIR ${SUBDIRECTORIES} )
  add_subdirectory ( ${DIR} )
endforeach ( DIR )
