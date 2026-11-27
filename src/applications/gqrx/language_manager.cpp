#include "language_manager.h"

#include <QCoreApplication>
#include <QFile>
#include <QLocale>
#include <QSettings>

namespace {
QString preferredLanguage()
{
    // A separate settings file keeps the language stable when switching devices
    // and avoids changing the format or meaning of existing receiver profiles.
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName() + QStringLiteral("-ui"));
    return settings.value(QStringLiteral("language"), QStringLiteral("system")).toString();
}
}

LanguageManager::LanguageManager(QObject *parent) : QObject(parent)
{
    if (!setLanguage(preferredLanguage()))
        setLanguage(QStringLiteral("en"));
}

LanguageManager &LanguageManager::instance()
{
    static auto *manager = new LanguageManager(QCoreApplication::instance());
    return *manager;
}

bool LanguageManager::setLanguage(const QString &language)
{
    const QString requested = (language == QLatin1String("zh_CN") ||
                               language == QLatin1String("en"))
            ? language : QStringLiteral("system");
    bool chinese = requested == QLatin1String("zh_CN");
    if (requested == QLatin1String("system"))
    {
        // Match Simplified Chinese only; Traditional Chinese falls back to
        // English instead of silently substituting the wrong writing system.
        const QLocale system = QLocale::system();
        chinese = system.language() == QLocale::Chinese &&
                system.script() == QLocale::SimplifiedChineseScript;
    }
    if (m_initialized && requested == m_language && chinese == m_chinese)
        return true;

    // Load both catalogs before touching the installed pair. A missing or
    // invalid catalog must leave the current language and preferences intact.
    std::unique_ptr<QTranslator> applicationTranslator;
    std::unique_ptr<QTranslator> qtTranslator;
    if (chinese)
    {
        qtTranslator.reset(new QTranslator);
        applicationTranslator.reset(new QTranslator);
        if (!qtTranslator->load(QStringLiteral(":/translations/qtbase_zh_CN.qm")) ||
            !applicationTranslator->load(QStringLiteral(":/translations/gqrx_zh_CN.qm")))
            return false;
    }
    if (m_applicationTranslator)
        QCoreApplication::removeTranslator(m_applicationTranslator.get());
    if (m_qtTranslator)
        QCoreApplication::removeTranslator(m_qtTranslator.get());
    m_applicationTranslator = std::move(applicationTranslator);
    m_qtTranslator = std::move(qtTranslator);
    if (m_qtTranslator)
        QCoreApplication::installTranslator(m_qtTranslator.get());
    if (m_applicationTranslator)
        QCoreApplication::installTranslator(m_applicationTranslator.get());

    m_language = requested;
    m_chinese = chinese;
    m_initialized = true;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName() + QStringLiteral("-ui"));
    settings.setValue(QStringLiteral("language"), requested);
    settings.sync();
    emit languageChanged();
    return true;
}

QString LanguageManager::localizedResource(const QString &path) const
{
    if (m_chinese && path.endsWith(QLatin1String(".txt")))
    {
        const QString translated = path.left(path.size() - 4) + QStringLiteral("_zh_CN.txt");
        if (QFile::exists(translated))
            return translated;
    }
    return path;
}
