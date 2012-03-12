#include "qnxabstractqtversion.h"

#include "qnxbaseqtconfigwidget.h"

#include <utils/environment.h>

#include <QtCore/QDir>

using namespace Qnx;
using namespace Qnx::Internal;

QnxAbstractQtVersion::QnxAbstractQtVersion()
    : QtSupport::BaseQtVersion()
    , m_arch(UnknownArch)
{
}

QnxAbstractQtVersion::QnxAbstractQtVersion(QnxArchitecture arch, const Utils::FileName &path, bool isAutoDetected, const QString &autoDetectionSource)
    : QtSupport::BaseQtVersion(path, isAutoDetected, autoDetectionSource)
    , m_arch(arch)
{
}

QnxArchitecture QnxAbstractQtVersion::architecture() const
{
    return m_arch;
}

QString QnxAbstractQtVersion::archString() const
{
    switch (m_arch) {
    case X86:
        return QLatin1String("x86");
    case ArmLeV7:
        return QLatin1String("ARMle-v7");
    case UnknownArch:
        return QString();
    }

    return QString();
}

QVariantMap QnxAbstractQtVersion::toMap() const
{
    QVariantMap result = BaseQtVersion::toMap();
    result.insert(QLatin1String("SDKPath"), sdkPath());
    result.insert(QLatin1String("Arch"), m_arch);
    return result;
}

void QnxAbstractQtVersion::fromMap(const QVariantMap &map)
{
    BaseQtVersion::fromMap(map);
    setSdkPath(QDir::fromNativeSeparators(map.value(QLatin1String("SDKPath")).toString()));
    m_arch = static_cast<QnxArchitecture>(map.value(QLatin1String("Arch"), UnknownArch).toInt());
}

QList<ProjectExplorer::Abi> QnxAbstractQtVersion::detectQtAbis() const
{
    ensureMkSpecParsed();
    return qtAbisFromLibrary(qtCorePath(versionInfo(), qtVersionString()));
}

bool QnxAbstractQtVersion::supportsTargetId(const QString &id) const
{
    return supportedTargetIds().contains(id);
}

void QnxAbstractQtVersion::addToEnvironment(Utils::Environment &env) const
{
    QtSupport::BaseQtVersion::addToEnvironment(env);

    if (!m_environmentUpToDate)
        updateEnvironment();

    QMap<QString, QString>::const_iterator it;
    for (it = m_envMap.constBegin(); it != m_envMap.constEnd(); ++it) {
        const QString key = it.key();
        const QString value = it.value();

        if (key == QLatin1String("PATH"))
            env.prependOrSetPath(value);
        else if (key == QLatin1String("LD_LIBRARY_PATH"))
            env.prependOrSetLibrarySearchPath(value);
        else
            env.set(key, value);
    }

    env.prependOrSetLibrarySearchPath(versionInfo().value(QLatin1String("QT_INSTALL_LIBS")));
}

QString QnxAbstractQtVersion::sdkPath() const
{
    return m_sdkPath;
}

void QnxAbstractQtVersion::setSdkPath(const QString &sdkPath)
{
    if (m_sdkPath == sdkPath)
        return;

    m_sdkPath = sdkPath;
    m_environmentUpToDate = false;
}

void QnxAbstractQtVersion::updateEnvironment() const
{
    m_envMap = environment();
    m_environmentUpToDate = true;
}

QString QnxAbstractQtVersion::qnxHost() const
{
    if (!m_environmentUpToDate)
        updateEnvironment();

    return m_envMap.value(QLatin1String(Constants::QNX_HOST_KEY));
}

QString QnxAbstractQtVersion::qnxTarget() const
{
    if (!m_environmentUpToDate)
        updateEnvironment();

    return m_envMap.value(QLatin1String(Constants::QNX_TARGET_KEY));
}

QtSupport::QtConfigWidget *QnxAbstractQtVersion::createConfigurationWidget() const
{
    return new QnxBaseQtConfigWidget(const_cast<QnxAbstractQtVersion *>(this));
}

bool QnxAbstractQtVersion::isValid() const
{
    return QtSupport::BaseQtVersion::isValid() && !sdkPath().isEmpty();
}

QString QnxAbstractQtVersion::invalidReason() const
{
    if (sdkPath().isEmpty())
        return QCoreApplication::translate("QtVersion", "No SDK path set");

    return QtSupport::BaseQtVersion::invalidReason();
}
