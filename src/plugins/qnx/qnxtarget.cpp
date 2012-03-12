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

#include "qnxtarget.h"
#include "qnxconstants.h"
#include "blackberrydeployconfiguration.h"
#include "blackberrydeployconfigurationfactory.h"
#include "blackberryrunconfiguration.h"
#include "qnxrunconfiguration.h"

#include <projectexplorer/customexecutablerunconfiguration.h>
#include <qt4projectmanager/qt4buildconfiguration.h>
#include <qt4projectmanager/qt4nodes.h>
#include <qt4projectmanager/qt4project.h>
#include <remotelinux/remotelinuxdeployconfigurationfactory.h>

using namespace Qnx;
using namespace Qnx::Internal;

namespace {
QString osTypeFromId(const QString &id)
{
    if (id == QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            || id == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID))
        return QLatin1String(Constants::QNX_BB_OS_TYPE);
    if (id == QLatin1String(Constants::QNX_QNX_X86_TARGET_ID)
            || id == QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID))
        return QLatin1String(Constants::QNX_QNX_OS_TYPE);

    return QString();
}
}

QnxTarget::QnxTarget(Qt4ProjectManager::Qt4Project *parent, const QString &id)
    : RemoteLinux::AbstractEmbeddedLinuxTarget(parent, id, osTypeFromId(id))
    , m_buildConfigurationFactory(new Qt4ProjectManager::Qt4BuildConfigurationFactory(this))
{
    setIcon(QIcon(":/qnx/images/target.png"));

    // TODO: Separate icon for pure QNX target
}

QnxTarget::~QnxTarget()
{
}

QString QnxTarget::defaultDisplayName(const QString &id)
{
    if (id == QLatin1String(Constants::QNX_BB_X86_TARGET_ID))
        return QCoreApplication::translate("Qt4ProjectManager::Qt4Target", "BlackBerry x86", "BlackBerry x86 target display name");
    if (id == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID))
        return QCoreApplication::translate("Qt4ProjectManager::Qt4Target", "BlackBerry Arm", "BlackBerry Arm target display name");
    if (id == QLatin1String(Constants::QNX_QNX_X86_TARGET_ID))
        return QCoreApplication::translate("Qt4ProjectManager::Qt4Target", "QNX x86", "QNX x86 target display name");
    if (id == QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID))
        return QCoreApplication::translate("Qt4ProjectManager::Qt4Target", "QNX Arm", "QNX Arm target display name");
    return QString();
}

ProjectExplorer::IBuildConfigurationFactory *QnxTarget::buildConfigurationFactory() const
{
    return m_buildConfigurationFactory;
}

void QnxTarget::createApplicationProFiles(bool reparse)
{
    Q_UNUSED(reparse);

    removeUnconfiguredCustomExectutableRunConfigurations();

    // We use the list twice
    QList<Qt4ProjectManager::Qt4ProFileNode *> profiles = qt4Project()->applicationProFiles();
    QSet<QString> deployConfigPaths;
    QSet<QString> runConfigPaths;
    foreach (Qt4ProjectManager::Qt4ProFileNode *pro, profiles) {
        deployConfigPaths << pro->path();
        runConfigPaths << pro->path();
    }

    foreach (ProjectExplorer::RunConfiguration *rc, runConfigurations())
        if (BlackBerryRunConfiguration *qt4rc = qobject_cast<BlackBerryRunConfiguration *>(rc))
            runConfigPaths.remove(qt4rc->proFilePath());

    if (id() == QLatin1String(Constants::QNX_BB_X86_TARGET_ID)
            || id() == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)) {
        foreach (ProjectExplorer::DeployConfiguration *dc, deployConfigurations())
            if (BlackBerryDeployConfiguration *qt4dc = qobject_cast<BlackBerryDeployConfiguration *>(dc))
                deployConfigPaths.remove(BlackBerryDeployConfigurationFactory::pathFromId(qt4dc->id()));

        // Only add new configurations if there are none.
        if (deployConfigurations().isEmpty()) {
            foreach (const QString &path, deployConfigPaths)
                addDeployConfiguration(createDeployConfiguration(QLatin1String(Constants::QNX_BB_DEPLOYCONFIGURATION_PREFIX) + path));
        }

        if (runConfigurations().isEmpty()) {
            foreach (const QString &path, runConfigPaths)
                addRunConfiguration(new BlackBerryRunConfiguration(this, path));
        }
    } else if (id() == QLatin1String(Constants::QNX_QNX_X86_TARGET_ID)
               || id() == QLatin1String(Constants::QNX_QNX_ARMLEV7_TARGET_ID)) {
        if (deployConfigurations().isEmpty())
            addDeployConfiguration(createDeployConfiguration(QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID)));

        if (runConfigurations().isEmpty()) {
            foreach (const QString &path, runConfigPaths) {
                QnxRunConfiguration *rc = new QnxRunConfiguration(this, path);
                rc->setDefaultDisplayName(tr("%1 (on QNX Device)").arg(QFileInfo(path).completeBaseName()));
                addRunConfiguration(rc);
            }
        }

    }

    // Oh still none? Add a custom executable runconfiguration
    if (runConfigurations().isEmpty()) {
        addRunConfiguration(new ProjectExplorer::CustomExecutableRunConfiguration(this));
    }
}

QList<ProjectExplorer::RunConfiguration *> QnxTarget::runConfigurationsForNode(ProjectExplorer::Node *n)
{
    QList<ProjectExplorer::RunConfiguration *> result;
    foreach (ProjectExplorer::RunConfiguration *rc, runConfigurations()) {
        if (id() == QLatin1String(Constants::QNX_BB_X86_TARGET_ID) ||
                id() == QLatin1String(Constants::QNX_BB_ARMLEV7_TARGET_ID)) {
            if (BlackBerryRunConfiguration *qnxrc = qobject_cast<BlackBerryRunConfiguration *>(rc))
                if (qnxrc->proFilePath() == n->path())
                    result << rc;
        }
    }
    return result;
}
