set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

### translation stuff

FILE(GLOB_RECURSE FILES_TO_TRANSLATE ../*.cpp ../*.h ../*.qml)

# Add languages here!
set(TRANSLATION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/qt5sdk_it.ts
)

qt5_create_translation(TRANSLATION_MESSAGES ${FILES_TO_TRANSLATE} ${TRANSLATION_FILES})
qt5_add_translation(TRANSLATION_QM ${TRANSLATION_FILES})
add_custom_target(translations_update DEPENDS ${TRANSLATION_MESSAGES})
add_custom_target(translations DEPENDS ${TRANSLATION_QM})

add_dependencies(Core translations)

install(FILES ${TRANSLATION_QM} DESTINATION ${INSTALL_TRANSLATIONS_DIR})
