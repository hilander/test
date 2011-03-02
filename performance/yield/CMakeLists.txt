add_executable ( libmixed_yield libmixed_yield.cpp )
target_link_libraries ( libmixed_yield libmixed )
target_link_libraries ( libmixed_yield -pthread )
