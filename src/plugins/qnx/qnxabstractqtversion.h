#ifndef QNX_INTERNAL_QNXABSTRACTQTVERSION_H
#define QNX_INTERNAL_QNXABSTRACTQTVERSION_H

#include "qnxconstants.h"

#include <qtsupport/baseqtversion.h>

namespace Qnx {
namespace Internal {

class QnxAbstractQtVersion : public QtSupport::BaseQtVersion
{
    friend class QnxBaseQtConfigWidget;
public:
    QnxAbstractQtVersion();
    QnxAbstractQtVersion(QnxArchitecture arch, const Utils::FileName &path,
                 bool isAutoDetected = false,
                 const QString &autoDetectionSource = QString());

    QString qnxHost() const;
    QString qnxTarget() const;

    QnxArchitecture architecture() const;
    QString archString() const;

    QVariantMap toMap() const;
    void fromMap(const QVariantMap &map);

    bool supportsTargetId(const QString &id) const;

    QList<ProjectExplorer::Abi> detectQtAbis() const;

    void addToEnvironment(Utils::Environment &env) const;

    QtSupport::QtConfigWidget *createConfigurationWidget() const;

    bool isValid() const;
    QString invalidReason() const;

    virtual QString sdkDescription() const = 0;

protected:
    QString sdkPath() const;

private:
    void updateEnvironment() const;
    virtual QMap<QString, QString> environment() const = 0;

    void setSdkPath(const QString &sdkPath);

    QnxArchitecture m_arch;
    QString m_sdkPath;

    mutable bool m_environmentUpToDate;
    mutable QMap<QString, QString> m_envMap;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_QNXABSTRACTQTVERSION_H
