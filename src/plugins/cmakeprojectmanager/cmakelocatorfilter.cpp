
/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nicolas Arnaud-Cormos
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

#include "cmakelocatorfilter.h"
#include "cmaketarget.h"
#include "cmakeproject.h"
#include "makestep.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>


using namespace CMakeProjectManager;
using namespace CMakeProjectManager::Internal;

CMakeLocatorFilter::CMakeLocatorFilter():
    m_icon(QPixmap(QLatin1String(":/cmakeproject/images/cmake.png")))
{
    setShortcutString(QLatin1String("make"));
}

CMakeLocatorFilter::~CMakeLocatorFilter()
{

}

QList<Locator::FilterEntry> CMakeLocatorFilter::matchesFor(QFutureInterface<Locator::FilterEntry> &future, const QString &entry)
{
    Q_UNUSED(future)
    QList<Locator::FilterEntry> result;

    QList<ProjectExplorer::Project *> projects =
            ProjectExplorer::ProjectExplorerPlugin::instance()->session()->projects();
    foreach (ProjectExplorer::Project *p, projects) {
        CMakeProject *cmakeProject = qobject_cast<CMakeProject *>(p);
        if (cmakeProject) {
            foreach(CMakeBuildTarget ct, cmakeProject->buildTargets()) {
                if (ct.title != QLatin1String("all") && ct.title != QLatin1String("install")) {
                    if (ct.title.contains(entry)) {
                        Locator::FilterEntry entry(this, ct.title, cmakeProject->id(), m_icon);
                        result.append(entry);
                    }
                }
            }
        }
    }

    return result;
}

void CMakeLocatorFilter::accept(Locator::FilterEntry selection) const
{
    // Get the project
    CMakeProject *cmakeProject = 0;

    QList<ProjectExplorer::Project *> projects =
            ProjectExplorer::ProjectExplorerPlugin::instance()->session()->projects();
    foreach (ProjectExplorer::Project *p, projects) {
        cmakeProject = qobject_cast<CMakeProject *>(p);
        if (cmakeProject && cmakeProject->id() == selection.internalData.toString())
            break;
    }
    Q_ASSERT(cmakeProject);

    // Change the make step
    MakeStep *makeStep = qobject_cast<MakeStep *>(
                cmakeProject->activeTarget()->activeBuildConfiguration()
                ->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD)->at(0));
    Q_ASSERT(makeStep);
    QVariantMap map = makeStep->toMap();
    makeStep->setClean(false);
    makeStep->clearBuildTargets();
    makeStep->setBuildTarget(selection.displayName, true);

    // Build
    ProjectExplorer::ProjectExplorerPlugin::instance()->buildProject(cmakeProject);
    makeStep->fromMap(map);
}

void CMakeLocatorFilter::refresh(QFutureInterface<void> &future)
{
    Q_UNUSED(future)
}
