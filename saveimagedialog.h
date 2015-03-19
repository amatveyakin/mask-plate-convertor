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
    Units m_units = Units::Pixels;
    double m_scale = 1.;
    int m_dpi = 300;
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

    QLineEdit* m_outputFileLineEdit = nullptr;
    QPushButton* m_browseButton = nullptr;
    QComboBox* m_unitsComboBox = nullptr;
    QDoubleSpinBox* m_widthDoubleSpinBox = nullptr;
    QDoubleSpinBox* m_heightDoubleSpinBox = nullptr;
    QSpinBox* m_dpiSpinBox = nullptr;
    QLabel* m_otherUnitsLabel = nullptr;
    QDialogButtonBox* m_buttonBox = nullptr;

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
