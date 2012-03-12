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

#ifndef QNX_INTERNAL_QNXDEPLOYCONFIGURATION_H
#define QNX_INTERNAL_QNXDEPLOYCONFIGURATION_H

#include <remotelinux/remotelinuxdeployconfiguration.h>

namespace ProjectExplorer {
class Target;
}

namespace Qt4ProjectManager {
class Qt4BaseTarget;
}

namespace Qnx {
namespace Internal {

class QnxDeployConfiguration : public RemoteLinux::RemoteLinuxDeployConfiguration
{
    Q_OBJECT
    friend class QnxDeployConfigurationFactory;

public:
    explicit QnxDeployConfiguration(ProjectExplorer::Target *parent, const QString &proFilePath);
    virtual ~QnxDeployConfiguration();

    ProjectExplorer::DeployConfigurationWidget *configurationWidget() const;

    QString proFilePath() const;

    QString barPackage() const;
    QString deviceHost() const;
    QString password() const;
    QString deviceName() const;

    bool fromMap(const QVariantMap &map);

public slots:
    void setBarPackage(const QString &barPackage);
    void setCurrentDevice(int index);

signals:
    void targetInformationChanged();

protected:
    QnxDeployConfiguration(ProjectExplorer::Target *parent, QnxDeployConfiguration *source);

    QVariantMap toMap() const;

private slots:
    void updateBarPackage();

private:
    Qt4ProjectManager::Qt4BaseTarget *qt4Target() const;
    void setDeviceConfig(RemoteLinux::LinuxDeviceConfiguration::Id internalId);
    void updateDisplayName();

    QString m_barPackage;
    QString m_proFilePath;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_QNXDEPLOYCONFIGURATION_H
