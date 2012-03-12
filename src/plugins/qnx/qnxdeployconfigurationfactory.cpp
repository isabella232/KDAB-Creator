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

#include "qnxdeployconfigurationfactory.h"

#include "qnxconstants.h"
#include "qnxtarget.h"

#include <remotelinux/genericdirectuploadstep.h>
#include <remotelinux/remotelinuxdeployconfiguration.h>

using namespace Qnx;
using namespace Qnx::Internal;

QnxDeployConfigurationFactory::QnxDeployConfigurationFactory(QObject *parent)
    : ProjectExplorer::DeployConfigurationFactory(parent)
{
}

QStringList QnxDeployConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    QnxTarget *target = qobject_cast<QnxTarget *>(parent);
    if (!target)
        return QStringList();

    QStringList ids;
    if (parent->id() == QLatin1String(Constants::QNX_QNX_X86_TARGET_ID)
             || parent->id() == QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID))
        ids << QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID);

    return ids;
}

QString QnxDeployConfigurationFactory::displayNameForId(const QString &id) const
{
    if (id.startsWith(QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID)))
        return tr("Deploy to QNX Device");

    return QString();
}

bool QnxDeployConfigurationFactory::canCreate(ProjectExplorer::Target *parent, const QString &id) const
{
    QnxTarget *t = qobject_cast<QnxTarget *>(parent);
    if (!t || (t->id() != QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID)
               && t->id() != QLatin1String(Constants::QNX_QNX_X86_TARGET_ID))
            || !id.startsWith(QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID)))
        return false;

    return true;
}

ProjectExplorer::DeployConfiguration *QnxDeployConfigurationFactory::create(ProjectExplorer::Target *parent, const QString &id)
{
    if (!canCreate(parent, id))
        return 0;

    if (id.startsWith(QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID))) {
        ProjectExplorer::DeployConfiguration * const dc = new RemoteLinux::RemoteLinuxDeployConfiguration(parent, id,
            displayNameForId(id));
        dc->stepList()->insertStep(0, new RemoteLinux::GenericDirectUploadStep(dc->stepList(),
            RemoteLinux::GenericDirectUploadStep::stepId()));
        return dc;
    }

    return 0;
}

bool QnxDeployConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::DeployConfiguration *QnxDeployConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    QString id = ProjectExplorer::idFromMap(map);
    RemoteLinux::RemoteLinuxDeployConfiguration * const dc = new RemoteLinux::RemoteLinuxDeployConfiguration(parent, id, displayNameForId(id));
    if (!dc->fromMap(map)) {
        delete dc;
        return 0;
    }
    return dc;
}

bool QnxDeployConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::DeployConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::DeployConfiguration *QnxDeployConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::DeployConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    return new RemoteLinux::RemoteLinuxDeployConfiguration(parent, qobject_cast<RemoteLinux::RemoteLinuxDeployConfiguration *>(source));
}
