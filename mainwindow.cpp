// TODO: blink status bar background when showing message
// TODO: Increase font size
// TODO: Allow to change laser width when drawing (?) (or may be, only when laser is enabled, but no when drawing)
// TODO: Step-by-step drawing
// TODO: Add recent documents

#include <exception>

#include <QAbstractItemModel>
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QFileDialog>
#include <QListView>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPrintPreviewDialog>
#include <QScrollBar>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTextBlock>
#include <QTextLayout>
#include <QToolBar>

#include "appinfo.h"
#include "autocadconvertor.h"
#include "blueprint.h"
#include "blueprintview.h"
#include "logdatamodel.h"
#include "mainwindow.h"
#include "program.h"
#include "programparser.h"
#include "runningprogram.h"
#include "saveimagedialog.h"


static const int statusMessageDuration = 5000;  // ms


MainWindow::MainWindow(QWidget* parentArg)
    : QMainWindow(parentArg)
{
    setWindowTitle(titleText());
    setCentralWidget(new QWidget(this));
    statusBar();  // show status bar

    initTextFormats();

    m_logModel = new LogDataModel(this);

    m_programTextEdit = new QPlainTextEdit(this);
    m_programTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_blueprintView = new BlueprintView(this);

    m_logView = new QListView(this);
    m_logView->setModel(m_logModel);
    m_logView->hide();

    m_newAction = new QAction(QIcon(":/images/new_document.png"), "Новый", this);
    m_openAction = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), "Открыть...", this);
    m_saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Сохранить", this);
    // TODO: Add ``Save as...''
    m_undoAction = new QAction(QIcon(":/images/undo.png"), "Отменить", this);
    m_redoAction = new QAction(QIcon(":/images/redo.png"), "Повторить", this);
    m_convertAction = new QAction(QIcon(":/images/go.png"), "Конвертировать", this);
    m_flipHorizontallyAction = new QAction(QIcon(":/images/flip_horizontally.png"), "Отразить по горизонтали", this);
    m_flipVerticallyAction = new QAction(QIcon(":/images/flip_vertically.png"), "Отразить по вертикали", this);
    m_saveImageAction = new QAction(QIcon(":/images/save_image.png"), "Сохранить изображение...", this);
    m_printImageAction = new QAction(QIcon(":/images/print_image.png"), "Распечатать изображение...", this);

    m_flipHorizontallyAction->setCheckable(true);
    m_flipVerticallyAction->setCheckable(true);

    m_newAction->setShortcut(QKeySequence::New);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setShortcut(QKeySequence::Save);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_convertAction->setShortcut(Qt::CTRL | Qt::Key_Return);
    m_printImageAction->setShortcut(QKeySequence::Print);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addAction(m_newAction);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_saveAction);
    toolbar->addSeparator();
    toolbar->addAction(m_undoAction);
    toolbar->addAction(m_redoAction);
    toolbar->addSeparator();
    toolbar->addAction(m_convertAction);
    toolbar->addSeparator();
    toolbar->addAction(m_flipHorizontallyAction);
    toolbar->addAction(m_flipVerticallyAction);
    toolbar->addSeparator();
    toolbar->addAction(m_saveImageAction);
    toolbar->addAction(m_printImageAction);
    addToolBar(toolbar);

    QWidget* editorWidget = new QWidget(this);

    QBoxLayout* editorLayout = new QVBoxLayout(editorWidget);
    editorLayout->addWidget(m_programTextEdit);
    editorLayout->addWidget(m_logView);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(editorWidget);
    splitter->addWidget(m_blueprintView);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    QLayout* mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->addWidget(splitter);

    resize(sizeHint().expandedTo(QSize(800, 600)));

    QTextDocument* document = m_programTextEdit->document();

    m_undoAction->setEnabled(document->isUndoAvailable());
    m_redoAction->setEnabled(document->isRedoAvailable());

    connect(m_newAction,  SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveDocument()));

    connect(m_undoAction, SIGNAL(triggered()), document, SLOT(undo()));
    connect(m_redoAction, SIGNAL(triggered()), document, SLOT(redo()));
    connect(document, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)));
    connect(document, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)));
    connect(document, SIGNAL(modificationChanged(bool)), this, SLOT(updateWindowTitle()));
    connect(document, SIGNAL(contentsChanged()), this, SLOT(clearAdditionalFormats()));

    connect(m_convertAction, SIGNAL(triggered()), this, SLOT(convert()));
    connect(m_saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));
    connect(m_printImageAction, SIGNAL(triggered()), this, SLOT(printImage()));

    connect(m_flipHorizontallyAction, SIGNAL(toggled(bool)), m_blueprintView, SLOT(setflipHorizontally(bool)));
    connect(m_flipVerticallyAction,   SIGNAL(toggled(bool)), m_blueprintView, SLOT(setflipVertically(bool)));

    connect(m_programTextEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateCurrentLineHighlighting()));

    connect(m_logView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateOnLogItemClicked(QModelIndex)));

    updateWindowTitle();
    updateCurrentLineHighlighting();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    if (confirmClose())
        ev->accept();
    else
        ev->ignore();
}

void MainWindow::initTextFormats()
{
    m_currentLineFormat.setBackground(QColor::fromRgbF(0.2, 0.2, 0.6, 0.15));
    m_currentLineFormat.setProperty(QTextFormat::FullWidthSelection, true);

    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
}

void MainWindow::showProgramError(TextRange range, const QString& message, const CallStack& callStack)
{
    const TextPosition begin = range.begin;
    const TextPosition end = range.end.valid() ? range.end : TextPosition(begin.line, std::numeric_limits<int>::max());
    assert(begin.line == end.line);

    QTextBlock errorBlock = m_programTextEdit->document()->findBlockByLineNumber(begin.line);
    assert(errorBlock.isValid());
    const int nNewLineCharacters = 1;
    const int minSelectionLength = 1;
    const int selectionEnd = qMin(end.column, errorBlock.length() - nNewLineCharacters);
    const int selectionStart = qMin(begin.column, selectionEnd - minSelectionLength);
    const int selectionLength = selectionEnd - selectionStart;
    QTextLayout::FormatRange errorRange;
    errorRange.start = selectionStart;
    errorRange.length = selectionLength;
    errorRange.format = m_errorFormat;
    errorBlock.layout()->setAdditionalFormats(errorBlock.layout()->additionalFormats() << errorRange);
    setTextCursor(begin);

    m_logModel->clear();
    m_logModel->addLine(LogDataModel::Error, message, begin);
    if (!callStack.empty()) {
        for (auto it = callStack.rbegin(); it != callStack.rend(); ++it) {
            QString text = (it == callStack.rbegin())
                           ? (it->routineIndex == mainRoutineIndex ? "В главной программе"            : QString("В подпрограмме %1").arg(it->routineIndex))
                           : (it->routineIndex == mainRoutineIndex ? "вызванной из главной программы" : QString("вызванной из подпрограммы %1").arg(it->routineIndex));
            m_logModel->addLine(LogDataModel::Neutral, text, it->inputPosition);
        }
    }
    showLog();

    m_programTextEdit->setFocus();
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

    int res = QMessageBox::warning(this, titleText("Сохранить?"), "Файл содержит несохранённые изменения. Сохранить?",
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
        setWindowTitle(titleText());
    else {
        QString modificationMarker = m_programTextEdit->document()->isModified() ? "*" : "";
        setWindowTitle(titleText(QString("%1%2").arg(m_fileName, modificationMarker)));
    }
}

void MainWindow::showLog()
{
    const int maxLogHeight = m_logView->fontMetrics().height() * 10;
    const int outerMargin = 6;  // TODO
    const int contentHeight = m_logView->sizeHintForRow(0) * m_logModel->rowCount();
    const int scrollBarHeight = m_logView->horizontalScrollBar()->sizeHint().height();
    const int desiredHeight = contentHeight + outerMargin + scrollBarHeight;
    const int viewHeight = qMin(desiredHeight, maxLogHeight);
    m_logView->setFixedHeight(viewHeight);

    m_logModel->setIconSize(m_logView->fontMetrics().height());

    m_logView->show();
}

void MainWindow::hideLog()
{
    m_logView->hide();
}

void MainWindow::updateOnLogItemClicked(const QModelIndex& idx)
{
    setTextCursor(idx.data(LogDataModel::SourceTextPositionRole).value<TextPosition>());
    m_programTextEdit->setFocus();
}

void MainWindow::setTextCursor(TextPosition position)
{
    QTextBlock block = m_programTextEdit->document()->findBlockByLineNumber(position.line);
    QTextCursor newCursor(block);
    newCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position.column);
    m_programTextEdit->setTextCursor(newCursor);
}

void MainWindow::updateCurrentLineHighlighting()
{
    QTextEdit::ExtraSelection selection;
    selection.format = m_currentLineFormat;
    selection.cursor = m_programTextEdit->textCursor();
    selection.cursor.clearSelection();
    m_programTextEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>() << selection);
}

void MainWindow::clearAdditionalFormats()
{
    QTextDocument* document = m_programTextEdit->document();
    for (QTextBlock block = document->begin(); block != document->end(); block = block.next())
        block.layout()->clearAdditionalFormats();
    updateCurrentLineHighlighting();
}

bool MainWindow::newDocument()
{
    if (!confirmClose())
        return false;
    m_programTextEdit->clear();
    m_fileName.clear();
    updateOnDocumentChanged();
    return true;
}

bool MainWindow::openDocument()
{
    if (!confirmClose())
        return false;

    QString newFileName = QFileDialog::getOpenFileName(this, titleText("Открыть файл"), QString(), "Текстовые файлы (*.txt);;Все файлы (*)");
    if (newFileName.isEmpty())
        return false;

    QFile file(newFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, titleErrorText(), QString("Не удалось открыть файл: \"%1\"").arg(newFileName));
        return false;
    }

    m_programTextEdit->setPlainText(file.readAll());
    m_fileName = newFileName;
    updateOnDocumentChanged();
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
    QString newFileName = QFileDialog::getSaveFileName(this, titleText("Сохранить файл"), QString(), "Текстовые файлы (*.txt);;Все файлы (*)");
    if (newFileName.isEmpty())
        return false;
    return doSaveDocument(newFileName);
}

bool MainWindow::doSaveDocument(const QString& target)
{
    QFile file(target);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, titleErrorText(), QString("Не удалось открыть на запись файл: \"%1\"").arg(target));
        return false;
    }

    file.write(m_programTextEdit->toPlainText().toUtf8());
    m_programTextEdit->document()->clearUndoRedoStacks();
    m_programTextEdit->document()->setModified(false);
    m_fileName = target;
    updateWindowTitle();
    return true;
}

void MainWindow::updateOnDocumentChanged()
{
    hideLog();
    setBlueprint(nullptr);
    updateWindowTitle();
}

void MainWindow::convert()
{
    try {
        m_blueprintView->setBlueprint(nullptr);
        QStringList programLines = m_programTextEdit->toPlainText().split('\n');
        ProgramParser parser;
        for (const QString& line : programLines)
            parser.processLine(line);

        std::unique_ptr<Program> program = parser.finish();
        setBlueprint(program->execute());
        QString output = blueprintToAutocadCommandLineCommands(m_blueprint.get());

        QApplication::clipboard()->setText(output);
        m_logModel->clear();
        hideLog();
        statusBar()->showMessage("Конвертация прошла успешно.\n(Результат скопирован в буфер обмена)", statusMessageDuration);
    }
    catch (const ParseError& error) {
        showProgramError(TextRange(error.position(), TextPosition()), error.what(), CallStack());
    }
    catch (const ExecutionError& error) {
        showProgramError(error.commandTextRange(), error.what(), error.callStack());
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, titleErrorText(), error.what());
    }
}

void MainWindow::saveImage()
{
    // TODO: save last settings accross launches
    if (!m_blueprint)  // TODO: Disable when there is no blueprint
        return;
    SaveImageDialog dialog(m_blueprint->boundingRect().size(), this);
    int ret = dialog.exec();
    if (ret == QDialog::Accepted) {
        SaveImageSettings imageSettings = dialog.settings();
        imageSettings.setUnits(SaveImageSettings::Pixels);
        // TODO: +QSvgGenerator
        QImage outputImage(imageSettings.size().toSize(), QImage::Format_ARGB32);
        outputImage.fill(Qt::white);
        m_blueprintView->renderBlueprint(&outputImage, outputImage.rect());
        bool ok = outputImage.save(imageSettings.outputFile());
        if (ok)
            statusBar()->showMessage(QString("Файл \"%1\" сохранён успешно").arg(imageSettings.outputFile()), statusMessageDuration);
        else
            QMessageBox::critical(this, titleErrorText(), QString("Не удалось сохранить файл \"%1\"").arg(imageSettings.outputFile()));
    }
}

void MainWindow::printImage()
{
    if (!m_blueprint)  // TODO: Disable when there is no blueprint
        return;
    QPrintPreviewDialog dialog(this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), m_blueprintView, SLOT(renderBlueprint(QPrinter*)));
    dialog.exec();
}
