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

#include "qnxtoolchain.h"

#include "qnxconstants.h"
#include "qnxtoolchainfactory.h"
#include "qnxabstractqtversion.h"

#include <qtsupport/qtversionmanager.h>
#include <utils/qtcassert.h>

using namespace Qnx;
using namespace Qnx::Internal;

QnxToolChain::QnxToolChain(bool autoDetected)
    : ProjectExplorer::GccToolChain(QLatin1String(Constants::QNX_TOOLCHAIN_ID), autoDetected)
    , m_qtVersionId(-1)
{
}

QnxToolChain::~QnxToolChain()
{

}

QString QnxToolChain::typeName() const
{
    return QnxToolChainFactory::tr("QNX GCC");
}

bool QnxToolChain::isValid() const
{
    return GccToolChain::isValid() && m_qtVersionId >= 0;
}

bool QnxToolChain::canClone() const
{
    return false;
}

bool QnxToolChain::operator ==(const ProjectExplorer::ToolChain &tc) const
{
    if (!ToolChain::operator ==(tc))
        return false;

    const QnxToolChain *tcPtr = static_cast<const QnxToolChain *>(&tc);
    return m_qtVersionId == tcPtr->m_qtVersionId;
}

QVariantMap QnxToolChain::toMap() const
{
    QVariantMap result = GccToolChain::toMap();
    result.insert(QLatin1String(Constants::QNX_QT_VERSION_KEY), m_qtVersionId);
    return result;
}

bool QnxToolChain::fromMap(const QVariantMap &data)
{
    if (!GccToolChain::fromMap(data))
        return false;

    m_qtVersionId = data.value(QLatin1String(Constants::QNX_QT_VERSION_KEY), -1).toInt();

    return isValid();
}

void QnxToolChain::setQtVersionId(int id)
{
    if (id < 0) {
        m_qtVersionId = -1;
        setTargetAbi(ProjectExplorer::Abi());
        return;
    }

    QnxAbstractQtVersion *version = dynamic_cast<QnxAbstractQtVersion *>(
                QtSupport::QtVersionManager::instance()->version(id));
    if (!version || !version->isValid())
        return;
    QTC_ASSERT(version->qtAbis().count() == 1, return);

    m_qtVersionId = id;

    setMkspecList(QList<Utils::FileName>() << version->mkspec());
    setDisplayName(QnxToolChainFactory::tr("QNX GCC for %1").arg(version->displayName()));
    setTargetAbi(version->qtAbis().at(0));
}

int QnxToolChain::qtVersionId() const
{
    return m_qtVersionId;
}

QList<ProjectExplorer::Abi> QnxToolChain::detectSupportedAbis() const
{
    QnxAbstractQtVersion *version = dynamic_cast<QnxAbstractQtVersion *>(
                QtSupport::QtVersionManager::instance()->version(m_qtVersionId));
    if (!version || !version->isValid())
        return QList<ProjectExplorer::Abi>();

    return version->qtAbis();
}
