#ifndef AUTOCADSETTINGSDIALOG_H
#define AUTOCADSETTINGSDIALOG_H

#include <QDialog>

#include "autocadconvertorsettings.h"

class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;


class AutocadSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutocadSettingsDialog(AutocadConvertorSettings settings, QWidget* parentArg = 0);

    AutocadConvertorSettings settings() const;

private:
    AutocadConvertorSettings m_settings;

    QComboBox* m_languageComboBox = nullptr;
    QLineEdit* m_sizeCoeffLineEdit = nullptr;
    QDialogButtonBox* m_buttonBox = nullptr;

private:
    void initWidgets();
    void connectSettings();

private slots:
    void setLanguage(int newLanguageCode);
    void setSizeCoeff(QString newSizeCoeff);
};

#endif // AUTOCADSETTINGSDIALOG_H
