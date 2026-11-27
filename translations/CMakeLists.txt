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
if(NOT QTBASE_ZH_CN_QM)
    message(FATAL_ERROR "qtbase_zh_CN.qm not found. Install Qt translations (qttranslations5-l10n or qt6-translations-l10n).")
endif()

set(GQRX_TRANSLATION_QRC "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc")
file(WRITE "${GQRX_TRANSLATION_QRC}"
    "<RCC><qresource prefix=\"/translations\">\n"
    "<file alias=\"gqrx_zh_CN.qm\">${GQRX_QM_FILES}</file>\n"
    "<file alias=\"qtbase_zh_CN.qm\">${QTBASE_ZH_CN_QM}</file>\n"
    "</qresource></RCC>\n")
add_custom_target(gqrx_translations DEPENDS ${GQRX_QM_FILES})

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
