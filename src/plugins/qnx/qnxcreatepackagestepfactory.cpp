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

#include "qnxcreatepackagestepfactory.h"

#include "qnxconstants.h"
#include "qnxcreatepackagestep.h"

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>

using namespace Qnx;
using namespace Qnx::Internal;

QnxCreatePackageStepFactory::QnxCreatePackageStepFactory(QObject *parent) :
    ProjectExplorer::IBuildStepFactory(parent)
{
}

QnxCreatePackageStepFactory::~QnxCreatePackageStepFactory()
{
}

QStringList QnxCreatePackageStepFactory::availableCreationIds(
        ProjectExplorer::BuildStepList *parent) const
{
    if (parent->id() != QLatin1String(ProjectExplorer::Constants::BUILDSTEPS_DEPLOY))
        return QStringList();
    if (parent->target()->id() == QLatin1String(Constants::QNX_X86_TARGET_ID)
            || parent->target()->id() == QLatin1String(Constants::QNX_ARMLEV7_TARGET_ID))
        return QStringList() << QLatin1String(Constants::QNX_CREATE_PACKAGE_BS_ID);
    return QStringList();
}

QString QnxCreatePackageStepFactory::displayNameForId(const QString &id) const
{
    if (id == QLatin1String(Constants::QNX_CREATE_PACKAGE_BS_ID))
        return tr("Create Package");
    return QString();
}

bool QnxCreatePackageStepFactory::canCreate(ProjectExplorer::BuildStepList *parent,
                                            const QString &id) const
{
    if (parent->id() != QLatin1String(ProjectExplorer::Constants::BUILDSTEPS_DEPLOY))
        return false;
    if (parent->target()->id() != QLatin1String(Constants::QNX_X86_TARGET_ID)
            && parent->target()->id() != QLatin1String(Constants::QNX_ARMLEV7_TARGET_ID))
        return false;
    return (id == QLatin1String(Constants::QNX_CREATE_PACKAGE_BS_ID));
}

ProjectExplorer::BuildStep *QnxCreatePackageStepFactory::create(
        ProjectExplorer::BuildStepList *parent, const QString &id)
{
    if (!canCreate(parent, id))
        return 0;
    return new QnxCreatePackageStep(parent);
}

bool QnxCreatePackageStepFactory::canRestore(ProjectExplorer::BuildStepList *parent,
                                             const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::BuildStep *QnxCreatePackageStepFactory::restore(
        ProjectExplorer::BuildStepList *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    QnxCreatePackageStep *bs = new QnxCreatePackageStep(parent);
    if (bs->fromMap(map))
        return bs;
    delete bs;
    return 0;
}

bool QnxCreatePackageStepFactory::canClone(ProjectExplorer::BuildStepList *parent,
                                           ProjectExplorer::BuildStep *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::BuildStep *QnxCreatePackageStepFactory::clone(ProjectExplorer::BuildStepList *parent,
                                                               ProjectExplorer::BuildStep *source)
{
    if (!canClone(parent, source))
        return 0;
    return new QnxCreatePackageStep(parent, static_cast<QnxCreatePackageStep *>(source));
}
