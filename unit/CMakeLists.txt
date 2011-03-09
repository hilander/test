add_executable ( fiber_communication fiber_communication.cpp )

target_link_libraries ( fiber_communication pthread )
target_link_libraries ( fiber_communication libmixed )

