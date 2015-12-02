CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(share-panel C)

SET(VERSION 0.1.0)

SET(PACKAGE "org.tizen.${PROJECT_NAME}")
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(RESDIR "${PREFIX}/res")
SET(EDJEDIR "${RESDIR}/edje")
SET(LOCALEDIR "${RESDIR}/locale")

SET(SRCS
	src/index.c
	src/grid.c
	src/ui_manager.c
	src/list.c
	src/page.c
	src/scroller.c
	src/share_panel.c
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	aul
	capi-appfw-application
	capi-appfw-app-manager
	capi-system-system-settings
	capi-base-common
	capi-content-mime-type
	capi-ui-efl-util
	dlog
	elementary
	evas
	feedback
	isf
	notification
	efl-extension
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fvisibility=hidden -g -Wall -O2")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

ADD_DEFINITIONS("-DPROJECT=\"${PROJECT_NAME}\"")
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DEDJEDIR=\"${EDJEDIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE}\"")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/res/A01-1_icon_Menu.png DESTINATION res/images)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${PACKAGE}.xml DESTINATION /usr/share/packages)

# subdir
ADD_SUBDIRECTORY(res)
ADD_SUBDIRECTORY(po)
