#include <exception>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTextBlock>
#include <QToolBar>

#include "blueprint.h"
#include "blueprintview.h"
#include "mainwindow.h"
#include "program.h"
#include "programparser.h"


static const QString appName = "Mask plate convertor";
static const int statusMessageDuration = 5000;  // ms


MainWindow::MainWindow(QWidget* parentArg)
    : QMainWindow(parentArg)
{
    setWindowTitle(appName);
    setCentralWidget(new QWidget(this));
    statusBar();  // show status bar

    m_programTextEdit = new QPlainTextEdit(this);
    m_programTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_blueprintView = new BlueprintView(this);

    m_newAction = new QAction(QIcon(":/images/new_document.png"), "Новый", this);
    m_openAction = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), "Открыть...", this);
    m_saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Сохранить", this);
    m_undoAction = new QAction(QIcon(":/images/undo.png"), "Отменить", this);
    m_redoAction = new QAction(QIcon(":/images/redo.png"), "Повторить", this);
    m_convertAction = new QAction(QIcon(":/images/go.png"), "Конвертировать", this);

    m_newAction->setShortcut(QKeySequence::New);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setShortcut(QKeySequence::Save);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_redoAction->setShortcut(QKeySequence::Redo);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addAction(m_newAction);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_saveAction);
    toolbar->addSeparator();
    toolbar->addAction(m_undoAction);
    toolbar->addAction(m_redoAction);
    toolbar->addSeparator();
    toolbar->addAction(m_convertAction);
    addToolBar(toolbar);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(m_programTextEdit);
    splitter->addWidget(m_blueprintView);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    QLayout* mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->addWidget(splitter);

    resize(sizeHint().expandedTo(QSize(800, 600)));

    QTextDocument* document = m_programTextEdit->document();

    m_undoAction->setEnabled(document->isUndoAvailable());
    m_redoAction->setEnabled(document->isRedoAvailable());

    connect(m_newAction, SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveDocument()));

    connect(m_undoAction, SIGNAL(triggered()), document, SLOT(undo()));
    connect(m_redoAction, SIGNAL(triggered()), document, SLOT(redo()));
    connect(document, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)));
    connect(document, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)));
    connect(document, SIGNAL(modificationChanged(bool)), this, SLOT(updateWindowTitle()));

    connect(m_convertAction, SIGNAL(triggered()), this, SLOT(convert()));

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (confirmClose())
        event->accept();
    else
        event->ignore();
}

void MainWindow::setBlueprint(std::unique_ptr<Blueprint> newBlueprint)
{
    m_blueprint = std::move(newBlueprint);
    m_blueprintView->setBlueprint(m_blueprint.get());
}

bool MainWindow::confirmClose()
{
    assert(m_programTextEdit->document());
    if (!m_programTextEdit->document()->isModified())
        return true;

    int res = QMessageBox::warning(this, QString("%1 — Сохранить?").arg(appName), "Файл содержит несохранённые изменения. Сохранить?",
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);
    switch (res) {
    case QMessageBox::Yes:
        return saveDocument();
    case QMessageBox::No:
        return true;
    case QMessageBox::Cancel:
        return false;
    }
    assert(false);
}

void MainWindow::updateWindowTitle()
{
    if (m_fileName.isEmpty())
        setWindowTitle(appName);
    else {
        QString modificationMarker = m_programTextEdit->document()->isModified() ? "*" : "";
        setWindowTitle(QString("%1%2 — %3").arg(m_fileName, modificationMarker, appName));
    }
}

bool MainWindow::newDocument()
{
    if (!confirmClose())
        return false;
    m_programTextEdit->clear();
    m_fileName.clear();
    setBlueprint(nullptr);
    updateWindowTitle();
    return true;
}

bool MainWindow::openDocument()
{
    if (!confirmClose())
        return false;

    QString newFileName = QFileDialog::getOpenFileName(this, QString("%1 — открыть файл").arg(appName), QString(), "Текстовые файлы (*.txt);;Все файлы (*)");
    if (newFileName.isEmpty())
        return false;

    QFile file(newFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, appName, QString("Не удалось открыть файл: \"%1\"").arg(newFileName));
        return false;
    }

    m_programTextEdit->setPlainText(file.readAll());
    m_fileName = newFileName;
    setBlueprint(nullptr);
    updateWindowTitle();
    return true;
}

bool MainWindow::saveDocument()
{
    if (m_fileName.isEmpty())
        return saveDocumentAs();
    return doSaveDocument(m_fileName);
}

bool MainWindow::saveDocumentAs()
{
    QString newFileName = QFileDialog::getSaveFileName(this, QString("%1 — сохранить файл").arg(appName), QString(), "Текстовые файлы (*.txt);;Все файлы (*)");
    if (newFileName.isEmpty())
        return false;
    return doSaveDocument(newFileName);
}

bool MainWindow::doSaveDocument(const QString& target)
{
    QFile file(target);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, appName, QString("Не удалось открыть на запись файл: \"%1\"").arg(target));
        return false;
    }

    file.write(m_programTextEdit->toPlainText().toUtf8());
    m_programTextEdit->document()->clearUndoRedoStacks();
    m_programTextEdit->document()->setModified(false);
    m_fileName = target;
    updateWindowTitle();
    return true;
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
        setBlueprint(program->execute());
        QString output = m_blueprint->toAutocadCommandLineCommands();

        QApplication::clipboard()->setText(output);
        statusBar()->showMessage("Конвертация прошла успешно.\n(Результат скопирован в буфер обмена)", statusMessageDuration);
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
