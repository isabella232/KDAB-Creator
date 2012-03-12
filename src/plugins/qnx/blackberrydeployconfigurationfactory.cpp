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

#include "blackberrydeployconfigurationfactory.h"

#include "qnxconstants.h"
#include "blackberrydeployconfiguration.h"
#include "qnxtarget.h"
#include "blackberrycreatepackagestep.h"
#include "blackberrydeploystep.h"

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/target.h>
#include <qt4projectmanager/qt4project.h>
#include <remotelinux/genericdirectuploadstep.h>

using namespace Qnx;
using namespace Qnx::Internal;

BlackBerryDeployConfigurationFactory::BlackBerryDeployConfigurationFactory(QObject *parent) :
    ProjectExplorer::DeployConfigurationFactory(parent)
{
}

BlackBerryDeployConfigurationFactory::~BlackBerryDeployConfigurationFactory()
{
}

QStringList BlackBerryDeployConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    QnxTarget *target = qobject_cast<QnxTarget *>(parent);
    if (!target)
        return QStringList();

    QStringList ids;
    if (parent->id() == QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            || parent->id() == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID))
        ids << target->qt4Project()->applicationProFilePathes(QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_PREFIX));

    return ids;
}

QString BlackBerryDeployConfigurationFactory::displayNameForId(const QString &id) const
{
    if (id.startsWith(QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_ID)) && !pathFromId(id).isEmpty())
        return tr("%1 to BlackBerry Device").arg(QFileInfo(pathFromId(id)).completeBaseName());

    return QString();
}

bool BlackBerryDeployConfigurationFactory::canCreate(ProjectExplorer::Target *parent,
                                              const QString &id) const
{
    QnxTarget *t = qobject_cast<QnxTarget *>(parent);
    if (!t || (t->id() != QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)
               && t->id() != QLatin1String(Constants::QNX_BB_X86_TARGET_ID))
            || !id.startsWith(QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_ID)))
        return false;

    return true;
}

ProjectExplorer::DeployConfiguration *BlackBerryDeployConfigurationFactory::create(
        ProjectExplorer::Target *parent, const QString &id)
{
    if (!canCreate(parent, id))
        return 0;

    if (id.startsWith(QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_ID))) {
        QnxTarget *t = static_cast<QnxTarget *>(parent);
        BlackBerryDeployConfiguration *dc = new BlackBerryDeployConfiguration(t, pathFromId(id));
        dc->stepList()->insertStep(0, new BlackBerryCreatePackageStep(dc->stepList()));
        dc->stepList()->insertStep(1, new BlackBerryDeployStep(dc->stepList()));
        return dc;
    }

    return 0;
}

bool BlackBerryDeployConfigurationFactory::canRestore(ProjectExplorer::Target *parent,
                                               const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::DeployConfiguration *BlackBerryDeployConfigurationFactory::restore(
        ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    QnxTarget *t = static_cast<QnxTarget *>(parent);
    BlackBerryDeployConfiguration *dc = new BlackBerryDeployConfiguration(t, QString());
    if (dc->fromMap(map))
        return dc;

    delete dc;
    return 0;
}

bool BlackBerryDeployConfigurationFactory::canClone(ProjectExplorer::Target *parent,
                                             ProjectExplorer::DeployConfiguration *source) const
{
    if (!qobject_cast<QnxTarget *>(parent))
        return false;
    return source->id() == QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_ID);
}

ProjectExplorer::DeployConfiguration *BlackBerryDeployConfigurationFactory::clone(
        ProjectExplorer::Target *parent, ProjectExplorer::DeployConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    QnxTarget *t = static_cast<QnxTarget *>(parent);
    BlackBerryDeployConfiguration *old = static_cast<BlackBerryDeployConfiguration *>(source);
    return new BlackBerryDeployConfiguration(t, old);
}

QString BlackBerryDeployConfigurationFactory::pathFromId(const QString &id)
{
    if (id.startsWith(QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_PREFIX)))
        return id.mid(QString::fromLatin1(Constants::QNX_BB_DEPLOYCONFIGURATION_PREFIX).size());

    return QString();
}
