set( dir "${CMAKE_CURRENT_SOURCE_DIR}" )

list(
		APPEND src
		"${dir}/CImage.cpp"
		"${dir}/utils.cpp"
		"${dir}/filters.cpp"
		"${dir}/CHuffmanCanon.cpp"
		"${dir}/CCustomEncoder.cpp"
		"${dir}/CCustomDecoder.cpp"
		"${dir}/CApp.cpp"
		"${dir}/main.cpp"
	)

set( src "${src}" PARENT_SCOPE )
