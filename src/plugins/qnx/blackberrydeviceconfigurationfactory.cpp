/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion (blackberry-qt@qnx.com)
** Contact: KDAB (info@kdab.com)
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

#include "blackberrydeviceconfigurationfactory.h"

#include "qnxconstants.h"
#include "blackberrydeviceconfigurationwizard.h"
#include "blackberrydeviceconfigurationwidget.h"

#include <utils/qtcassert.h>

using namespace Qnx;
using namespace Qnx::Internal;

BlackBerryDeviceConfigurationFactory::BlackBerryDeviceConfigurationFactory(QObject *parent) :
    RemoteLinux::ILinuxDeviceConfigurationFactory(parent)
{
}

QString BlackBerryDeviceConfigurationFactory::displayName() const
{
    return tr("BlackBerry Device");
}

QString BlackBerryDeviceConfigurationFactory::displayNameForOsType(const QString &type) const
{
    QTC_ASSERT(supportsOsType(type), return QString());
    return tr("BlackBerry");
}

RemoteLinux::ILinuxDeviceConfigurationWizard *BlackBerryDeviceConfigurationFactory::createWizard(QWidget *parent) const
{
    return new BlackBerryDeviceConfigurationWizard(parent);
}

RemoteLinux::ILinuxDeviceConfigurationWidget *BlackBerryDeviceConfigurationFactory::createWidget(
        const RemoteLinux::LinuxDeviceConfiguration::Ptr &deviceConfig, QWidget *parent) const
{
    return new BlackBerryDeviceConfigurationWidget(deviceConfig, parent);
}

bool BlackBerryDeviceConfigurationFactory::supportsOsType(const QString &type) const
{
    return type == QLatin1String(Constants::QNX_BB_OS_TYPE);
}

QStringList BlackBerryDeviceConfigurationFactory::supportedDeviceActionIds() const
{
    return QStringList();
}

QString BlackBerryDeviceConfigurationFactory::displayNameForActionId(const QString &actionId) const
{
    Q_UNUSED(actionId);
    return QString();
}

QDialog *BlackBerryDeviceConfigurationFactory::createDeviceAction(
        const QString &actionId,
        const RemoteLinux::LinuxDeviceConfiguration::ConstPtr &deviceConfig,
        QWidget *parent) const
{
    Q_UNUSED(actionId);
    Q_UNUSED(deviceConfig);
    Q_UNUSED(parent);
    return 0;
}
