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
#include "autocadlanguage.h"
#include "autocadsettingsdialog.h"
#include "utils.h"


AutocadSettingsDialog::AutocadSettingsDialog(AutocadConvertorSettings settings, QWidget* parentArg)
    : QDialog(parentArg)
    , m_settings(settings)
{
    setWindowTitle(titleText("Параметры AutoCAD"));

    m_languageComboBox = new QComboBox(this);
    for (const auto& language : AutocadLanguageFactory::singleton().allLanguages())
        m_languageComboBox->addItem(language->codeName(), static_cast<int>(language->code()));

    m_sizeCoeffLineEdit = new QLineEdit(this);
    auto* sizeCoeffValidator = new QDoubleValidator(this);
    sizeCoeffValidator->setBottom(0.);
    m_sizeCoeffLineEdit->setValidator(sizeCoeffValidator);

    m_buttonBox = new QDialogButtonBox;
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QLabel* languageLabel = new QLabel("Язык команд Autocad", this);
    QLabel* sizeCoeffLabel = new QLabel("Коэффициент масштабирования", this);
    QGridLayout* gridLayout = new QGridLayout;

    gridLayout->addWidget(languageLabel,            0,  0);
    gridLayout->addWidget(m_languageComboBox,       0,  1);
    gridLayout->addWidget(sizeCoeffLabel,           1,  0);
    gridLayout->addWidget(m_sizeCoeffLineEdit,      1,  1);

    QBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(gridLayout);
    topLayout->addWidget(m_buttonBox);

    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    initWidgets();
    connectSettings();
    resize(sizeHint().expandedTo(QSize(360, 1)));
}

AutocadConvertorSettings AutocadSettingsDialog::settings() const
{
    return m_settings;
}

void AutocadSettingsDialog::initWidgets()
{
    m_languageComboBox->setCurrentIndex(static_cast<int>(m_settings.autocadLanguageCode));
    m_sizeCoeffLineEdit->setText(QString::number(m_settings.sizeCoeff));
}

void AutocadSettingsDialog::connectSettings()
{
    connect(m_languageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setLanguage(int)));
    connect(m_sizeCoeffLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setSizeCoeff(QString)));
}

void AutocadSettingsDialog::setLanguage(int newLanguageCode)
{
    m_settings.autocadLanguageCode = static_cast<AutocadLanguageCode>(newLanguageCode);
}

void AutocadSettingsDialog::setSizeCoeff(QString newSizeCoeff)
{
    m_settings.sizeCoeff = newSizeCoeff.toDouble();
}
