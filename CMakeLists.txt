set ( SUBDIRECTORIES  
      performance
      unit
			main/libmixed/multiclient
    )

foreach ( DIR ${SUBDIRECTORIES} )
  add_subdirectory ( ${DIR} )
endforeach ( DIR )
