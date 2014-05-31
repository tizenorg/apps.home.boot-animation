CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(boot-animation C)

SET(SRCS
	src/boot-animation.c
	src/animation.c
)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	elementary
	avsystem
	audio-session-mgr
	utilX
	vconf
        ecore-x
        evas
        ecore-evas
        edje
        x11
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

ADD_CUSTOM_COMMAND(OUTPUT poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/poweroff.edc ${CMAKE_BINARY_DIR}/poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/poweroff.edc
			${CMAKE_SOURCE_DIR}/data/poweroff_image.h
			${CMAKE_SOURCE_DIR}/data/poweroff_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT poweron.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/poweron.edc ${CMAKE_BINARY_DIR}/poweron.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/poweron.edc
			${CMAKE_SOURCE_DIR}/data/poweron_image.h
			${CMAKE_SOURCE_DIR}/data/poweron_part.h
)
ADD_CUSTOM_TARGET(edj_build DEPENDS poweron.edj poweroff.edj)
ADD_DEPENDENCIES(${PROJECT_NAME} edj_build)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/poweron.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/poweroff.edj DESTINATION share/edje)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/boot-ani DESTINATION /etc/init.d
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

