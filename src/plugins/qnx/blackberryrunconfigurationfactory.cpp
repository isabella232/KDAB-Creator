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

#include "blackberryrunconfigurationfactory.h"
#include "qnxconstants.h"
#include "blackberryrunconfiguration.h"
#include "qnxtarget.h"

#include <qt4projectmanager/qt4project.h>

using namespace Qnx;
using namespace Qnx::Internal;

namespace {
QString pathFromId(const QString &id)
{
    if (id.startsWith(QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_PREFIX)))
        return id.mid(QString::fromLatin1(Constants::QNX_BB_RUNCONFIGURATION_PREFIX).size());

    return QString();
}
}

BlackBerryRunConfigurationFactory::BlackBerryRunConfigurationFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{
}

QStringList BlackBerryRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    QnxTarget *target = qobject_cast<QnxTarget *>(parent);
    if (!target)
        return QStringList();

    if (target->id() == QLatin1String(Constants::QNX_BB_X86_TARGET_ID) || target->id() == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID))
        return target->qt4Project()->applicationProFilePathes(QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_PREFIX));

    return QStringList();
}

QString BlackBerryRunConfigurationFactory::displayNameForId(const QString &id) const
{
    const QString path = pathFromId(id);
    if (path.isEmpty())
        return QString();

    if (id.startsWith(QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_PREFIX)))
        return tr("%1 on BlackBerry Device").arg(QFileInfo(path).completeBaseName());

    return QString();
}

bool BlackBerryRunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, const QString &id) const
{
    QnxTarget *t = qobject_cast<QnxTarget *>(parent);
    if (!t || (t->id() != QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
               && t->id() != QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)))
        return false;

    return t->qt4Project()->hasApplicationProFile(pathFromId(id));
}

ProjectExplorer::RunConfiguration *BlackBerryRunConfigurationFactory::create(ProjectExplorer::Target *parent,
                                                                      const QString &id)
{
    if (!canCreate(parent, id))
        return 0;

    QnxTarget *t = static_cast<QnxTarget *>(parent);

    if (id.startsWith(QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_PREFIX)))
        return new BlackBerryRunConfiguration(t, pathFromId(id));

    return 0;
}

bool BlackBerryRunConfigurationFactory::canRestore(ProjectExplorer::Target *parent,
                                            const QVariantMap &map) const
{
    QnxTarget *t = qobject_cast<QnxTarget *>(parent);
    if (!t || (t->id() != QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
               && t->id() != QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)))
        return false;

    const QString id = ProjectExplorer::idFromMap(map);
    return id == QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_ID);
}

ProjectExplorer::RunConfiguration *BlackBerryRunConfigurationFactory::restore(
        ProjectExplorer::Target *parent,
        const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    QnxTarget *t = static_cast<QnxTarget *>(parent);
    ProjectExplorer::RunConfiguration *rc = 0;
    const QString id = ProjectExplorer::idFromMap(map);
    if (id == QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_ID)) {
        rc = new BlackBerryRunConfiguration(t, QString());
        if (rc->fromMap(map))
            return rc;
    }

    delete rc;
    return 0;
}

bool BlackBerryRunConfigurationFactory::canClone(ProjectExplorer::Target *parent,
                                          ProjectExplorer::RunConfiguration *source) const
{
    if (!qobject_cast<QnxTarget *>(parent))
        return false;
    return source->id() == QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_ID);
}

ProjectExplorer::RunConfiguration *BlackBerryRunConfigurationFactory::clone(
        ProjectExplorer::Target *parent,
        ProjectExplorer::RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;

    QnxTarget *t = static_cast<QnxTarget *>(parent);

    if (source->id() == QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_ID)) {
        BlackBerryRunConfiguration *old = static_cast<BlackBerryRunConfiguration *>(source);
        return new BlackBerryRunConfiguration(t, old);
    }

    return 0;
}
