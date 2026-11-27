/* -*- c++ -*- */
/*
 * Gqrx SDR: Software defined radio receiver powered by GNU Radio and Qt
 *           https://gqrx.dk/
 *
 * Copyright 2011-2013 Alexandru Csete OZ9AEC.
 *
 * Gqrx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gqrx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gqrx; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include "retranslation.h"
#include <QDebug>
#include <QCoreApplication>
#include <QStringList>
#include <QVariant>
#include "dockrds.h"
#include "ui_dockrds.h"

static const char * const rdsProgramTypes[] = {
    QT_TRANSLATE_NOOP("RdsProgramType", "Adult Hits"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Alarm"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Alarm Test"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Children’s Programmes"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Classic Rock"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Classical"),
    QT_TRANSLATE_NOOP("RdsProgramType", "College"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Country"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Country Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Culture"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Current Affairs"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Documentary"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Drama"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Easy Listening"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Education"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Emergency"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Emergency Test"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Finance"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Folk Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Hip Hop"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Information"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Jazz"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Jazz Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Language"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Leisure"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Light Classical"),
    QT_TRANSLATE_NOOP("RdsProgramType", "National Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "News"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Nostalgia"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Oldies"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Oldies Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Other Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Personality"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Phone-In"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Pop Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Public"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Religion"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Religious Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Religious Talk"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Rhythm & Blues"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Rock"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Rock Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Science"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Serious Classical"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Social Affairs"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Soft"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Soft Rhythm & Blues"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Soft Rock"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Spanish Music"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Spanish Talk"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Sport"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Sports"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Talk"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Top 40"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Travel"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Unassigned"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Undefined"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Varied"),
    QT_TRANSLATE_NOOP("RdsProgramType", "Weather"),
};

DockRDS::DockRDS(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockRDS)
{
    ui->setupUi(this);

    ui->scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
}

DockRDS::~DockRDS()
{
    delete ui;
}

void DockRDS::updateRDS(QString text, int type)
{
    QStringList flags;

    /* type 0 = PI
     * type 1 = PS
     * type 2 = PTY
     * type 3 = flagstring: TP, TA, MuSp, MoSt, AH, CMP, stPTY
     * type 4 = RadioText
     * type 5 = ClockTime
     * type 6 = Alternative Frequencies
     */
    switch (type)
    {
    case 0:
        if (text != ui->program_information->text()) {
            // Clear the display if PI has changed
            ClearTextFields();
        }
        emit rdsPI(text);
        ui->program_information->setText(text);
        break;
    case 1:
        emit stationChanged(text);
        ui->station_name->setText(text);
        break;
    case 2:
        m_programType = text;
        ui->program_type->setText(text);
        for (const char *type : rdsProgramTypes) {
            if (text == QString::fromUtf8(type)) {
                ui->program_type->setText(QCoreApplication::translate("RdsProgramType", type));
                break;
            }
        }
        break;
    case 3:
        m_flags = text;
        if (text.size() < 7) break;
        if (text.at(0) == QLatin1Char('1')) flags << QStringLiteral("TP");
        if (text.at(1) == QLatin1Char('1')) flags << QStringLiteral("TA");
        flags << (text.at(2) == QLatin1Char('1') ? tr("Music") : tr("Speech"));
        flags << (text.at(3) == QLatin1Char('1') ? tr("Stereo") : tr("Mono"));
        if (text.at(4) == QLatin1Char('1')) flags << QStringLiteral("AH");
        if (text.at(5) == QLatin1Char('1')) flags << QStringLiteral("CMP");
        if (text.at(6) == QLatin1Char('0')) flags << QStringLiteral("stPTY");
        ui->flags->setText(flags.join(QLatin1Char(' ')));
        break;
    case 4:
        emit radiotextChanged(text);
        ui->radiotext->setText(text);
        break;
    case 5:
        ui->clocktime->setText(text);
        break;
    case 6:
        ui->alt_freq->setText(text);
        break;
    default:
        // nothing to do
        break;
    }
}

void DockRDS::ClearTextFields()
{
    m_programType.clear();
    m_flags.clear();
    ui->program_information->setText("");
    ui->station_name->setText("");
    ui->program_type->setText("");
    ui->flags->setText("");
    ui->radiotext->setText("");
    ui->clocktime->setText("");
    ui->alt_freq->setText("");

    emit rdsPI("0000");
    emit stationChanged("");
    emit radiotextChanged("");
}

void DockRDS::showEnabled()
{
    ClearTextFields();
    if (!ui->rdsCheckbox->isChecked())
    {
        ui->rdsCheckbox->blockSignals(true);
        ui->rdsCheckbox->setChecked(true);
        ui->rdsCheckbox->blockSignals(false);
    }
}

void DockRDS::showDisabled()
{
    ClearTextFields();
}

void DockRDS::setDisabled()
{
    ui->rdsCheckbox->setDisabled(true);
    ui->rdsCheckbox->blockSignals(true);
    ui->rdsCheckbox->setChecked(false);
    ui->rdsCheckbox->blockSignals(false);
}

void DockRDS::setEnabled()
{
    ui->rdsCheckbox->setDisabled(false);
}

/** Enable/disable RDS decoder */
void DockRDS::on_rdsCheckbox_toggled(bool checked)
{
    emit rdsDecoderToggled(checked);
}

/* used by remote control */
void DockRDS::setRDSmode(bool cmd)
{
    if (!ui->rdsCheckbox->isEnabled())
        return;

    ui->rdsCheckbox->setChecked(cmd);
}

void DockRDS::changeEvent(QEvent *event)
{
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
    {
        const QString program_informationText = ui->program_information->text();
        const QString station_nameText = ui->station_name->text();
        const QString program_typeText = ui->program_type->text();
        const QString flagsText = ui->flags->text();
        const QString radiotextText = ui->radiotext->text();
        const QString clocktimeText = ui->clocktime->text();
        const QString alt_freqText = ui->alt_freq->text();
        RetranslationGuard guard(this);
        ui->retranslateUi(this);
        toggleViewAction()->setText(windowTitle());
        ui->program_information->setText(program_informationText);
        ui->station_name->setText(station_nameText);
        ui->program_type->setText(program_typeText);
        ui->flags->setText(flagsText);
        ui->radiotext->setText(radiotextText);
        ui->clocktime->setText(clocktimeText);
        ui->alt_freq->setText(alt_freqText);
        if (!m_programType.isEmpty()) updateRDS(m_programType, 2);
        if (!m_flags.isEmpty()) updateRDS(m_flags, 3);
    }
}
