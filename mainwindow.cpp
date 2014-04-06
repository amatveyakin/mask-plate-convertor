#include <exception>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStyle>
#include <QTextBlock>
#include <QToolBar>

#include "blueprint.h"
#include "blueprintview.h"
#include "mainwindow.h"
#include "program.h"
#include "programparser.h"


const QString appName = "Mask plate convertor";

MainWindow::MainWindow(QWidget* parentArg)
    : QMainWindow(parentArg)
{
    setWindowTitle(appName);
    setCentralWidget(new QWidget(this));

    m_programTextEdit = new QPlainTextEdit(this);
    m_blueprintView = new BlueprintView(this);

    m_openAction = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), "Открыть...", this);
    m_saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Сохранить", this);
    m_convertAction = new QAction(QIcon(":/images/go.png"), "Конвертировать", this);

    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setShortcut(QKeySequence::Save);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_convertAction);
    addToolBar(toolbar);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(m_programTextEdit);
    splitter->addWidget(m_blueprintView);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);

    QLayout* mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->addWidget(splitter);

    resize(sizeHint().expandedTo(QSize(640, 480)));

    connect(m_openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_convertAction, SIGNAL(triggered()), this, SLOT(convert()));

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateWindowTitle()
{
    if (m_fileName.isEmpty())
        setWindowTitle(appName);
    else
        setWindowTitle(QString("%1 — %2").arg(m_fileName, appName));
}

void MainWindow::open()
{
    QString newFileName = QFileDialog::getOpenFileName(this, "Введите имя входного файла", QString(), "Текстовые файлы (.txt);;Все файлы (*)");
    if (newFileName.isEmpty())
        return;

    QFile file(newFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, appName, QString("Не удалось открыть файл: \"%1\"").arg(newFileName));
        return;
    }

    m_programTextEdit->setPlainText(file.readAll());
    m_fileName = newFileName;
    updateWindowTitle();
}

void MainWindow::save()
{
    // TODO
}

void MainWindow::convert()
{
    QString errorHeader = QString("%1 — Ошибка").arg(appName);
    try {
        m_blueprintView->setBlueprint(nullptr);
        QStringList programLines = m_programTextEdit->toPlainText().split('\n');
        ProgramParser parser;
        for (const QString& line : programLines)
            parser.processLine(line);

        std::unique_ptr<Program> program = parser.finish();
        m_blueprint = program->execute();
        m_blueprintView->setBlueprint(m_blueprint.get());
        QString output = m_blueprint->toAutocadCommandLineCommands();

        QApplication::clipboard()->setText(output);
        QMessageBox::information(this, appName, "Конвертация прошла успешно.\n(Результат скопирован в буфер обмена)");
    }
    catch (const ParseError &error) {
        QTextCursor newCursor(m_programTextEdit->document()->findBlockByLineNumber(error.position().line));
        newCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, error.position().column);
        m_programTextEdit->setTextCursor(newCursor);
        QMessageBox::critical(this, errorHeader, error.what());
    }
    catch (const std::exception &error) {
        QMessageBox::critical(this, errorHeader, error.what());
    }
}
