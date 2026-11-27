#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <QObject>
#include <QString>
#include <QTranslator>
#include <memory>

/** Application language is a UI preference, independent of receiver profiles. */
class LanguageManager : public QObject
{
    Q_OBJECT

public:
    explicit LanguageManager(QObject *parent = nullptr);
    static LanguageManager &instance();

    QString language() const { return m_language; }
    bool isChinese() const { return m_chinese; }
    bool setLanguage(const QString &language);
    QString localizedResource(const QString &path) const;

signals:
    void languageChanged();

private:
    QString m_language;
    bool m_chinese = false;
    bool m_initialized = false;
    std::unique_ptr<QTranslator> m_applicationTranslator;
    std::unique_ptr<QTranslator> m_qtTranslator;
};

#endif // LANGUAGE_MANAGER_H
