
include_directories (
  ${SCENARIOENGINE_DLL_INCLUDE_DIR}
  ${COMMON_MINI_INCLUDE_DIR}  
  ${OSI_INCLUDE_DIR}
)

set (TARGET esmini-dyn)

set ( SOURCES
  main.cpp
)

set ( INCLUDES
)

link_directories( ${OSI_DIR}/lib )

add_executable ( ${TARGET} ${SOURCES} ${INCLUDES} )

if (USE_OSG)
  add_definitions(-D_SCENARIO_VIEWER)
endif (USE_OSG)

target_link_libraries ( 
	${TARGET}
	esminiLib
	CommonMini
	${OSI_LIBRARIES}
	${TIME_LIB}
)

if (UNIX)
  install ( TARGETS ${TARGET} DESTINATION "${INSTALL_DIRECTORY}")
else()
  install ( TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION "${INSTALL_DIRECTORY}")
  install ( TARGETS ${TARGET} CONFIGURATIONS Debug DESTINATION "${INSTALL_DIRECTORY}")
endif (UNIX)
