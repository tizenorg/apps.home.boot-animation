CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(boot-animation C)

IF("$ENV{CFLAGS}" MATCHES "-DMICRO_ANI")
	OPTION(USE_MICRO_ANI "Use Micro Ani" ON)
ENDIF()

SET(SRCS
	src/boot.c
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
		capi-appfw-preference
		capi-system-info
		capi-system-system-settings
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIE")
SET(CMAKE_C_FLAGS_PROFILING "-O0 -g -pg -fPIE")
SET(CMAKE_CXX_FLAGS_PROFILING "-O0 -std=c++0x -g -pg -fPIE")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -fPIE")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -std=c++0x -g -fPIE")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -g -fPIE")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 std=c++0x -g -fPIE")
SET(CMAKE_C_FLAGS_CCOV "-O2 -g --coverage -fPIE")
SET(CMAKE_CXX_FLAGS_CCOV "-O2 std=c++0x -g --coverage -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

IF("${TIZEN_PROFILE_NAME}" STREQUAL "mobile")
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
ADD_CUSTOM_COMMAND(OUTPUT mobile_poweroff1.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/mobile_poweroff1.edc ${CMAKE_BINARY_DIR}/mobile_poweroff1.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/mobile_poweroff1.edc
			${CMAKE_SOURCE_DIR}/data/mobile_poweroff1_image.h
			${CMAKE_SOURCE_DIR}/data/mobile_poweroff1_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT mobile_poweron1.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/mobile_poweron1.edc ${CMAKE_BINARY_DIR}/mobile_poweron1.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/mobile_poweron1.edc
			${CMAKE_SOURCE_DIR}/data/mobile_poweron1_image.h
			${CMAKE_SOURCE_DIR}/data/mobile_poweron1_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT wearable_poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/wearable_poweroff.edc ${CMAKE_BINARY_DIR}/wearable_poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/wearable_poweroff.edc
			${CMAKE_SOURCE_DIR}/data/wearable_poweroff_image.h
			${CMAKE_SOURCE_DIR}/data/wearable_poweroff_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT wearable_poweron.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/wearable_poweron.edc ${CMAKE_BINARY_DIR}/wearable_poweron.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/wearable_poweron.edc
			${CMAKE_SOURCE_DIR}/data/wearable_poweron_image.h
			${CMAKE_SOURCE_DIR}/data/wearable_poweron_part.h
)
ELSE()
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
ADD_CUSTOM_COMMAND(OUTPUT mobile_poweroff1.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/mobile_poweroff1.edc ${CMAKE_BINARY_DIR}/mobile_poweroff1.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/mobile_poweroff1.edc
			${CMAKE_SOURCE_DIR}/data/mobile_poweroff1_image.h
			${CMAKE_SOURCE_DIR}/data/mobile_poweroff1_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT mobile_poweron1.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/mobile_poweron1.edc ${CMAKE_BINARY_DIR}/mobile_poweron1.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/mobile_poweron1.edc
			${CMAKE_SOURCE_DIR}/data/mobile_poweron1_image.h
			${CMAKE_SOURCE_DIR}/data/mobile_poweron1_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT wearable_poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/wearable_poweroff.edc ${CMAKE_BINARY_DIR}/wearable_poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/wearable_poweroff.edc
			${CMAKE_SOURCE_DIR}/data/wearable_poweroff_image.h
			${CMAKE_SOURCE_DIR}/data/wearable_poweroff_part.h
)
ADD_CUSTOM_COMMAND(OUTPUT wearable_poweron.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data/image
			${CMAKE_SOURCE_DIR}/data/wearable_poweron.edc ${CMAKE_BINARY_DIR}/wearable_poweron.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/wearable_poweron.edc
			${CMAKE_SOURCE_DIR}/data/wearable_poweron_image.h
			${CMAKE_SOURCE_DIR}/data/wearable_poweron_part.h
)
ENDIF()
ADD_CUSTOM_TARGET(edj_build DEPENDS poweron.edj poweroff.edj mobile_poweron1.edj mobile_poweroff1.edj wearable_poweron.edj wearable_poweroff.edj)
ADD_DEPENDENCIES(${PROJECT_NAME} edj_build)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/boot-animation.efl DESTINATION /etc/smack/accesses.d)
INSTALL(FILES ${CMAKE_BINARY_DIR}/poweron.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/poweroff.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/mobile_poweron1.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/mobile_poweroff1.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/wearable_poweron.edj DESTINATION share/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/wearable_poweroff.edj DESTINATION share/edje)
IF("${ARCH}" STREQUAL "emulator")
ADD_DEFINITIONS("-DEMULATOR_TYPE")
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sound/SDK_poweron/poweron.wav DESTINATION share/sounds)
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/1X1_poweron.edj DESTINATION share/edje/emul)
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/1X1_poweroff.edj DESTINATION share/edje/emul)
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/3X4_poweron.edj DESTINATION share/edje/emul)
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/image/emul/3X4_poweroff.edj DESTINATION share/edje/emul)
ELSEIF("${ARCH}" STREQUAL "arm")
ADD_DEFINITIONS("-DARM_TYPE")
IF("${USE_MICRO_ANI}" STREQUAL "ON")
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sound/B2_poweron/poweron.ogg DESTINATION share/sounds)
ELSEIF("${USE_MICRO_ANI}" STREQUAL "OFF")
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sound/poweron.ogg DESTINATION share/sounds)
ENDIF()
ENDIF()
