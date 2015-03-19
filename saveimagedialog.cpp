// TODO: Don't change data explicitly set by user! (it sometimes happens that width/height being entered right now is rounded)

#include <assert.h>

#include <QBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include "appinfo.h"
#include "saveimagedialog.h"
#include "utils.h"


static const double maxSize = 1e9;
static const int maxDpi = 10000;
static const int sizeDecimals = 2;

static inline QString unitsString(SaveImageSettings::Units units)
{
    switch (units) {
    case SaveImageSettings::Pixels:         return "px";
    case SaveImageSettings::Millimeters:    return "mm";
    }
    assert(false);
}

static inline bool alwaysIntegerUnits(SaveImageSettings::Units units)
{
    switch (units) {
    case SaveImageSettings::Pixels:         return true;
    case SaveImageSettings::Millimeters:    return false;
    }
    assert(false);
}

static inline double pixelsToUnitsCoeff(SaveImageSettings::Units units, int dpi)
{
    double inchedToMillimetersCoeff = 25.4;
    switch (units) {
    case SaveImageSettings::Pixels:         return 1.;
    case SaveImageSettings::Millimeters:    return (1. * inchedToMillimetersCoeff) / dpi;
    }
    assert(false);
}

static inline SaveImageSettings::Units extraUnits(SaveImageSettings::Units units)
{
    switch (units) {
    case SaveImageSettings::Pixels:         return SaveImageSettings::Millimeters;
    case SaveImageSettings::Millimeters:    return SaveImageSettings::Pixels;
    }
    assert(false);
}

static inline QSize round(QSizeF s)
{
    return QSize(qRound(s.width()), qRound(s.height()));
}


SaveImageSettings::SaveImageSettings(QSize blueprintSize)
{
    m_blueprintSize = blueprintSize;
    m_units = Pixels;  // TODO: May be, propose a reasonable size in millimeters better?
    m_scale = 0.1;
    m_dpi = 300;
}

SaveImageSettings::Units SaveImageSettings::units() const
{
    return m_units;
}

bool SaveImageSettings::alwaysIntegerSize() const
{
    return alwaysIntegerUnits(m_units);
}

QSizeF SaveImageSettings::size() const
{
    QSizeF result = QSizeF(m_blueprintSize) * (m_scale * pixelsToUnitsCoeff(m_units, m_dpi));
    return alwaysIntegerSize() ? round(result).expandedTo(QSize(1, 1)) : result;
}

int SaveImageSettings::dpi() const
{
    return m_dpi;
}

QString SaveImageSettings::outputFile() const
{
    return m_outputFile;
}

void SaveImageSettings::setUnits(SaveImageSettings::Units newUnits)
{
    m_units = newUnits;
}

void SaveImageSettings::setWidth(double newWidth)
{
    m_scale = newWidth / (m_blueprintSize.width() * pixelsToUnitsCoeff(m_units, m_dpi));
}

void SaveImageSettings::setHeight(double newHeight)
{
    m_scale = newHeight / (m_blueprintSize.height() * pixelsToUnitsCoeff(m_units, m_dpi));
}

void SaveImageSettings::setDpi(int newDpi)
{
    m_dpi = newDpi;
}

void SaveImageSettings::setOutputFile(const QString& newFile)
{
    m_outputFile = newFile;
}


SaveImageDialog::SaveImageDialog(QSize blueprintSize, QWidget* parentArg)
    : QDialog(parentArg)
    , m_settings(blueprintSize)
{
    setWindowTitle(titleText("Сохранить изображение"));

    m_outputFileLineEdit = new QLineEdit(this);
    m_outputFileLineEdit->setPlaceholderText("Имя выходного файла");

    m_browseButton = new QPushButton("Обзор...", this);

    m_widthDoubleSpinBox = new QDoubleSpinBox(this);
    m_widthDoubleSpinBox->setRange(0., maxSize);

    m_heightDoubleSpinBox = new QDoubleSpinBox(this);
    m_heightDoubleSpinBox->setRange(0., maxSize);

    m_dpiSpinBox = new QSpinBox(this);
    m_dpiSpinBox->setRange(0, maxDpi);

    m_unitsComboBox = new QComboBox(this);
    for (SaveImageSettings::Units units : {SaveImageSettings::Pixels, SaveImageSettings::Millimeters})
        m_unitsComboBox->addItem(unitsString(units), static_cast<int>(units));

    m_otherUnitsLabel = new QLabel(this);

    m_buttonBox = new QDialogButtonBox;
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QBoxLayout* outputFileLayout = new QHBoxLayout;
    outputFileLayout->addWidget(m_outputFileLineEdit);
    outputFileLayout->addWidget(m_browseButton);

    enum { WidthRow, HeightRow, DpiRow };
    enum { LabelColumn, ValueColumn, AdditionalColumn, StretchColumn };

    m_widthDoubleSpinBox->setDecimals(sizeDecimals);  // will be overwritten in `updateWidgets'
    int sizeSpinBoxesWidth = m_widthDoubleSpinBox->sizeHint().width();
    m_widthDoubleSpinBox->setFixedWidth(sizeSpinBoxesWidth);
    m_heightDoubleSpinBox->setFixedWidth(sizeSpinBoxesWidth);

    QLabel* widthLabel = new QLabel("Ширина", this);
    QLabel* heightLabel = new QLabel("Высота", this);
    QLabel* dpiLabel = new QLabel("DPI", this);
    QSpacerItem* horizontalSpacing = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored);
    QGridLayout* gridLayout = new QGridLayout;

    gridLayout->addWidget(widthLabel,               WidthRow,   LabelColumn);
    gridLayout->addWidget(m_widthDoubleSpinBox,     WidthRow,   ValueColumn);
    gridLayout->addWidget(m_unitsComboBox,          WidthRow,   AdditionalColumn);
    gridLayout->addItem  (horizontalSpacing,        WidthRow,   StretchColumn);
    gridLayout->addWidget(heightLabel,              HeightRow,  LabelColumn);
    gridLayout->addWidget(m_heightDoubleSpinBox,    HeightRow,  ValueColumn);
    gridLayout->addWidget(dpiLabel,                 DpiRow,     LabelColumn);
    gridLayout->addWidget(m_dpiSpinBox,             DpiRow,     ValueColumn);

    // TODO: fix ValueColumn width

    QBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(outputFileLayout);
    topLayout->addLayout(gridLayout);
    topLayout->addWidget(m_otherUnitsLabel);
    topLayout->addWidget(m_buttonBox);

    connect(m_browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connectSettings();
    updateWidgets();
    resize(sizeHint().expandedTo(QSize(480, 1)));
}

SaveImageSettings SaveImageDialog::settings() const
{
    return m_settings;
}

void SaveImageDialog::connectSettings()
{
    connect(m_outputFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setOutputFile(QString)));
    connect(m_unitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setUnits(int)));
    connect(m_widthDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
    connect(m_heightDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
    connect(m_dpiSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setDpi(int)));
}

void SaveImageDialog::diconnectSettings()
{
    disconnect(m_outputFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setOutputFile(QString)));
    disconnect(m_unitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setUnits(int)));
    disconnect(m_widthDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWidth(double)));
    disconnect(m_heightDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHeight(double)));
    disconnect(m_dpiSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setDpi(int)));
}

void SaveImageDialog::updateWidgets()
{
    diconnectSettings();

    SaveImageSettings::Units otherUnits = extraUnits(m_settings.units());
    SaveImageSettings extraSettings = m_settings;
    extraSettings.setUnits(otherUnits);

    m_outputFileLineEdit->setText(m_settings.outputFile());
    m_unitsComboBox->setCurrentIndex(m_unitsComboBox->findData(m_settings.units()));
    int decimals = m_settings.alwaysIntegerSize() ? 0 : sizeDecimals;  // TODO: Is it a good way to turn double spinboxes into int spinboxes?
    m_widthDoubleSpinBox->setDecimals(decimals);
    m_heightDoubleSpinBox->setDecimals(decimals);
    m_widthDoubleSpinBox->setValue(m_settings.size().width());
    m_heightDoubleSpinBox->setValue(m_settings.size().height());
    m_dpiSpinBox->setValue(m_settings.dpi());
    m_otherUnitsLabel->setText(QString("(%1×%2 %3)").arg(prettyPrintNumber(extraSettings.size().width()),
                                                         prettyPrintNumber(extraSettings.size().height()),
                                                         unitsString(extraSettings.units())));

    connectSettings();
}

void SaveImageDialog::setUnits(int newUnitsIndex)
{
    m_settings.setUnits(static_cast<SaveImageSettings::Units>(m_unitsComboBox->itemData(newUnitsIndex).toInt()));
    updateWidgets();
}

void SaveImageDialog::setWidth(double newWidth)
{
    m_settings.setWidth(newWidth);
    updateWidgets();
}

void SaveImageDialog::setHeight(double newHeight)
{
    m_settings.setHeight(newHeight);
    updateWidgets();
}

void SaveImageDialog::setDpi(int newDpi)
{
    m_settings.setDpi(newDpi);
    updateWidgets();
}

void SaveImageDialog::setOutputFile(const QString& newFile)
{
    m_settings.setOutputFile(newFile);
    updateWidgets();
}

void SaveImageDialog::browse()
{
    // TODO: Support SVG
    QString fileName = QFileDialog::getSaveFileName(this, titleText("Сохранить изображение"), QString(), "Изображения PNG (*.png);;Изображения BMP (*.bmp)");
    if (!fileName.isEmpty())
        m_outputFileLineEdit->setText(fileName);
}
