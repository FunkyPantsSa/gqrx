if(Qt6_FOUND)
    qt_add_resources(TEST_RESOURCES ${GQRX_TRANSLATION_QRC}
        "${PROJECT_SOURCE_DIR}/resources/textfiles.qrc"
        "${PROJECT_SOURCE_DIR}/resources/icons.qrc")
    qt_wrap_ui(TEST_UI_HEADERS
        ../src/qtgui/dockrxopt.ui ../src/qtgui/agc_options.ui
        ../src/qtgui/demod_options.ui ../src/qtgui/nb_options.ui
        ../src/qtgui/dockfft.ui ../src/qtgui/dockinputctl.ui
        ../src/qtgui/iq_tool.ui ../src/applications/gqrx/remote_control_settings.ui)
    set(TEST_QT_LIBRARIES Qt6::Widgets)
else()
    qt5_add_resources(TEST_RESOURCES ${GQRX_TRANSLATION_QRC}
        "${PROJECT_SOURCE_DIR}/resources/textfiles.qrc"
        "${PROJECT_SOURCE_DIR}/resources/icons.qrc")
    qt5_wrap_ui(TEST_UI_HEADERS
        ../src/qtgui/dockrxopt.ui ../src/qtgui/agc_options.ui
        ../src/qtgui/demod_options.ui ../src/qtgui/nb_options.ui
        ../src/qtgui/dockfft.ui ../src/qtgui/dockinputctl.ui
        ../src/qtgui/iq_tool.ui ../src/applications/gqrx/remote_control_settings.ui)
    set(TEST_QT_LIBRARIES Qt5::Widgets)
endif()

add_executable(language_switch_test language_switch_test.cpp
    ../src/applications/gqrx/language_manager.cpp
    ../src/applications/gqrx/remote_control_settings.cpp
    ../src/qtgui/dockrxopt.cpp ../src/qtgui/agc_options.cpp
    ../src/qtgui/demod_options.cpp ../src/qtgui/nb_options.cpp
    ../src/qtgui/freqctrl.cpp ../src/qtgui/dockfft.cpp
    ../src/qtgui/dockinputctl.cpp ../src/qtgui/iq_tool.cpp
    ../src/qtgui/qtcolorpicker.cpp ../src/qtgui/ctk/ctkRangeSlider.cpp
    ../src/qtgui/bandplan.cpp ../src/qtgui/bookmarks.cpp
    ../src/qtgui/dxc_spots.cpp ../src/qtgui/plotter.cpp
    ${TEST_UI_HEADERS} ${TEST_RESOURCES})
if(Qt6_FOUND)
    set_property(TARGET language_switch_test PROPERTY CXX_STANDARD 17)
else()
    set_property(TARGET language_switch_test PROPERTY CXX_STANDARD 14)
endif()
target_include_directories(language_switch_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(language_switch_test ${TEST_QT_LIBRARIES} Volk::volk)
add_dependencies(language_switch_test gqrx_translations)
add_test(NAME language_switch COMMAND language_switch_test)
set_tests_properties(language_switch PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
add_test(NAME language_switch_fusion COMMAND language_switch_test)
set_tests_properties(language_switch_fusion PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_STYLE_OVERRIDE=fusion")

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    if(Qt6_FOUND)
        set(TEST_LUPDATE_TARGET Qt6::lupdate)
    else()
        set(TEST_LUPDATE_TARGET Qt5::lupdate)
    endif()
    add_test(NAME translation_source_extract
        COMMAND $<TARGET_FILE:${TEST_LUPDATE_TARGET}> "${PROJECT_SOURCE_DIR}/src"
            -no-obsolete -ts "${CMAKE_CURRENT_BINARY_DIR}/source.ts")
    set_tests_properties(translation_source_extract PROPERTIES FIXTURES_SETUP translation_sources)
    add_test(NAME translation_catalog
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_translations.py"
            --reference "${CMAKE_CURRENT_BINARY_DIR}/source.ts")
    set_tests_properties(translation_catalog PROPERTIES FIXTURES_REQUIRED translation_sources)
endif()
