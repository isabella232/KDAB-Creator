/**************************************************************************
**
** This file is part of Qt Creator Instrumentation Tools
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Milian Wolff, KDAB (milian.wolff@kdab.com)
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "gammarayconfigwidget.h"
#include "gammaraysettings.h"

#include "ui_gammarayconfigwidget.h"

#include <utils/qtcassert.h>

#include <QtGui/QFileDialog>

using namespace Gammaray;

GammarayConfigWidget::GammarayConfigWidget(GammarayBaseSettings *settings,
                                           QWidget *parent)
    : QWidget(parent),
      m_settings(settings),
      m_ui(new Ui::GammarayConfigWidget)
{
    m_ui->setupUi(this);

    m_ui->gammarayExeChooser->setExpectedKind(Utils::PathChooser::ExistingCommand);
    m_ui->gammarayExeChooser->setPromptDialogTitle(tr("Gammaray Command"));

    m_ui->injectorCombo->addItem(tr("Default"), Constants::DefaultInjector);
#ifndef Q_OS_WIN
    m_ui->injectorCombo->addItem(tr("Preload"), Constants::PreloadInjector);
#else
    m_ui->injectorCombo->addItem(tr("WinDLL"), Constants::WinDLLInjector);
#endif
    m_ui->injectorCombo->addItem(tr("GDB"), Constants::GDBInjector);
    m_ui->injectorCombo->addItem(tr("Style"), Constants::StyleInjector);
    QTC_CHECK(m_ui->injectorCombo->count() == Constants::INJECTOR_COUNT);

    updateUi();
    connect(m_settings, SIGNAL(changed()),
            this, SLOT(updateUi()));

    connect(m_ui->gammarayExeChooser, SIGNAL(changed(QString)),
            m_settings, SLOT(setGammarayExecutable(QString)));
    connect(m_settings, SIGNAL(gammarayExecutableChanged(QString)),
            m_ui->gammarayExeChooser, SLOT(setPath(QString)));

    connect(m_ui->injectorCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(injectorSelected(int)));
    connect(m_settings, SIGNAL(injectorChanged(Constants::InjectorType)),
            this, SLOT(injectorChanged(Constants::InjectorType)));

    connect(m_ui->useQtcGammaray, SIGNAL(toggled(bool)),
            this, SLOT(useQtcToggled(bool)));
    connect(m_settings, SIGNAL(useQtcGammarayChanged(bool)),
            this, SLOT(useQtcChanged(bool)));
}

GammarayConfigWidget::~GammarayConfigWidget()
{
    delete m_ui;
}

void GammarayConfigWidget::updateUi()
{
    m_ui->gammarayExeChooser->setPath(m_settings->gammarayExecutable());

    injectorChanged(m_settings->injector());
    useQtcChanged(m_settings->useQtcGammaray());
}

void GammarayConfigWidget::injectorChanged(Constants::InjectorType type)
{
    for (int i = 0; i < m_ui->injectorCombo->count(); ++i) {
        if (m_ui->injectorCombo->itemData(i).toInt() == type) {
            m_ui->injectorCombo->setCurrentIndex(i);
            return;
        }
    }
    // unhandled
    QTC_CHECK(false && "unhandled injector type");
    m_ui->injectorCombo->setCurrentIndex(0);
}

void GammarayConfigWidget::injectorSelected(int index)
{
    const QVariant data = m_ui->injectorCombo->itemData(index);

    bool ok = true;
    Constants::InjectorType type = static_cast<Constants::InjectorType>(data.toInt());
    QTC_ASSERT(ok && type >= 0 && type < Constants::INJECTOR_COUNT, return);

    m_settings->setInjector(type);
}

void GammarayConfigWidget::useQtcToggled(bool use)
{
    m_settings->setUseQtcGammaray(use);
    m_ui->gammarayExeChooser->setEnabled(!use);
}

void GammarayConfigWidget::useQtcChanged(bool use)
{
    m_ui->useQtcGammaray->setChecked(use);
    m_ui->gammarayExeChooser->setEnabled(!use);
}
