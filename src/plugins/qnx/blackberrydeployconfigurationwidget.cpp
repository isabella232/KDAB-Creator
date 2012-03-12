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

#include "blackberrydeployconfigurationwidget.h"
#include "ui_blackberrydeployconfigurationwidget.h"
#include "blackberrydeployconfiguration.h"

#include <coreplugin/icore.h>
#include <remotelinux/abstractembeddedlinuxtarget.h>
#include <remotelinux/linuxdeviceconfigurations.h>
#include <remotelinux/remotelinux_constants.h>
#include <remotelinux/typespecificdeviceconfigurationlistmodel.h>
#include <utils/pathchooser.h>

using namespace Qnx;
using namespace Qnx::Internal;

BlackBerryDeployConfigurationWidget::BlackBerryDeployConfigurationWidget(QWidget *parent)
    : ProjectExplorer::DeployConfigurationWidget(parent)
    , m_ui(new Ui::BlackBerryDeployConfigurationWidget)
    , m_deployConfiguration(0)
{
    m_ui->setupUi(this);
}

BlackBerryDeployConfigurationWidget::~BlackBerryDeployConfigurationWidget()
{
    delete m_ui;
}

void BlackBerryDeployConfigurationWidget::init(ProjectExplorer::DeployConfiguration *dc)
{
    m_deployConfiguration = qobject_cast<BlackBerryDeployConfiguration *>(dc);

    m_ui->deviceCombo->setModel(m_deployConfiguration->target()->deviceConfigModel());
    m_ui->barFilePath->setExpectedKind(Utils::PathChooser::File);
    m_ui->barFilePath->setPromptDialogFilter(QLatin1String("*.bar"));
    m_ui->barFilePath->setFileName(Utils::FileName::fromString(m_deployConfiguration->barPackage()));

    connect(m_deployConfiguration, SIGNAL(targetInformationChanged()), this, SLOT(updateUi()));
    connect(m_deployConfiguration, SIGNAL(deviceConfigurationListChanged()),
            SLOT(handleDeviceConfigurationListChanged()));

    connect(m_ui->deviceCombo, SIGNAL(activated(int)),
            m_deployConfiguration, SLOT(setCurrentDevice(int)));
    connect(m_ui->manageDevicesLabel, SIGNAL(linkActivated(QString)),
            this, SLOT(showDeviceConfigurations()));
    connect(m_ui->barFilePath, SIGNAL(changed(QString)),
            this, SLOT(updateBarPackage(QString)));

    handleDeviceConfigurationListChanged();
}

void BlackBerryDeployConfigurationWidget::showDeviceConfigurations()
{
    Core::ICore::instance()->showOptionsDialog(RemoteLinux::Constants::RemoteLinuxSettingsCategory,
                                               RemoteLinux::Constants::RemoteLinuxSettingsPageId);
}

void BlackBerryDeployConfigurationWidget::updateUi()
{
    bool blocked = m_ui->barFilePath->blockSignals(true);
    m_ui->barFilePath->setFileName(Utils::FileName::fromString(m_deployConfiguration->barPackage()));
    m_ui->barFilePath->blockSignals(blocked);
}

void BlackBerryDeployConfigurationWidget::updateBarPackage(const QString &barPackage)
{
    // To prevent the updating of the bar package to propagate back here,
    // causing the cursor to move to the end of the line edit
    // Can't block all signals, as that will prevent it from updating the run configuration widget
    disconnect(m_deployConfiguration, SIGNAL(targetInformationChanged()), this, SLOT(updateUi()));
    m_deployConfiguration->setBarPackage(barPackage);
    connect(m_deployConfiguration, SIGNAL(targetInformationChanged()), this, SLOT(updateUi()));
}

void BlackBerryDeployConfigurationWidget::handleDeviceConfigurationListChanged()
{
    const RemoteLinux::LinuxDeviceConfiguration::ConstPtr &devConf =
            m_deployConfiguration->deviceConfiguration();
    const RemoteLinux::LinuxDeviceConfiguration::Id internalId =
            RemoteLinux::LinuxDeviceConfigurations::instance()->internalId(devConf);
    const int newIndex =
            RemoteLinux::LinuxDeviceConfigurations::instance()->indexForInternalId(internalId);
    m_ui->deviceCombo->setCurrentIndex(newIndex);
}
