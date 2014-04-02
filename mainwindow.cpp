#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
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
}

MainWindow::~MainWindow()
{
}
