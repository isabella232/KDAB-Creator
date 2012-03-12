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

#include "qnxruncontrolfactory.h"
#include "qnxconstants.h"
#include "qnxrunconfiguration.h"
#include "qnxdebugsupport.h"
#include "qnxqtversion.h"
#include "qnxruncontrol.h"
#include "qnxutils.h"

#include <debugger/debuggerengine.h>
#include <debugger/debuggerplugin.h>
#include <debugger/debuggerrunner.h>
#include <debugger/debuggerstartparameters.h>
#include <projectexplorer/target.h>
#include <projectexplorer/toolchain.h>
#include <qt4projectmanager/qt4buildconfiguration.h>
#include <remotelinux/linuxdeviceconfiguration.h>
#include <remotelinux/portlist.h>

using namespace Qnx;
using namespace Qnx::Internal;

QnxRunControlFactory::QnxRunControlFactory(QObject *parent)
    : IRunControlFactory(parent)
{
}

QnxRunControlFactory::~QnxRunControlFactory()
{
}

bool QnxRunControlFactory::canRun(ProjectExplorer::RunConfiguration *runConfiguration, ProjectExplorer::RunMode mode) const
{
    if (mode != ProjectExplorer::NormalRunMode && mode != ProjectExplorer::DebugRunMode)
        return false;

    if (!runConfiguration->isEnabled()
            || !runConfiguration->id().startsWith(QLatin1String(Constants::QNX_QNX_RUNCONFIGURATION_ID))) {
        return false;
    }

    const QnxRunConfiguration * const rc = qobject_cast<QnxRunConfiguration *>(runConfiguration);
    if (mode == ProjectExplorer::DebugRunMode)
        return rc->portsUsedByDebuggers() <= rc->freePorts().count();
    return true;
}

ProjectExplorer::RunControl *QnxRunControlFactory::create(ProjectExplorer::RunConfiguration *runConfig, ProjectExplorer::RunMode mode)
{
    Q_ASSERT(canRun(runConfig, mode));

    QnxRunConfiguration *rc = qobject_cast<QnxRunConfiguration *>(runConfig);
    Q_ASSERT(rc);
    if (mode == ProjectExplorer::NormalRunMode)
        return new QnxRunControl(rc);

    const Debugger::DebuggerStartParameters params = startParameters(rc);
    Debugger::DebuggerRunControl * const runControl = Debugger::DebuggerPlugin::createDebugger(params, rc);
    if (!runControl)
        return 0;

    QnxDebugSupport *debugSupport = new QnxDebugSupport(rc, runControl->engine());
    connect(runControl, SIGNAL(finished()), debugSupport, SLOT(handleDebuggingFinished()));

    return runControl;
}

QString QnxRunControlFactory::displayName() const
{
    return tr("Run on remote QNX device");
}

ProjectExplorer::RunConfigWidget *QnxRunControlFactory::createConfigurationWidget(ProjectExplorer::RunConfiguration *config)
{
    Q_UNUSED(config)
    return 0;
}

Debugger::DebuggerStartParameters QnxRunControlFactory::startParameters(
        const QnxRunConfiguration *runConfig)
{
    const RemoteLinux::LinuxDeviceConfiguration::ConstPtr &devConf = runConfig->deviceConfig();

    Debugger::DebuggerStartParameters params;
    params.startMode = Debugger::AttachToRemoteServer;
    params.toolChainAbi = runConfig->abi();
    params.symbolFileName = runConfig->localExecutableFilePath();
    params.remoteExecutable = runConfig->remoteExecutableFilePath();
    params.debuggerCommand =
            runConfig->activeQt4BuildConfiguration()->toolChain()->debuggerCommand().toString();
    params.remoteChannel = devConf->sshParameters().host + QLatin1String(":-1");
    params.displayName = runConfig->displayName();
    params.remoteArchitecture =
            runConfig->target()->activeBuildConfiguration()->toolChain()->targetAbi().toString();
    params.requestRemoteSetup = true;

    QnxQtVersion *qtVersion =
            static_cast<QnxQtVersion *>(runConfig->activeQt4BuildConfiguration()->qtVersion());
    params.searchPath = QnxUtils::searchPaths(qtVersion).join(QLatin1String(":"));

    return params;
}
