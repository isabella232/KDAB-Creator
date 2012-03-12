#ifndef QNX_INTERNAL_BARDESCRIPTORFILEIMAGEWIZARDPAGE_H
#define QNX_INTERNAL_BARDESCRIPTORFILEIMAGEWIZARDPAGE_H

#include <QWizardPage>

namespace Qnx {
namespace Internal {

namespace Ui {
class BarDescriptorFileImageWizardPage;
}

class BarDescriptorFileImageWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit BarDescriptorFileImageWizardPage(QWidget *parent = 0);
    ~BarDescriptorFileImageWizardPage();

    bool isComplete() const;

    QString icon() const;
    QString landscapeSplashScreen() const;
    QString portraitSplashScreen() const;

private slots:
    void validateIcon(const QString &path);
    void validateLandscapeSplashScreen(const QString &path);
    void validatePortraitSplashScreen(const QString &path);

private:
    enum ImageValidationResult {
        Valid,
        CouldNotLoad,
        IncorrectSize
    };

    void updateSplashScreenValidationLabel();

    ImageValidationResult validateImage(const QString &path, const QSize &minimumSize, const QSize &maximumSize);
    QSize imageSize(const QString &path);

    Ui::BarDescriptorFileImageWizardPage *m_ui;

    ImageValidationResult m_iconValidationResult;
    ImageValidationResult m_landscapeSplashScreenValidationResult;
    ImageValidationResult m_portraitSplashScreenValidationResult;
};


} // namespace Internal
} // namespace Qnx
#endif // QNX_INTERNAL_BARDESCRIPTORFILEIMAGEWIZARDPAGE_H
