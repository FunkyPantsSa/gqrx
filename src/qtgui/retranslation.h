#ifndef GQRX_RETRANSLATION_H
#define GQRX_RETRANSLATION_H

#include <QAction>
#include <QComboBox>
#include <QEvent>
#include <QLineEdit>
#include <QPointer>
#include <QWidget>
#include <functional>
#include <vector>

// Designer's retranslateUi() can set editable text and emit change signals.
// Keep language changes independent of receiver settings and unsaved edits.
class RetranslationGuard
{
public:
    explicit RetranslationGuard(QWidget *widget)
    {
        auto objects = widget->findChildren<QObject *>();
        objects.prepend(widget);
        for (QObject *object : objects)
        {
            // Internal models must still notify their views: otherwise an
            // editable combo's selected label would stay in the old language.
            if (!qobject_cast<QWidget *>(object) && !qobject_cast<QAction *>(object)) continue;
            const bool blocked = object->blockSignals(true);
            const QPointer<QObject> pointer(object);
            m_unblock.emplace_back([pointer, blocked]() {
                if (pointer) pointer->blockSignals(blocked);
            });
        }
        for (QLineEdit *edit : widget->findChildren<QLineEdit *>())
        {
            // Editable combo boxes restore their own custom text below. Their
            // selected item's display text must be allowed to change language.
            if (qobject_cast<QComboBox *>(edit->parentWidget())) continue;
            const QPointer<QLineEdit> pointer(edit);
            const QString text = edit->text();
            const int cursor = edit->cursorPosition();
            const int selection = edit->selectionStart();
            const int selectedLength = edit->selectedText().size();
            m_restore.emplace_back([pointer, text, cursor, selection, selectedLength]() {
                if (pointer && pointer->text() != text) {
                    pointer->setText(text);
                    pointer->setCursorPosition(cursor);
                    if (selection >= 0) pointer->setSelection(selection, selectedLength);
                }
            });
        }
        for (QComboBox *combo : widget->findChildren<QComboBox *>())
        {
            const QPointer<QComboBox> pointer(combo);
            const int index = combo->currentIndex();
            const QString text = combo->currentText();
            const bool customText = combo->isEditable() && (index < 0 || text != combo->itemText(index));
            const QLineEdit *edit = combo->lineEdit();
            const int cursor = edit ? edit->cursorPosition() : 0;
            const int selection = edit ? edit->selectionStart() : -1;
            const int selectedLength = edit ? edit->selectedText().size() : 0;
            m_restore.emplace_back([pointer, index, text, customText, cursor, selection, selectedLength]() {
                if (!pointer) return;
                if (pointer->currentIndex() != index) pointer->setCurrentIndex(index);
                if (customText) {
                    if (pointer->currentText() != text) pointer->setEditText(text);
                    if (QLineEdit *edit = pointer->lineEdit()) {
                        edit->setCursorPosition(cursor);
                        if (selection >= 0) edit->setSelection(selection, selectedLength);
                    }
                }
            });
        }
    }

    ~RetranslationGuard()
    {
        for (const auto &restore : m_restore) restore();
        for (const auto &unblock : m_unblock) unblock();
    }

    RetranslationGuard(const RetranslationGuard &) = delete;
    RetranslationGuard &operator=(const RetranslationGuard &) = delete;

private:
    std::vector<std::function<void()>> m_restore;
    std::vector<std::function<void()>> m_unblock;
};

#endif
