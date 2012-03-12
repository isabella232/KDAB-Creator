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

#include "qnxtoolchainfactory.h"

#include "qnxconstants.h"
#include "qnxtoolchain.h"
#include "qnxabstractqtversion.h"

#include <projectexplorer/toolchainmanager.h>
#include <qtsupport/qtversionmanager.h>

#include <QDir>

using namespace Qnx;
using namespace Qnx::Internal;

QnxToolChainFactory::QnxToolChainFactory()
{
}

QString QnxToolChainFactory::displayName() const
{
    return tr("QNX GCC");
}

QString QnxToolChainFactory::id() const
{
    return QLatin1String(Constants::QNX_TOOLCHAIN_ID);
}

bool QnxToolChainFactory::canRestore(const QVariantMap &data)
{
    return idFromMap(data).startsWith(QLatin1String(Constants::QNX_TOOLCHAIN_ID) + QLatin1Char(':'));
}

ProjectExplorer::ToolChain *QnxToolChainFactory::restore(const QVariantMap &data)
{
    QnxToolChain *tc = new QnxToolChain(false);
    if (tc->fromMap(data))
        return tc;

    delete tc;
    return 0;
}

QList<ProjectExplorer::ToolChain *> QnxToolChainFactory::autoDetect()
{
    QtSupport::QtVersionManager *vm = QtSupport::QtVersionManager::instance();
    connect(vm, SIGNAL(qtVersionsChanged(QList<int>, QList<int>, QList<int>)),
            this, SLOT(handleQtVersionChanges(QList<int>, QList<int>, QList<int>)));

    QList<int> versionList;
    foreach (QtSupport::BaseQtVersion *v, vm->versions())
        versionList.append(v->uniqueId());

    return createToolChainList(versionList);
}

void QnxToolChainFactory::handleQtVersionChanges(const QList<int> &added, const QList<int> &removed, const QList<int> &changed)
{
    QList<int> changes;
    changes << added << removed << changed;

    ProjectExplorer::ToolChainManager *tcm = ProjectExplorer::ToolChainManager::instance();
    QList<ProjectExplorer::ToolChain *> tcList = createToolChainList(changes);
    foreach (ProjectExplorer::ToolChain *tc, tcList)
        tcm->registerToolChain(tc);
}


QList<ProjectExplorer::ToolChain *> QnxToolChainFactory::createToolChainList(
        const QList<int> &changes)
{
    ProjectExplorer::ToolChainManager *tcm = ProjectExplorer::ToolChainManager::instance();
    QtSupport::QtVersionManager *vm = QtSupport::QtVersionManager::instance();
    QList<ProjectExplorer::ToolChain *> result;

    foreach (int i, changes) {
        QtSupport::BaseQtVersion *v = vm->version(i);
        // remove tool chain (on removal, change or addition:
        QList<ProjectExplorer::ToolChain *> toRemove;
        foreach (ProjectExplorer::ToolChain *tc, tcm->toolChains()) {
            if (!tc->id().startsWith(QLatin1String(Constants::QNX_TOOLCHAIN_ID)))
                continue;
            QnxToolChain *qTc = static_cast<QnxToolChain *>(tc);
            if (qTc->qtVersionId() == i)
                toRemove.append(qTc);
        }
        foreach (ProjectExplorer::ToolChain *tc, toRemove)
            tcm->deregisterToolChain(tc);

        const QnxAbstractQtVersion * const qqv = dynamic_cast<QnxAbstractQtVersion *>(v);
        if (!qqv || !qqv->isValid())
            continue;

        // (Re-)add toolchain:
        // add tool chain:
        QnxToolChain *qTc = new QnxToolChain(true);
        qTc->setQtVersionId(i);
        qTc->setCompilerCommand(Utils::FileName::fromString(qqv->qnxHost()
                                                            + QLatin1String("/usr/bin/qcc")));
        switch (qqv->architecture()) {
        case X86:
            qTc->setDebuggerCommand(
                        Utils::FileName::fromString(qqv->qnxHost()
                                                    + QLatin1String("/usr/bin/ntox86-gdb")));
            break;
        case ArmLeV7:
            qTc->setDebuggerCommand(
                        Utils::FileName::fromString(qqv->qnxHost()
                                                    + QLatin1String("/usr/bin/ntoarm-gdb")));
            break;
        case UnknownArch:
            break;
        }

        result.append(qTc);
    }
    return result;
}
