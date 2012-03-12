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

#include "blackberrycreatepackagestep.h"

#include "qnxconstants.h"
#include "blackberrycreatepackagestepconfigwidget.h"
#include "blackberrydeployconfiguration.h"
#include "qnxutils.h"
#include "blackberryqtversion.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfiguration.h>
#include <qt4projectmanager/qt4buildconfiguration.h>
#include <qt4projectmanager/qt4nodes.h>
#include <qt4projectmanager/qt4project.h>
#include <qt4projectmanager/qt4target.h>
#include <utils/qtcassert.h>

#include <QtCore/QTemporaryFile>

using namespace Qnx;
using namespace Qnx::Internal;

namespace {
const char PACKAGER_CMD[] = "blackberry-nativepackager";

const char QT_INSTALL_LIBS[]        = "QT_INSTALL_LIBS";
const char QT_INSTALL_LIBS_VAR[]    = "%QT_INSTALL_LIBS%";
const char QT_INSTALL_PLUGINS[]     = "QT_INSTALL_PLUGINS";
const char QT_INSTALL_PLUGINS_VAR[] = "%QT_INSTALL_PLUGINS%";
const char QT_INSTALL_IMPORTS[]     = "QT_INSTALL_IMPORTS";
const char QT_INSTALL_IMPORTS_VAR[] = "%QT_INSTALL_IMPORTS%";
}

BlackBerryCreatePackageStep::BlackBerryCreatePackageStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, QLatin1String(Constants::QNX_CREATE_PACKAGE_BS_ID))
    , m_appDescriptorFile(QString())
    , m_preparedAppDescriptorFile(0)
{
    setDisplayName(tr("Create Bar package"));

    BlackBerryDeployConfiguration *dc = static_cast<BlackBerryDeployConfiguration *>(deployConfiguration());
    QFileInfo fi(dc->proFilePath());
    const QString fileName = fi.absolutePath() + QLatin1String("/bar-descriptor.xml");
    if (QFileInfo(fileName).exists())
        setAppDescriptorFile(fileName);
}

BlackBerryCreatePackageStep::BlackBerryCreatePackageStep(ProjectExplorer::BuildStepList *bsl,
                                           BlackBerryCreatePackageStep *bs)
    : ProjectExplorer::AbstractProcessStep(bsl, bs)
    , m_appDescriptorFile(bs->appDescriptorFile())
{
    setDisplayName(tr("Create Bar package"));
}

bool BlackBerryCreatePackageStep::immutable() const
{
    return false;
}

bool BlackBerryCreatePackageStep::init()
{
    const QString packageCmd = qt4BuildConfiguration()->environment().searchInPath(QLatin1String(PACKAGER_CMD));
    if (packageCmd.isEmpty()) {
        raiseError(tr("Could not find packager command '%s' in the build environment")
                   .arg(QLatin1String(PACKAGER_CMD)));
        return false;
    }

    if (appDescriptorFile().isEmpty()) {
        raiseError(tr("Application descriptor file not specified, please check deployment settings"));
        return false;
    }

    if (!prepareAppDescriptorFile()) // If there is an error, prepareAppDescriptorFile() will raise it
        return false;

    QStringList args;
    args << QLatin1String("-devMode");
    if (!debugToken().isEmpty())
        args << QLatin1String("-debugToken") << QnxUtils::addQuotes(QDir::toNativeSeparators(debugToken()));
    args << QLatin1String("-package") << QnxUtils::addQuotes(QDir::toNativeSeparators(barPackage()));
    args << QnxUtils::addQuotes(QDir::toNativeSeparators(m_preparedAppDescriptorFile->fileName()));

    processParameters()->setCommand(packageCmd);
    processParameters()->setArguments(args.join(" "));
    processParameters()->setEnvironment(qt4BuildConfiguration()->environment());
    processParameters()->setWorkingDirectory(qt4BuildConfiguration()->buildDirectory());
    return ProjectExplorer::AbstractProcessStep::init();
}


ProjectExplorer::BuildStepConfigWidget *BlackBerryCreatePackageStep::createConfigWidget()
{
    return new BlackBerryCreatePackageStepConfigWidget(this);
}

QString BlackBerryCreatePackageStep::barPackage() const
{
    BlackBerryDeployConfiguration *dc = static_cast<BlackBerryDeployConfiguration *>(deployConfiguration());
    return dc->barPackage();
}

QString BlackBerryCreatePackageStep::appDescriptorFile() const
{
    return m_appDescriptorFile;
}

QString BlackBerryCreatePackageStep::debugToken() const
{
    BlackBerryDeployConfiguration *dc = static_cast<BlackBerryDeployConfiguration *>(deployConfiguration());
    if (dc->deviceConfiguration().isNull())
        return QString();

    return dc->deviceConfiguration()->attribute(QLatin1String(Constants::QNX_DEBUG_TOKEN_KEY)).toString();
}

void BlackBerryCreatePackageStep::setAppDescriptorFile(const QString &appDescriptorFile)
{
    m_appDescriptorFile = appDescriptorFile;
}

QVariantMap BlackBerryCreatePackageStep::toMap() const
{
    QVariantMap map(ProjectExplorer::BuildStep::toMap());
    map.insert(QLatin1String(Constants::QNX_APPDESCRIPTOR_KEY), appDescriptorFile());
    return map;
}

bool BlackBerryCreatePackageStep::fromMap(const QVariantMap &map)
{
    setAppDescriptorFile(map.value(QLatin1String(Constants::QNX_APPDESCRIPTOR_KEY)).toString());
    return ProjectExplorer::BuildStep::fromMap(map);
}

Qt4ProjectManager::Qt4BuildConfiguration *BlackBerryCreatePackageStep::qt4BuildConfiguration() const
{
    return static_cast<Qt4ProjectManager::Qt4BuildConfiguration *>(
                target()->activeBuildConfiguration());
}

void BlackBerryCreatePackageStep::raiseError(const QString &errorMessage)
{
    emit addOutput(errorMessage, BuildStep::ErrorMessageOutput);
    emit addTask(ProjectExplorer::Task(ProjectExplorer::Task::Error, errorMessage, Utils::FileName(), -1,
                                       Core::Id(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM)));
}

bool BlackBerryCreatePackageStep::prepareAppDescriptorFile()
{
    if (m_preparedAppDescriptorFile) {
        delete m_preparedAppDescriptorFile;
        m_preparedAppDescriptorFile = 0;
    }

    BlackBerryQtVersion *qtVersion = dynamic_cast<BlackBerryQtVersion *>(qt4BuildConfiguration()->qtVersion());
    if (!qtVersion) {
        raiseError(tr("Error preparing application descriptor file"));
        return false;
    }

    QFile file(appDescriptorFile());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        raiseError(tr("Could not open '%1' for reading").arg(appDescriptorFile()));
        return false;
    }

    QByteArray fileContent = file.readAll();

    // Replace Qt path placeholders
    if (fileContent.contains(QT_INSTALL_LIBS_VAR))
        fileContent.replace(QT_INSTALL_LIBS_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_LIBS)).toLatin1());
    if (fileContent.contains(QT_INSTALL_PLUGINS_VAR))
        fileContent.replace(QT_INSTALL_PLUGINS_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_PLUGINS)).toLatin1());
    if (fileContent.contains(QT_INSTALL_IMPORTS_VAR))
        fileContent.replace(QT_INSTALL_IMPORTS_VAR, qtVersion->versionInfo().value(QLatin1String(QT_INSTALL_IMPORTS)).toLatin1());

    // Add parameter for QML debugging (if enabled)
    if (target()->activeRunConfiguration()->debuggerAspect()->useQmlDebugger()) {
        if (!fileContent.contains("-qmljsdebugger")) {
            const QString argString = QString::fromLatin1("<arg>-qmljsdebugger=port:%1</arg>\n</qnx>")
                    .arg(target()->activeRunConfiguration()->debuggerAspect()->qmlDebugServerPort());
            fileContent.replace("</qnx>", argString.toLatin1());
        }
    }

    const QString buildDir = qt4BuildConfiguration()->buildDirectory();
    QDir dir(buildDir);
    if (!dir.exists()) {
        if (!dir.mkpath(buildDir)) {
            raiseError(tr("Could not create build directory '%1'").arg(buildDir));
            return false;
        }
    }

    m_preparedAppDescriptorFile = new QTemporaryFile(buildDir + QLatin1String("/bar-descriptor_XXXXXX.xml"));
    if (!m_preparedAppDescriptorFile->open()) {
        raiseError(tr("Could not create prepared bar descriptor file in '%1'").arg(buildDir));
        return false;
    }

    m_preparedAppDescriptorFile->write(fileContent);
    m_preparedAppDescriptorFile->close();

    return true;
}

void BlackBerryCreatePackageStep::processFinished(int exitCode, QProcess::ExitStatus status)
{
    delete m_preparedAppDescriptorFile;
    m_preparedAppDescriptorFile = 0;

    AbstractProcessStep::processFinished(exitCode, status);
}
