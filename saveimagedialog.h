#ifndef SAVEIMAGEDIALOG_H
#define SAVEIMAGEDIALOG_H

#include <QDialog>

class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;


class SaveImageSettings
{
public:
    enum Units
    {
        Pixels,
        Millimeters,
    };

public:
    SaveImageSettings(QSize blueprintSize);

    Units units() const;
    bool alwaysIntegerSize() const;
    QSizeF size() const;
    int dpi() const;
    QString outputFile() const;

public:
    void setUnits(Units newUnits);
    void setWidth(double newWidth);
    void setHeight(double newHeight);
    void setDpi(int newDpi);
    void setOutputFile(const QString& newFile);

private:
    QSize m_blueprintSize;
    Units m_units;
    double m_scale;
    int m_dpi;
    QString m_outputFile;
};


class SaveImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveImageDialog(QSize blueprintSize, QWidget* parentArg = 0);

    SaveImageSettings settings() const;

private:
    SaveImageSettings m_settings;

    QLineEdit* m_outputFileLineEdit;
    QPushButton* m_browseButton;
    QComboBox* m_unitsComboBox;
    QDoubleSpinBox* m_widthDoubleSpinBox;
    QDoubleSpinBox* m_heightDoubleSpinBox;
    QSpinBox* m_dpiSpinBox;
    QLabel* m_otherUnitsLabel;
    QDialogButtonBox* m_buttonBox;

private:
    void connectSettings();
    void diconnectSettings();
    void updateWidgets();

private slots:
    void setUnits(int newUnitsIndex);
    void setWidth(double newWidth);
    void setHeight(double newHeight);
    void setDpi(int newDpi);
    void setOutputFile(const QString& newFile);
    void browse();
};

#endif // SAVEIMAGEDIALOG_H
