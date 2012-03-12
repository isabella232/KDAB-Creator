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

#include "qnxtargetfactory.h"
#include "qnxconstants.h"
#include "qnxtarget.h"

#include <projectexplorer/customexecutablerunconfiguration.h>
#include <qtsupport/qtversionmanager.h>
#include <qt4projectmanager/qt4projectmanagerconstants.h>
#include <qt4projectmanager/buildconfigurationinfo.h>
#include <qt4projectmanager/qt4project.h>
#include <qt4projectmanager/qt4buildconfiguration.h>

#include <QIcon>

using namespace Qnx;
using namespace Qnx::Internal;

QnxTargetFactory::QnxTargetFactory(QObject *parent)
    : Qt4ProjectManager::Qt4BaseTargetFactory(parent)
{
}

QnxTargetFactory::~QnxTargetFactory()
{

}

QStringList QnxTargetFactory::supportedTargetIds() const
{
    QStringList availableIds;
    availableIds << QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
                 << QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)
                 << QLatin1String(Constants::QNX_QNX_X86_TARGET_ID)
                 << QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID);

    QStringList supportedIds;

    Q_FOREACH(const QString &availableId, availableIds) {
        if (QtSupport::QtVersionManager::instance()->supportsTargetId(availableId))
            supportedIds << availableId;
    }

    return supportedIds;
}

bool QnxTargetFactory::supportsTargetId(const QString &id) const
{
    return id == QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            || id == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)
            || id == QLatin1String(Constants::QNX_QNX_X86_TARGET_ID)
            || id == QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID);
}

QString QnxTargetFactory::displayNameForId(const QString &id) const
{
    return QnxTarget::defaultDisplayName(id);
}

QIcon QnxTargetFactory::iconForId(const QString &id) const
{
    if (id == QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            || id == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)
            || id == QLatin1String(Constants::QNX_QNX_X86_TARGET_ID)
            || id == QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID))
        return QIcon(":/qnx/images/target-small.png");

    // TODO: Separate icon for pure QNX target

    return QIcon();
}

QSet<QString> QnxTargetFactory::targetFeatures(const QString &id) const
{
    Q_UNUSED(id)

    QSet<QString> features;
    features << QLatin1String(Qt4ProjectManager::Constants::MOBILE_TARGETFEATURE_ID);
    features << QLatin1String(Qt4ProjectManager::Constants::SHADOWBUILD_TARGETFEATURE_ID);

    return features;
}

bool QnxTargetFactory::canCreate(ProjectExplorer::Project *parent, const QString &id) const
{
    Q_UNUSED(parent)

    return supportsTargetId(id);
}

bool QnxTargetFactory::canRestore(ProjectExplorer::Project *parent, const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::Target *QnxTargetFactory::create(ProjectExplorer::Project *parent,
                                                  const QString &id)
{
    if (!canCreate(parent, id))
        return 0;

    QList<QtSupport::BaseQtVersion *> knownVersions =
            QtSupport::QtVersionManager::instance()->versionsForTargetId(id);
    if (knownVersions.isEmpty())
        return 0;

    QtSupport::BaseQtVersion *qtVersion = knownVersions.first();
    QtSupport::BaseQtVersion::QmakeBuildConfigs config = qtVersion->defaultBuildConfig();

    QList<Qt4ProjectManager::BuildConfigurationInfo> infos;
    infos.append(Qt4ProjectManager::BuildConfigurationInfo(qtVersion->uniqueId(), config, QString(), QString()));
    infos.append(Qt4ProjectManager::BuildConfigurationInfo(
                     qtVersion->uniqueId(), config ^ QtSupport::BaseQtVersion::DebugBuild, QString(), QString()));

    return create(parent, id, infos);
}

ProjectExplorer::Target *QnxTargetFactory::create(
        ProjectExplorer::Project *parent,
        const QString &id,
        const QList<Qt4ProjectManager::BuildConfigurationInfo> &infos)
{
    if (!canCreate(parent, id))
        return 0;
    QnxTarget *t = new QnxTarget(static_cast<Qt4ProjectManager::Qt4Project *>(parent), id);
    foreach (const Qt4ProjectManager::BuildConfigurationInfo &info, infos) {
        t->addQt4BuildConfiguration(msgBuildConfigurationName(info), QString(),
                                    info.version(), info.buildConfig,
                                    info.additionalArguments, info.directory, info.importing);
    }

    t->setDisplayName(t->defaultDisplayName(id));
    t->createApplicationProFiles(false);
    if (t->runConfigurations().isEmpty())
        t->addRunConfiguration(new ProjectExplorer::CustomExecutableRunConfiguration(t));
    return t;
}

ProjectExplorer::Target *QnxTargetFactory::restore(ProjectExplorer::Project *parent,
                                                   const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    Qt4ProjectManager::Qt4Project *qt4project = static_cast<Qt4ProjectManager::Qt4Project *>(parent);
    QnxTarget *target = new QnxTarget(qt4project, ProjectExplorer::idFromMap(map));

    if (target->fromMap(map))
        return target;
    delete target;
    return 0;
}
