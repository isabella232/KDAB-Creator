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

#include "gammaraysettings.h"
#include "gammarayconfigwidget.h"

#include <coreplugin/icore.h>
#include <utils/qtcassert.h>

#include <QtCore/QSettings>
#include <QtCore/QDebug>

using namespace Analyzer;

using namespace Gammaray;

namespace  {
static const QLatin1String executableC("Analyzer.Gammaray.Executable");
static const QLatin1String injectorC("Analyzer.Gammaray.Injector");
static const QLatin1String useQtcGammarayC("Analyzer.Gammaray.UseQtcGammaray");
}

QVariantMap GammarayBaseSettings::toMap() const
{
    QVariantMap map;

    map.insert(executableC, m_gammarayExecutable);
    map.insert(injectorC, m_injector);
    map.insert(useQtcGammarayC, m_useQtcGammaray);

    return map;
}

QVariantMap GammarayBaseSettings::defaults() const
{
    QVariantMap map;

    map.insert(executableC, QLatin1String("gammaray"));
    map.insert(injectorC, Constants::DefaultInjector);
    map.insert(useQtcGammarayC, true);

    return map;
}

bool GammarayBaseSettings::fromMap(const QVariantMap &map)
{
    setIfPresent(map, executableC, &m_gammarayExecutable);

    int injector = 0;
    setIfPresent(map, injectorC, &injector);
    QTC_CHECK(injector >= 0 && injector < Constants::INJECTOR_COUNT);
    m_injector = static_cast<Constants::InjectorType>(injector);

    setIfPresent(map, useQtcGammarayC, &m_useQtcGammaray);

    emit changed();
    return true;
}

QString GammarayBaseSettings::id() const
{
    return "Analyzer.Gammaray.Settings";
}

QString GammarayBaseSettings::displayName() const
{
    return tr("Gammaray");
}

QString GammarayBaseSettings::gammarayExecutable() const
{
    return m_gammarayExecutable;
}

void GammarayBaseSettings::setGammarayExecutable(const QString &exe)
{
    m_gammarayExecutable = exe;
    emit gammarayExecutableChanged(m_gammarayExecutable);
}

Constants::InjectorType GammarayBaseSettings::injector() const
{
    return m_injector;
}

void GammarayBaseSettings::setInjector(Constants::InjectorType injector)
{
    m_injector = injector;
    emit injectorChanged(m_injector);
}

bool GammarayBaseSettings::useQtcGammaray() const
{
    return m_useQtcGammaray;
}

void GammarayBaseSettings::setUseQtcGammaray(bool use)
{
    m_useQtcGammaray = use;
    emit useQtcGammarayChanged(m_useQtcGammaray);
}

QWidget *GammarayBaseSettings::createConfigWidget(QWidget *parent)
{
    return new GammarayConfigWidget(this, parent);
}
