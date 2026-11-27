# Compile and embed both application and Qt dialog translations. Keeping them
# in the executable also works for AppImage, Windows and macOS bundles.
set(GQRX_TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/gqrx_zh_CN.ts")
if(Qt6_FOUND)
    qt_add_translation(GQRX_QM_FILES "${GQRX_TS_FILE}" OPTIONS -nounfinished)
    get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
else()
    qt5_add_translation(GQRX_QM_FILES "${GQRX_TS_FILE}" OPTIONS -nounfinished)
    get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif()

execute_process(COMMAND "${QMAKE_EXECUTABLE}" -query QT_INSTALL_TRANSLATIONS
    OUTPUT_VARIABLE QT_TRANSLATIONS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
find_file(QTBASE_ZH_CN_QM qtbase_zh_CN.qm
    HINTS "${QT_TRANSLATIONS_DIR}"
    DOC "Qt Simplified Chinese catalog for standard buttons and dialogs")
set(GQRX_TRANSLATION_DEPENDENCIES ${GQRX_QM_FILES})
if(QTBASE_ZH_CN_QM AND EXISTS "${QTBASE_ZH_CN_QM}")
    set(GQRX_QTBASE_QM_FILE "${QTBASE_ZH_CN_QM}")
    message(STATUS "Using installed Qt Chinese translations: ${QTBASE_ZH_CN_QM}")
else()
    # Some distributions ship Qt translations without Simplified Chinese.
    # Use the checked-in Qt 5.15 source with this build's lrelease, without
    # downloading anything during configuration or changing the app catalog.
    set(GQRX_QTBASE_TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/qtbase/qtbase_zh_CN.ts")
    if(Qt6_FOUND)
        qt_add_translation(GQRX_QTBASE_QM_FILES "${GQRX_QTBASE_TS_FILE}" OPTIONS -nounfinished)
    else()
        qt5_add_translation(GQRX_QTBASE_QM_FILES "${GQRX_QTBASE_TS_FILE}" OPTIONS -nounfinished)
    endif()
    set(GQRX_QTBASE_QM_FILE "${GQRX_QTBASE_QM_FILES}")
    list(APPEND GQRX_TRANSLATION_DEPENDENCIES ${GQRX_QTBASE_QM_FILES})
    message(STATUS "Qt Chinese translations not installed; compiling bundled Qt 5.15 catalog")
endif()

set(GQRX_TRANSLATION_QRC "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc")
file(WRITE "${GQRX_TRANSLATION_QRC}"
    "<RCC><qresource prefix=\"/translations\">\n"
    "<file alias=\"gqrx_zh_CN.qm\">${GQRX_QM_FILES}</file>\n"
    "<file alias=\"qtbase_zh_CN.qm\">${GQRX_QTBASE_QM_FILE}</file>\n"
    "</qresource></RCC>\n")
add_custom_target(gqrx_translations DEPENDS ${GQRX_TRANSLATION_DEPENDENCIES})

install(FILES qtbase/LICENSE.GPL3-EXCEPT qtbase/README.md qtbase/README.upstream
    DESTINATION share/doc/gqrx/translations)

# Explicit maintenance target: regular builds never rewrite the source catalog.
if(Qt6_FOUND)
    set(LUPDATE_TARGET Qt6::lupdate)
else()
    set(LUPDATE_TARGET Qt5::lupdate)
endif()
add_custom_target(update_translations
    COMMAND $<TARGET_FILE:${LUPDATE_TARGET}>
        "${PROJECT_SOURCE_DIR}/src" -no-obsolete -locations relative
        -ts "${GQRX_TS_FILE}"
    COMMENT "Updating translatable source strings")

set(GQRX_TRANSLATION_QRC "${GQRX_TRANSLATION_QRC}" PARENT_SCOPE)
