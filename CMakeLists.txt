CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(boot-animation C)

IF("$ENV{CFLAGS}" MATCHES "-DMICRO_ANI")
	OPTION(USE_MICRO_ANI "Use Micro Ani" ON)
ENDIF()

SET(SRCS
	src/boot-animation.c
	src/animation.c
)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	elementary
	mm-bootsound
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

IF("${TIZEN_PROFILE_NAME}" STREQUAL "mobile")
ADD_CUSTOM_COMMAND(OUTPUT poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/poweroff_480x800.edc ${CMAKE_BINARY_DIR}/poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/poweroff_480x800.edc
			${CMAKE_SOURCE_DIR}/data/poweroff_480x800_image.h
			${CMAKE_SOURCE_DIR}/data/poweroff_480x800_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT poweron.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/poweron_480x800.edc ${CMAKE_BINARY_DIR}/poweron.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/poweron_480x800.edc
			${CMAKE_SOURCE_DIR}/data/poweron_480x800_image.h
			${CMAKE_SOURCE_DIR}/data/poweron_480x800_part.h
)
ELSE()
ADD_CUSTOM_COMMAND(OUTPUT poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/poweroff_480x800.edc ${CMAKE_BINARY_DIR}/poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/poweroff_480x800.edc
			${CMAKE_SOURCE_DIR}/data/poweroff_480x800_image.h
			${CMAKE_SOURCE_DIR}/data/poweroff_480x800_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT poweron.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/poweron_480x800.edc ${CMAKE_BINARY_DIR}/poweron.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/poweron_480x800.edc
			${CMAKE_SOURCE_DIR}/data/poweron_480x800_image.h
			${CMAKE_SOURCE_DIR}/data/poweron_480x800_part.h
)
ENDIF()
ADD_CUSTOM_TARGET(edj_build DEPENDS poweron.edj poweroff.edj)
ADD_DEPENDENCIES(${PROJECT_NAME} edj_build)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/poweron.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/poweroff.edj DESTINATION share/edje)
IF("${ARCH}" STREQUAL "emulator")
ADD_DEFINITIONS("-DEMULATOR_TYPE")
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sound/SDK_poweron/poweron.wav DESTINATION share/keysound)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/1X1_poweron.edj DESTINATION share/edje/emul)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/1X1_poweroff.edj DESTINATION share/edje/emul)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/3X4_poweron.edj DESTINATION share/edje/emul)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/3X4_poweroff.edj DESTINATION share/edje/emul)
ELSEIF("${ARCH}" STREQUAL "arm")
ADD_DEFINITIONS("-DARM_TYPE")
IF("${USE_MICRO_ANI}" STREQUAL "ON")
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sound/B2_poweron/poweron.ogg DESTINATION share/keysound)
ELSEIF("${USE_MICRO_ANI}" STREQUAL "OFF")
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sound/poweron.ogg DESTINATION share/keysound)
ENDIF()
ENDIF()