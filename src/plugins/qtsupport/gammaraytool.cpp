/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 KDAB.
**
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

#include "gammaraytool.h"

#include <coreplugin/icore.h>

#include <utils/synchronousprocess.h>
#include <utils/qtcassert.h>

#include <projectexplorer/projectexplorerconstants.h>

#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QQueue>

#include <QtGui/QDesktopServices>

#include "baseqtversion.h"

using namespace QtSupport;

static inline QStringList validBinaryFilenames()
{
    return QStringList()
            << QLatin1String("debug/gammaray.exe")
            << QLatin1String("gammaray")
            << QLatin1String("gammaray.app/Contents/MacOS/gammaray");
}

static QString sourcePath()
{
    return Core::ICore::instance()->resourcePath() + QLatin1String("/gammaray/");
}

static QStringList sourceFileNames()
{
    QStringList ret;

    QQueue<QString> paths;
    paths << sourcePath();

    QDir baseDir(sourcePath());
    QTC_ASSERT(baseDir.exists(), return ret);

    while(!paths.isEmpty()) {
        QDir dir(paths.dequeue());
        QTC_ASSERT(dir.exists(), return ret);

        foreach(const QFileInfo &entry, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
            if (entry.isDir()) {
                paths << entry.absoluteFilePath();
            } else {
                ret << baseDir.relativeFilePath(entry.absoluteFilePath());
            }
        }
    }

    return ret;
}

QString GammarayTool::toolByInstallData(const QString &qtInstallData)
{
    if (!Core::ICore::instance())
        return QString();

    const QStringList directories = installDirectories(qtInstallData);
    const QStringList binFilenames = validBinaryFilenames();

    return byInstallDataHelper(sourcePath(), sourceFileNames(), directories, binFilenames, false);
}

QStringList GammarayTool::locationsByInstallData(const QString &qtInstallData)
{
    QStringList result;
    QFileInfo fileInfo;
    const QStringList binFilenames = validBinaryFilenames();
    foreach(const QString &directory, installDirectories(qtInstallData)) {
        if (getHelperFileInfoFor(binFilenames, directory, &fileInfo))
            result << fileInfo.filePath();
    }
    return result;
}

bool GammarayTool::canBuild(const BaseQtVersion *qtVersion, QString *reason)
{
    foreach (const ProjectExplorer::Abi &abi, qtVersion->qtAbis()) {
        if (abi.os() == ProjectExplorer::Abi::WindowsOS) {
            // windows version, we only support msvc
            if (abi.osFlavor() != ProjectExplorer::Abi::WindowsMsvc2005Flavor &&
                    abi.osFlavor() != ProjectExplorer::Abi::WindowsMsvc2008Flavor &&
                    abi.osFlavor() != ProjectExplorer::Abi::WindowsMsvc2010Flavor)
            {
                *reason = QCoreApplication::translate("Analyzer::GammarayTool",
                                                      "Only available for Qt built with MSVC.");
                return false;
            }
            return true;
        }
    }

    // assume we can build it everywhere else
    return true;
}

QString GammarayTool::copy(const QString &qtInstallData,
                           QString *errorMessage)
{
    const QStringList directories = GammarayTool::installDirectories(qtInstallData);

    // Try to find a writeable directory.
    foreach(const QString &directory, directories) {
        if (copyFiles(sourcePath(), sourceFileNames(), directory, errorMessage)) {
            return directory;
        }
    }
    *errorMessage = QCoreApplication::translate("Analyzer::GammarayTool",
                                                "gammaray could not be built in any of the directories:\n- %1\n\nReason: %2")
            .arg(directories.join(QLatin1String("\n- ")), *errorMessage);
    return QString();
}

bool GammarayTool::build(BuildHelperArguments arguments, QString *log, QString *errorMessage)
{
    arguments.proFilename = QLatin1String("gammaray.pro");
    arguments.helperName = QCoreApplication::translate("Analyzer::GammarayTool",
                                                       "GammaRay");
    return buildHelper(arguments, log, errorMessage);
}

QStringList GammarayTool::installDirectories(const QString &qtInstallData)
{
    const QChar slash = QLatin1Char('/');
    const uint hash = qHash(qtInstallData);
    QStringList directories;
    directories
            << (qtInstallData + QLatin1String("/qtc-gammaray/"))
            << QDir::cleanPath((QCoreApplication::applicationDirPath() + QLatin1String("/../qtc-gammaray/") + QString::number(hash))) + slash
            << (QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/qtc-gammaray/") + QString::number(hash)) + slash;
    return directories;
}
