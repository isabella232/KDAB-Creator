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

#include "blackberryrunconfiguration.h"
#include "qnxconstants.h"
#include "blackberrydeployconfiguration.h"
#include "blackberryrunconfigurationwidget.h"
#include "qnxtarget.h"

#include <qt4projectmanager/qt4buildconfiguration.h>
#include <qt4projectmanager/qt4nodes.h>
#include <qt4projectmanager/qt4project.h>
#include <qt4projectmanager/qt4target.h>

using namespace Qnx;
using namespace Qnx::Internal;

BlackBerryRunConfiguration::BlackBerryRunConfiguration(Qt4ProjectManager::Qt4BaseTarget *parent, const QString &path)
    : ProjectExplorer::RunConfiguration(parent, QLatin1String(Constants::QNX_BB_RUNCONFIGURATION_ID))
    , m_proFilePath(path)
{
    init();
}

BlackBerryRunConfiguration::BlackBerryRunConfiguration(Qt4ProjectManager::Qt4BaseTarget *parent,
                                         BlackBerryRunConfiguration *source)
    : ProjectExplorer::RunConfiguration(parent, source)
    , m_proFilePath(source->m_proFilePath)
{
    init();
}

void BlackBerryRunConfiguration::init()
{
    updateDisplayName();
    BlackBerryDeployConfiguration *dc = deployConfiguration();
    if (dc) {
        connect(dc, SIGNAL(targetInformationChanged()),
                this, SIGNAL(targetInformationChanged()));
    }
}

void BlackBerryRunConfiguration::updateDisplayName()
{
    if (!m_proFilePath.isEmpty())
        setDefaultDisplayName(tr("%1 on BlackBerry device").arg(QFileInfo(m_proFilePath).completeBaseName()));
    else
        setDefaultDisplayName(tr("Run on BlackBerry device"));
}

QWidget *BlackBerryRunConfiguration::createConfigurationWidget()
{
    return new BlackBerryRunConfigurationWidget(this);
}

QString BlackBerryRunConfiguration::proFilePath() const
{
    return m_proFilePath;
}

QString BlackBerryRunConfiguration::deviceName() const
{
    return deployConfiguration()->deviceName();
}

QString BlackBerryRunConfiguration::barPackage() const
{
    return deployConfiguration()->barPackage();
}

QString BlackBerryRunConfiguration::localExecutableFilePath() const
{
    Qt4ProjectManager::TargetInformation ti =
            qt4Target()->qt4Project()->rootQt4ProjectNode()->targetInformation(m_proFilePath);
    if (!ti.valid)
        return QString();

    return QDir::cleanPath(ti.buildDir + QLatin1Char('/') + ti.target);
}

bool BlackBerryRunConfiguration::fromMap(const QVariantMap &map)
{
    if (!ProjectExplorer::RunConfiguration::fromMap(map))
        return false;

    m_proFilePath = map.value(QLatin1String(Constants::QNX_PROFILEPATH_KEY)).toString();
    if (m_proFilePath.isEmpty() || !QFileInfo(m_proFilePath).exists())
        return false;

    init();
    return true;
}

QVariantMap BlackBerryRunConfiguration::toMap() const
{
    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());
    map.insert(QLatin1String(Constants::QNX_PROFILEPATH_KEY), m_proFilePath);
    return map;
}

QnxTarget *BlackBerryRunConfiguration::qt4Target() const
{
    return static_cast<QnxTarget *>(target());
}

BlackBerryDeployConfiguration *BlackBerryRunConfiguration::deployConfiguration() const
{
    QList<ProjectExplorer::DeployConfiguration *> deployConfigurations = qt4Target()->deployConfigurations();
    foreach (ProjectExplorer::DeployConfiguration *dc, deployConfigurations) {
        BlackBerryDeployConfiguration *qnxDc = qobject_cast<BlackBerryDeployConfiguration *>(dc);
        if (qnxDc->proFilePath() == proFilePath())
            return qnxDc;
    }

    return 0;
}

Qt4ProjectManager::Qt4BuildConfiguration *BlackBerryRunConfiguration::activeQt4BuildConfiguration() const
{
    return static_cast<Qt4ProjectManager::Qt4BuildConfiguration *>(activeBuildConfiguration());
}

QString BlackBerryRunConfiguration::key() const
{
    return barPackage() + QLatin1String("_") + deployConfiguration()->deviceHost();
}
