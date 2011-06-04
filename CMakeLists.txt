set ( SUBDIRECTORIES  
      performance
      unit
			main/libmixed/multiclient
			main/pthread/multiclient
    )

foreach ( DIR ${SUBDIRECTORIES} )
  add_subdirectory ( ${DIR} )
endforeach ( DIR )
