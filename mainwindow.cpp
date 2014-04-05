#include <exception>

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>

#include "blueprint.h"
#include "mainwindow.h"
#include "program.h"
#include "programparser.h"


const QString appName = "Mask plate convertor";

MainWindow::MainWindow(QWidget* parentArg)
    : QMainWindow(parentArg)
{
    setWindowTitle(appName);
    setCentralWidget(new QWidget(this));

    m_fileNameLineedit = new QLineEdit(this);
    m_fileNameLineedit->setPlaceholderText("Входной файл");
    m_browseButton = new QPushButton("Обзор...", this);
    m_convertButton = new QPushButton("Конвертировать", this);
    m_convertButton->setDefault(true);

    QBoxLayout* fileNameLayout = new QHBoxLayout;
    fileNameLayout->addWidget(m_fileNameLineedit);
    fileNameLayout->addWidget(m_browseButton);

    QBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_convertButton);

    QBoxLayout* mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->addLayout(fileNameLayout);
    mainLayout->addLayout(bottomLayout);

    resize(600, sizeHint().height());

    connect(m_browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    connect(m_convertButton, SIGNAL(clicked()), this, SLOT(convert()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::browse()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Введите имя входного файла", QString(), "Текстовые файлы (.txt);;Все файлы (*)");
    if (!fileName.isEmpty())
        m_fileNameLineedit->setText(fileName);
}

void MainWindow::convert()
{
    try {
        QString fileName = m_fileNameLineedit->text();
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, appName, QString("Не удалось открыть файл: \"%1\"").arg(fileName));
            return;
        }

        ProgramParser parser;

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            parser.processLine(line);
        }

        std::unique_ptr<Program> program = parser.finish();
        std::unique_ptr<Blueprint> blueprint = program->execute();
        QString output = blueprint->toAutocadCommandLineCommands();

        QApplication::clipboard()->setText(output);
        QMessageBox::information(this, appName, "Конвертация прошла успешно.\n(Результат скопирован в буфер обмена)");
    }
    catch (const std::exception &error) {
        QMessageBox::critical(this, QString("%1 — Ошибка").arg(appName), error.what());
    }
}
