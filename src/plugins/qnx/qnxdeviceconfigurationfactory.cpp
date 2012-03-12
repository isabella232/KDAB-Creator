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

#include "qnxdeviceconfigurationfactory.h"

#include "qnxconstants.h"
#include "qnxdeviceconfigurationwizard.h"

#include <remotelinux/genericlinuxdeviceconfigurationwidget.h>
#include <utils/qtcassert.h>

namespace Qnx {
namespace Internal {

QnxDeviceConfigurationFactory::QnxDeviceConfigurationFactory(QObject *parent) :
    RemoteLinux::ILinuxDeviceConfigurationFactory(parent)
{
}

QString QnxDeviceConfigurationFactory::displayName() const
{
    return tr("QNX Device");
}

QString QnxDeviceConfigurationFactory::displayNameForOsType(const QString &type) const
{
    QTC_ASSERT(supportsOsType(type), return QString());
    return tr("QNX");
}

RemoteLinux::ILinuxDeviceConfigurationWizard *QnxDeviceConfigurationFactory::createWizard(QWidget *parent) const
{
    return new QnxDeviceConfigurationWizard(parent);
}

RemoteLinux::ILinuxDeviceConfigurationWidget *QnxDeviceConfigurationFactory::createWidget(const RemoteLinux::LinuxDeviceConfiguration::Ptr &deviceConfig, QWidget *parent) const
{
    return new RemoteLinux::GenericLinuxDeviceConfigurationWidget(deviceConfig, parent);
}

bool QnxDeviceConfigurationFactory::supportsOsType(const QString &type) const
{
    return type == QLatin1String(Constants::QNX_QNX_OS_TYPE);
}

QStringList QnxDeviceConfigurationFactory::supportedDeviceActionIds() const
{
    return QStringList();
}

QString QnxDeviceConfigurationFactory::displayNameForActionId(const QString &actionId) const
{
    Q_UNUSED(actionId);
    return QString();
}

QDialog *QnxDeviceConfigurationFactory::createDeviceAction(const QString &actionId, const RemoteLinux::LinuxDeviceConfiguration::ConstPtr &deviceConfig, QWidget *parent) const
{
    Q_UNUSED(actionId);
    Q_UNUSED(deviceConfig);
    Q_UNUSED(parent);
    return 0;
}

} // namespace Internal
} // namespace Qnx
