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

#include "qnxblackberryconnect.h"
#include "qnxrunconfiguration.h"
#include "qnxdeployconfiguration.h"

#include <qt4projectmanager/qt4buildconfiguration.h>
#include <projectexplorer/target.h>
#include <utils/ssh/sshconnection.h>

#include <QProcess>
#include <QApplication>

using namespace Qnx;
using namespace Qnx::Internal;

namespace {
const char CONNECT_CMD[] = "java";
const char CONNECT_SUCCESS_MSG[] = "Successfully connected";
}

QMap<QString, QnxBlackBerryConnect *> QnxBlackBerryConnect::m_instances = QMap<QString, QnxBlackBerryConnect *>();

QnxBlackBerryConnect *QnxBlackBerryConnect::instance(QnxRunConfiguration *runConfig)
{
    QnxDeployConfiguration *deployConfig = runConfig->deployConfiguration();
    QString deviceHost = deployConfig->deviceHost();
    if (!m_instances.contains(deviceHost))
        m_instances[deviceHost] = new QnxBlackBerryConnect(runConfig);

    return m_instances[deviceHost];
}

QnxBlackBerryConnect::QnxBlackBerryConnect(QnxRunConfiguration *runConfig, QObject *parent)
    : QObject(parent)
    , m_connectionCount(0)
{
    m_process = new QProcess(this);

    Utils::Environment env = runConfig->activeQt4BuildConfiguration()->environment();
    m_process->setEnvironment(env.toStringList());

    m_connectCmd = env.searchInPath(QLatin1String(CONNECT_CMD));
    m_qnxHost = env.value(QLatin1String("QNX_HOST"));

    QnxDeployConfiguration *deployConfig = runConfig->deployConfiguration();
    m_deviceHost = deployConfig->deviceHost();
    m_password = deployConfig->password();
    m_publicKeyFile = deployConfig->deviceConfiguration()->sshParameters().privateKeyFile
            + QLatin1String(".pub");

    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));
}

void QnxBlackBerryConnect::connectToDevice()
{
    if (m_connectionCount > 0) {
        ++m_connectionCount;
        emit connected();
        return;
    }

    // Since killing the blackberry-connect script won't kill the java process it launches, let's just call
    // the java process directly instead.
    QStringList connectArgs;
    connectArgs << QLatin1String("-Xmx512M");
    connectArgs << QLatin1String("-jar") << m_qnxHost  + QLatin1String("/usr/lib/Connect.jar");

    connectArgs << QLatin1String("-targetHost") << m_deviceHost;
    if (!m_password.isEmpty())
        connectArgs << QLatin1String("-password") << m_password;
    connectArgs << QLatin1String("-sshPublicKey") << m_publicKeyFile;

    m_process->start(m_connectCmd, connectArgs);
}

void QnxBlackBerryConnect::disconnectFromDevice()
{
    if (m_process->state() != QProcess::Running)
        return;

    --m_connectionCount;
    if (m_connectionCount == 0) {
        disconnect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
                   this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));

        m_process->terminate();
        if (!m_process->waitForFinished(5000))
            m_process->kill();
    }
}

void QnxBlackBerryConnect::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_connectionCount = 0;
    if (exitCode != 0 || exitStatus != QProcess::NormalExit)
        emit error(m_process->errorString());
}

void QnxBlackBerryConnect::readStandardOutput()
{
    m_process->setReadChannel(QProcess::StandardOutput);
    while (m_process->canReadLine()) {
        QString line = QString::fromLocal8Bit(m_process->readLine());
        emit output(line, Utils::StdOutFormat);
        if (line.contains(QLatin1String(CONNECT_SUCCESS_MSG))) {
            ++m_connectionCount;
            emit connected();
        }
    }
}

void QnxBlackBerryConnect::readStandardError()
{
    m_process->setReadChannel(QProcess::StandardError);
    QStringList errorLines;

    while (m_process->canReadLine()) {
        QString line = QString::fromLocal8Bit(m_process->readLine());
        emit output(line, Utils::StdErrFormat);
        if (line.contains(QLatin1String("Error:")))
            errorLines << line.mid(7);
    }

    // TODO: Handle error messages better
    if (!errorLines.isEmpty())
        emit error(errorLines.join(QLatin1String("\n")));
}
