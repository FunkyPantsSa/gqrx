#include <QApplication>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QSettings>
#include <QTemporaryDir>
#include <QVariant>
#include <iostream>
#include <stdexcept>

#include "applications/gqrx/language_manager.h"
#include "applications/gqrx/remote_control_settings.h"
#include "qtgui/bandplan.h"
#include "qtgui/bookmarks.h"
#include "qtgui/dockfft.h"
#include "qtgui/dockinputctl.h"
#include "qtgui/dockrxopt.h"
#include "qtgui/iq_tool.h"
#include "qtgui/dxc_spots.h"
#include "qtgui/plotter.h"

namespace {
void require(bool condition, const char *message)
{
    if (!condition)
        throw std::runtime_error(message);
}

QMap<QString, QVariant> snapshot(QSettings &settings)
{
    QMap<QString, QVariant> result;
    for (const QString &key : settings.allKeys())
        result.insert(key, settings.value(key));
    return result;
}

template <typename T> T *control(QWidget &widget, const char *name)
{
    auto *result = widget.findChild<T *>(QString::fromLatin1(name));
    require(result != nullptr, name);
    return result;
}

void capture(QWidget &widget, const QString &name)
{
    const QString directory = qEnvironmentVariable("GQRX_TEST_SCREENSHOT_DIR");
    if (directory.isEmpty())
        return;
    QDir().mkpath(directory);
    widget.show();
    widget.adjustSize();
    QApplication::processEvents();
    require(widget.grab().save(directory + QLatin1Char('/') + name + QStringLiteral(".png")),
            "Could not save a UI verification image");
    widget.hide();
}
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("gqrx-language-tests"));
    QCoreApplication::setApplicationName(QStringLiteral("gqrx"));
    QTemporaryDir temporary;
    require(temporary.isValid(), "Temporary directory unavailable");
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, temporary.path());

    try
    {
        auto &language = LanguageManager::instance();
        require(language.setLanguage(QStringLiteral("en")), "Cannot select English");
        QApplication::processEvents();

        DockRxOpt receiver;
        DockFft fft;
        DockInputCtl input;
        CIqTool iq;
        RemoteControlSettings remote;
        receiver.setCurrentDemod(DockRxOpt::MODE_USB);
        receiver.setCurrentFilter(FILTER_PRESET_NARROW);
        receiver.setFilterOffset(12345);
        receiver.setSquelchLevel(-87.0);
        fft.setFftRate(25);
        require(fft.setFftRate(0) == 0 && fft.fftRate() == 0,
                "Zero frame rate must keep the spectrum frozen");
        fft.setFftRate(25);
        fft.setFftSize(16384);
        fft.setSampleRate(2400000);
        fft.setWfResolution(42);
        auto *format = control<QComboBox>(iq, "formatCombo");
        format->setCurrentIndex(0);
        auto *directory = control<QLineEdit>(iq, "recDirEdit");
        directory->setText(temporary.path() + QString::fromUtf8("/未保存的录制目录"));
        directory->setCursorPosition(3);
        const QString unsavedDirectory = directory->text();
        remote.setPort(7456);
        remote.setHosts({QStringLiteral("127.0.0.1"), QStringLiteral("192.0.2.1")});

        QSettings before(temporary.filePath(QStringLiteral("before.conf")), QSettings::IniFormat);
        receiver.saveSettings(&before);
        fft.saveSettings(&before);
        input.saveSettings(&before);
        iq.saveSettings(&before);
        const auto expected = snapshot(before);
        const QString modulation = receiver.currentDemodAsString();
        require(modulation == QLatin1String("USB"), "Unexpected stable mode token");
        int changes = 0;
        QObject::connect(&receiver, &DockRxOpt::demodSelected, [&changes](int) { ++changes; });
        QObject::connect(&receiver, &DockRxOpt::filterOffsetChanged, [&changes](qint64) { ++changes; });
        QObject::connect(&receiver, &DockRxOpt::sqlLevelChanged, [&changes](double) { ++changes; });
        QObject::connect(&fft, &DockFft::fftRateChanged, [&changes](int) { ++changes; });
        QObject::connect(&fft, &DockFft::fftSizeChanged, [&changes](int) { ++changes; });
        QObject::connect(&fft, &DockFft::wfColormapChanged, [&changes](const QString &) { ++changes; });
        QObject::connect(&input, &DockInputCtl::freqCorrChanged, [&changes](double) { ++changes; });

        BandPlan::create();
        BandPlan::Get().setConfigDir(temporary.path());
        // Preserve a customized label even when it happens to be English.
        QFile customBand(temporary.filePath(QStringLiteral("bandplan.csv")));
        require(customBand.open(QIODevice::Append), "Cannot add a custom frequency band");
        customBand.write("\n1000000, 1001000, AM, 100, #FF0000, My custom band\n");
        customBand.close();
        require(BandPlan::Get().load(), "Cannot load the default band plan");
        const int bandCount = BandPlan::Get().size();
        Bookmarks::create();
        DXCSpots::create();
        CPlotter plot;
        plot.setSampleRate(2400000);
        plot.setSpanFreq(2400000);
        plot.setFftCenterFreq(10000);
        require(plot.getFftCenterFreq() == 0, "Full bandwidth must have a valid zero pan range");
        plot.setSpanFreq(1200000);
        plot.setFftCenterFreq(1000000);
        require(plot.getFftCenterFreq() == 599999, "Zoomed spectrum pan range changed");

        for (const QString &selected : {QStringLiteral("zh_CN"), QStringLiteral("en"),
                                        QStringLiteral("zh_CN"), QStringLiteral("en"),
                                        QStringLiteral("zh_CN")})
        {
            require(language.setLanguage(selected), "Cannot load a translation catalog");
            QApplication::processEvents();
            const bool chinese = selected == QLatin1String("zh_CN");
            require(language.isChinese() == chinese, "Wrong effective language");
            require(receiver.currentDemodAsString() == modulation, "Language changed the protocol mode");
            require(receiver.currentDemod() == DockRxOpt::MODE_USB, "Language reset the demodulator");
            require(receiver.currentFilter() == FILTER_PRESET_NARROW, "Language reset the filter");
            require(receiver.currentSquelchLevel() == -87.0, "Language reset squelch");
            require(fft.fftRate() == 25 && fft.fftSize() == 16384, "Language changed FFT settings");
            require(directory->text() == unsavedDirectory, "Language destroyed an unsaved edit");
            require(remote.getPort() == 7456 && remote.getHosts().size() == 2,
                    "Language changed remote control settings");
            require(changes == 0, "Retranslation emitted a receiver control signal");
            require(language.localizedResource(QStringLiteral(":/textfiles/news.txt")) ==
                    (chinese ? QStringLiteral(":/textfiles/news_zh_CN.txt") :
                               QStringLiteral(":/textfiles/news.txt")), "Wrong help resource");
            require(receiver.windowTitle().contains(chinese ? QString::fromUtf8("接收") :
                                                             QStringLiteral("Receiver")),
                    "An existing dock did not retranslate");
            require(receiver.toggleViewAction()->text() == receiver.windowTitle() &&
                    fft.toggleViewAction()->text() == fft.windowTitle(),
                    "View menu dock names did not retranslate");
            QPushButton cancel;
            cancel.setText(QCoreApplication::translate("QPlatformTheme", "Cancel"));
            require(cancel.text() == (chinese ? QString::fromUtf8("取消") : QStringLiteral("Cancel")),
                    "Qt standard dialogs did not retranslate");

            QSettings after(temporary.filePath(QStringLiteral("after.conf")), QSettings::IniFormat);
            after.clear();
            receiver.saveSettings(&after);
            fft.saveSettings(&after);
            input.saveSettings(&after);
            iq.saveSettings(&after);
            require(snapshot(after) == expected, "Language modified persisted receiver settings");
            require(BandPlan::Get().size() == bandCount, "Language changed the band plan");
            const auto custom = BandPlan::Get().getBandsEncompassing(1000500);
            bool customPreserved = false;
            for (const auto &band : custom)
                customPreserved |= band.name == QLatin1String("My custom band");
            require(customPreserved, "Language rewrote a user band label");
            const auto amateur = BandPlan::Get().getBandsEncompassing(144500000);
            bool amateurLocalized = false;
            for (const auto &band : amateur)
                if (band.builtIn && band.modulation == QLatin1String("NFM"))
                    amateurLocalized |= chinese ? band.name.contains(QString::fromUtf8("业余")) :
                                                   band.name.contains(QStringLiteral("Ham"));
            require(amateurLocalized, "Default band plan did not retranslate");
        }

        // New windows must pick up the current language as well.
        CAgcOptions agc;
        CDemodOptions demod;
        CNbOptions blanker;
        require(!agc.windowTitle().contains(QStringLiteral("Options")), "A new window was not translated");
        capture(receiver, QStringLiteral("receiver_zh_CN"));
        capture(fft, QStringLiteral("fft_zh_CN"));
        capture(input, QStringLiteral("input_zh_CN"));
        capture(iq, QStringLiteral("iq_zh_CN"));
        capture(remote, QStringLiteral("remote_zh_CN"));
        capture(agc, QStringLiteral("agc_zh_CN"));
        demod.setCurrentPage(CDemodOptions::PAGE_FM_OPT);
        capture(demod, QStringLiteral("fm_zh_CN"));
        demod.setCurrentPage(CDemodOptions::PAGE_CW_OPT);
        capture(demod, QStringLiteral("cw_zh_CN"));
        capture(blanker, QStringLiteral("blanker_zh_CN"));

        QSettings uiSettings(QSettings::IniFormat, QSettings::UserScope,
                             QCoreApplication::organizationName(), QStringLiteral("gqrx-ui"));
        require(uiSettings.value(QStringLiteral("language")).toString() == QLatin1String("zh_CN"),
                "Language preference was not saved");
        require(!before.contains(QStringLiteral("language")), "Language leaked into a receiver profile");
        require(language.setLanguage(QStringLiteral("unsupported")), "Cannot fall back to system language");
        require(language.language() == QLatin1String("system"), "Invalid language was not normalized");
        std::cout << "Language switching, settings, stable tokens, help and Qt dialogs verified.\n";
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
