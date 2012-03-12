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

#include "blackberrydeploystepfactory.h"
#include "qnxconstants.h"
#include "blackberrydeploystep.h"

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>

using namespace Qnx;
using namespace Qnx::Internal;

BlackBerryDeployStepFactory::BlackBerryDeployStepFactory()
{
}

QStringList BlackBerryDeployStepFactory::availableCreationIds(ProjectExplorer::BuildStepList *parent) const
{
    if (parent->id() != QLatin1String(ProjectExplorer::Constants::BUILDSTEPS_DEPLOY))
        return QStringList();
    if (parent->target()->id() == QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            || parent->target()->id() == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID))
        return QStringList() << QLatin1String(Constants::QNX_DEPLOY_PACKAGE_BS_ID);
    return QStringList();
}

QString BlackBerryDeployStepFactory::displayNameForId(const QString &id) const
{
    if (id == QLatin1String(Constants::QNX_DEPLOY_PACKAGE_BS_ID))
        return tr("Deploy Package");
    return QString();
}

bool BlackBerryDeployStepFactory::canCreate(ProjectExplorer::BuildStepList *parent, const QString &id) const
{
    if (parent->id() != QLatin1String(ProjectExplorer::Constants::BUILDSTEPS_DEPLOY))
        return false;
    if (parent->target()->id() != QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            && parent->target()->id() != QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID))
        return false;
    return (id == QLatin1String(Constants::QNX_DEPLOY_PACKAGE_BS_ID));
}

ProjectExplorer::BuildStep *BlackBerryDeployStepFactory::create(ProjectExplorer::BuildStepList *parent,
                                                         const QString &id)
{
    if (!canCreate(parent, id))
        return 0;
    return new BlackBerryDeployStep(parent);
}

bool BlackBerryDeployStepFactory::canRestore(ProjectExplorer::BuildStepList *parent,
                                      const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::BuildStep *BlackBerryDeployStepFactory::restore(ProjectExplorer::BuildStepList *parent,
                                                          const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    BlackBerryDeployStep *bs = new BlackBerryDeployStep(parent);
    if (bs->fromMap(map))
        return bs;
    delete bs;
    return 0;
}

bool BlackBerryDeployStepFactory::canClone(ProjectExplorer::BuildStepList *parent,
                                    ProjectExplorer::BuildStep *product) const
{
    return canCreate(parent, product->id());
}

ProjectExplorer::BuildStep *BlackBerryDeployStepFactory::clone(ProjectExplorer::BuildStepList *parent,
                                                        ProjectExplorer::BuildStep *product)
{
    if (!canClone(parent, product))
        return 0;
    return new BlackBerryDeployStep(parent, static_cast<BlackBerryDeployStep *>(product));
}
