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

#include "qnxdeployconfiguration.h"

#include "qnxconstants.h"
#include "qnxdeployconfigurationwidget.h"

#include <qt4projectmanager/qt4nodes.h>
#include <qt4projectmanager/qt4project.h>
#include <qt4projectmanager/qt4target.h>
#include <remotelinux/linuxdeviceconfigurations.h>
#include <remotelinux/abstractembeddedlinuxtarget.h>
#include <utils/ssh/sshconnection.h>

using namespace Qnx;
using namespace Qnx::Internal;

QnxDeployConfiguration::QnxDeployConfiguration(ProjectExplorer::Target *parent, const QString &proFilePath)
    : RemoteLinux::RemoteLinuxDeployConfiguration(parent,
                                                  QLatin1String(Constants::QNX_DEPLOYCONFIGURATION_ID),
                                                  tr("Deploy to QNX device"))
    , m_barPackage(QString())
    , m_proFilePath(proFilePath)
{
    connect(qt4Target()->qt4Project(), SIGNAL(proParsingDone()), this, SLOT(updateBarPackage()));
    updateDisplayName();
    updateBarPackage();
}

QnxDeployConfiguration::QnxDeployConfiguration(ProjectExplorer::Target *parent,
                                               QnxDeployConfiguration *source)
    : RemoteLinux::RemoteLinuxDeployConfiguration(parent, source)
    , m_barPackage(QString())
    , m_proFilePath(source->m_proFilePath)
{
    connect(qt4Target()->qt4Project(), SIGNAL(proParsingDone()), this, SLOT(updateBarPackage()));
    updateDisplayName();
    updateBarPackage();
}

QnxDeployConfiguration::~QnxDeployConfiguration()
{
}

QString QnxDeployConfiguration::barPackage() const
{
    return m_barPackage;
}

QString QnxDeployConfiguration::deviceHost() const
{
    if (deviceConfiguration().isNull())
        return QString();

    return deviceConfiguration()->sshParameters().host;
}

QString QnxDeployConfiguration::password() const
{
    if (deviceConfiguration().isNull())
        return QString();

    return deviceConfiguration()->sshParameters().password;
}

QString QnxDeployConfiguration::deviceName() const
{
    if (deviceConfiguration().isNull())
        return QString();

    return deviceConfiguration()->displayName();
}

void QnxDeployConfiguration::setBarPackage(const QString &barPackage)
{
    m_barPackage = barPackage;

    emit targetInformationChanged();
}

void QnxDeployConfiguration::setCurrentDevice(int index)
{
    setDeviceConfiguration(index);

    emit targetInformationChanged();
}

QVariantMap QnxDeployConfiguration::toMap() const
{
    QVariantMap map(RemoteLinux::RemoteLinuxDeployConfiguration::toMap());
    map.insert(QLatin1String(Constants::QNX_BARPACKAGE_KEY), barPackage());
    map.insert(QLatin1String(Constants::QNX_PROFILEPATH_KEY), m_proFilePath);
    return map;
}

bool QnxDeployConfiguration::fromMap(const QVariantMap &map)
{
    if (!RemoteLinux::RemoteLinuxDeployConfiguration::fromMap(map))
        return false;
    setBarPackage(map.value(QLatin1String(Constants::QNX_BARPACKAGE_KEY)).toString());
    m_proFilePath = map.value(QLatin1String(Constants::QNX_PROFILEPATH_KEY)).toString();
    if (m_proFilePath.isEmpty() || !QFileInfo(m_proFilePath).exists())
        return false;

    updateDisplayName();
    return true;
}

void QnxDeployConfiguration::updateBarPackage()
{
    if (m_barPackage.isEmpty() && !m_proFilePath.isEmpty()) {
        QList<Qt4ProjectManager::Qt4ProFileNode *> list = qt4Target()->qt4Project()->allProFiles();
        foreach (Qt4ProjectManager::Qt4ProFileNode *node, list) {
            if (!node->parseInProgress() && node->path() == m_proFilePath) {
                Qt4ProjectManager::TargetInformation ti = node->targetInformation();
                if (!ti.buildDir.isEmpty())
                    setBarPackage(ti.buildDir + QLatin1Char('/') + ti.target + QLatin1String(".bar"));
            }
        }
    }
}

Qt4ProjectManager::Qt4BaseTarget *QnxDeployConfiguration::qt4Target() const
{
    return static_cast<Qt4ProjectManager::Qt4BaseTarget *>(target());
}

ProjectExplorer::DeployConfigurationWidget *QnxDeployConfiguration::configurationWidget() const
{
    return new QnxDeployConfigurationWidget;
}

QString QnxDeployConfiguration::proFilePath() const
{
    return m_proFilePath;
}

void QnxDeployConfiguration::updateDisplayName()
{
    if (!m_proFilePath.isEmpty())
        setDefaultDisplayName(tr("%1 to QNX device").arg(QFileInfo(m_proFilePath).completeBaseName()));
    else
        setDefaultDisplayName(tr("Deploy to QNX device"));
}
