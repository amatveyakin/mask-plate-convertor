// TODO: blink status bar background when showing message
// TODO: Step-by-step drawing
// TODO: Add recent documents; save/load all settings

#include <exception>

#include <QAbstractItemModel>
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QFileDialog>
#include <QLabel>
#include <QListView>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintPreviewDialog>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>

#include "appinfo.h"
#include "autocadconvertor.h"
#include "blueprint.h"
#include "blueprintview.h"
#include "logdatamodel.h"
#include "mainwindow.h"
#include "program.h"
#include "programparser.h"
#include "programtextedit.h"
#include "runningprogram.h"
#include "saveimagedialog.h"


static const int statusMessageDuration = 5000;  // ms


MainWindow::MainWindow(QWidget* parentArg)
    : QMainWindow(parentArg)
{
    setWindowIcon(QIcon(":/images/application-icon.png"));
    setCentralWidget(new QWidget(this));
    statusBar();  // show status bar

    m_logModel = new LogDataModel(this);

    m_programTextEdit = new ProgramTextEdit(this);
    m_programTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_blueprintView = new BlueprintView(this);

    m_logView = new QListView(this);
    m_logView->setModel(m_logModel);
    m_logView->hide();

    m_coordinatesWidget = new QLabel(this);
    statusBar()->addPermanentWidget(m_coordinatesWidget);

    m_newAction = new QAction(QIcon(":/images/document-new.png"), "&Новый", this);
    m_openAction = new QAction(QIcon(":/images/document-open.png"), "&Открыть...", this);
    m_saveAction = new QAction(QIcon(":/images/document-save.png"), "&Сохранить", this);
    m_saveAsAction = new QAction(QIcon(":/images/document-save-as.png"), "Сохранить &как...", this);
    m_exitAction = new QAction(QIcon(":/images/application-exit.png"), "В&ыход", this);
    m_cutAction = new QAction(QIcon(":/images/edit-cut.png"), "Вы&резать", this);
    m_copyAction = new QAction(QIcon(":/images/edit-copy.png"), "&Копировать", this);
    m_pasteAction = new QAction(QIcon(":/images/edit-paste.png"), "&Вставить", this);
    m_undoAction = new QAction(QIcon(":/images/edit-undo.png"), "&Отменить", this);
    m_redoAction = new QAction(QIcon(":/images/edit-redo.png"), "&Повторить", this);
    m_drawAction = new QAction(QIcon(":/images/development-draw.png"), "&Нарисовать", this);
    m_drawAndConvertAction = new QAction(QIcon(":/images/development-convert.png"), "&Конвертировать", this);
    m_increaseFontSizeAction = new QAction(QIcon(":/images/view-increase-font.png"), "&Увеличить размер шрифта", this);
    m_decreaseFontSizeAction = new QAction(QIcon(":/images/view-decrease-font.png"), "У&меньшить размер шрифта", this);
    m_flipHorizontallyAction = new QAction(QIcon(":/images/view-flip-horizontally.png"), "Отразить по &горизонтали", this);
    m_flipVerticallyAction = new QAction(QIcon(":/images/view-flip-vertically.png"), "Отразить по &вертикали", this);
    m_showTransitionsAction = new QAction(QIcon(":/images/view-show-transitions.png"), "Показывать &переходы", this);
    m_saveImageAction = new QAction(QIcon(":/images/image-save.png"), "Сохранить &изображение...", this);
    m_printImageAction = new QAction(QIcon(":/images/image-print.png"), "&Распечатать изображение...", this);
    m_showAboutAction = new QAction(QIcon(":/images/help-about.png"), "&О программе...", this);

    m_flipHorizontallyAction->setCheckable(true);
    m_flipVerticallyAction->setCheckable(true);
    m_showTransitionsAction->setCheckable(true);

    m_newAction->setShortcut(QKeySequence::New);
    m_openAction->setShortcut(QKeySequence::Open);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_drawAction->setShortcut(Qt::CTRL | Qt::Key_Return);
    m_increaseFontSizeAction->setShortcut(Qt::ALT | Qt::Key_Plus);
    m_decreaseFontSizeAction->setShortcut(Qt::ALT | Qt::Key_Minus);
    m_printImageAction->setShortcut(QKeySequence::Print);

    QMenu* fileMenu = menuBar()->addMenu("&Файл");
    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_saveImageAction);
    fileMenu->addAction(m_printImageAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    QMenu* editMenu = menuBar()->addMenu("&Правка");
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addSeparator();
    editMenu->addAction(m_cutAction);
    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);

    QMenu* viewMenu = menuBar()->addMenu("&Вид");
    viewMenu->addAction(m_increaseFontSizeAction);
    viewMenu->addAction(m_decreaseFontSizeAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_flipHorizontallyAction);
    viewMenu->addAction(m_flipVerticallyAction);
    viewMenu->addAction(m_showTransitionsAction);

    QMenu* developmentMenu = menuBar()->addMenu("&Разработка");
    developmentMenu->addAction(m_drawAction);
    developmentMenu->addAction(m_drawAndConvertAction);

    QMenu* helpMenu = menuBar()->addMenu("П&омощь");
    helpMenu->addAction(m_showAboutAction);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addAction(m_newAction);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_saveAction);
    toolbar->addSeparator();
    toolbar->addAction(m_undoAction);
    toolbar->addAction(m_redoAction);
    toolbar->addSeparator();
    toolbar->addAction(m_drawAction);
    toolbar->addSeparator();
    toolbar->addAction(m_flipHorizontallyAction);
    toolbar->addAction(m_flipVerticallyAction);
    toolbar->addAction(m_showTransitionsAction);
    toolbar->addSeparator();
    toolbar->addAction(m_saveImageAction);
    toolbar->addAction(m_printImageAction);
    addToolBar(toolbar);

    QWidget* editorWidget = new QWidget(this);

    QBoxLayout* editorLayout = new QVBoxLayout(editorWidget);
    editorLayout->setContentsMargins(0, 0, 0, 0);
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

    setBlueprintActionsEnabled(m_blueprint.isValid());

    connect(&m_blueprint, SIGNAL(isValidChanged(bool)), this, SLOT(setBlueprintActionsEnabled(bool)));

    connect(m_newAction,    SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(m_openAction,   SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(m_saveAction,   SIGNAL(triggered()), this, SLOT(saveDocument()));
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));
    connect(m_exitAction,   SIGNAL(triggered()), this, SLOT(close()));

    connect(m_cutAction,    SIGNAL(triggered()), m_programTextEdit, SLOT(cut()));
    connect(m_copyAction,   SIGNAL(triggered()), m_programTextEdit, SLOT(copy()));
    connect(m_pasteAction,  SIGNAL(triggered()), m_programTextEdit, SLOT(paste()));

    connect(m_undoAction, SIGNAL(triggered()), document, SLOT(undo()));
    connect(m_redoAction, SIGNAL(triggered()), document, SLOT(redo()));
    connect(document, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)));
    connect(document, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)));

    connect(document, SIGNAL(modificationChanged(bool)), this, SLOT(updateWindowTitle()));

    connect(m_blueprintView, SIGNAL(selectedSegmentChanged(SegmentId)), this, SLOT(showSegmentOrigin(SegmentId)));

    connect(m_drawAction,           SIGNAL(triggered()), this, SLOT(draw()));
    connect(m_drawAndConvertAction, SIGNAL(triggered()), this, SLOT(drawAndConvert()));
    connect(m_saveImageAction,      SIGNAL(triggered()), this, SLOT(saveImage()));
    connect(m_printImageAction,     SIGNAL(triggered()), this, SLOT(printImage()));

    connect(m_increaseFontSizeAction, SIGNAL(triggered()), this, SLOT(increaseFontSize()));
    connect(m_decreaseFontSizeAction, SIGNAL(triggered()), this, SLOT(decreaseFontSize()));

    connect(m_flipHorizontallyAction, SIGNAL(toggled(bool)), m_blueprintView, SLOT(setFlipHorizontally(bool)));
    connect(m_flipVerticallyAction,   SIGNAL(toggled(bool)), m_blueprintView, SLOT(setFlipVertically(bool)));
    connect(m_showTransitionsAction,  SIGNAL(toggled(bool)), m_blueprintView, SLOT(setShowTransitions(bool)));

    connect(m_logView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateOnLogItemClicked(QModelIndex)));

    connect(m_showAboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    updateWindowTitle();
    loadSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    if (confirmClose()) {
        saveSettings();
        ev->accept();
    }
    else
        ev->ignore();
}

void MainWindow::showProgramError(TextRange range, const QString& message, const CallStack& callStack)
{
    if (!range.end.valid())
        range.end = TextPosition(range.begin.line, std::numeric_limits<int>::max());
    assert(range.begin.line == range.end.line);

    m_programTextEdit->indicateError(range);

    m_logModel->clear();
    m_logModel->addLine(LogDataModel::Error, message, range.begin);
    addBacktraceToLog(callStack);
    showLog();

    m_programTextEdit->setFocus();
}

void MainWindow::addBacktraceToLog(const CallStack& callStack)
{
    if (callStack.empty())
        return;
    for (auto it = callStack.rbegin(); it != callStack.rend(); ++it) {
        QString text = (it == callStack.rbegin())
                       ? (it->routineIndex == mainRoutineIndex ? "В главной программе"            : QString("В подпрограмме %1").arg(it->routineIndex))
                       : (it->routineIndex == mainRoutineIndex ? "вызванной из главной программы" : QString("вызванной из подпрограммы %1").arg(it->routineIndex));
        text += QString(" (строка %1)").arg(it->inputPosition.line);
        m_logModel->addLine(LogDataModel::Neutral, text, it->inputPosition);
    }
}

void MainWindow::setBlueprint(std::unique_ptr<Blueprint> newBlueprint)
{
    m_blueprint.reset(newBlueprint.release());
    m_blueprintView->setBlueprint(m_blueprint);
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

void MainWindow::loadSettings()
{
    QSettings settings;
    bool ok;
    int fontSize = settings.value("font_size").toInt(&ok);
    if (ok)
        setFontSize(fontSize);
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("font_size", centralWidget()->fontInfo().pointSize());
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

void MainWindow::setBlueprintActionsEnabled(bool enabled)
{
    m_flipHorizontallyAction->setEnabled(enabled);
    m_flipVerticallyAction->setEnabled(enabled);
    m_showTransitionsAction->setEnabled(enabled);
    m_saveImageAction->setEnabled(enabled);
    m_printImageAction->setEnabled(enabled);
}

void MainWindow::updateLogHeight()
{
    const int maxLogHeight = m_logView->fontMetrics().height() * 10;
    const int outerMargin = 6;  // TODO
    const int contentHeight = m_logView->sizeHintForRow(0) * m_logModel->rowCount();
    const int scrollBarHeight = m_logView->horizontalScrollBar()->sizeHint().height();
    const int desiredHeight = contentHeight + outerMargin + scrollBarHeight;
    const int viewHeight = qMin(desiredHeight, maxLogHeight);
    m_logView->setFixedHeight(viewHeight);
    m_logModel->setIconSize(m_logView->fontMetrics().height());
}

void MainWindow::showLog()
{
    updateLogHeight();
    m_logView->show();
}

void MainWindow::hideLog()
{
    m_logView->hide();
}

void MainWindow::updateOnLogItemClicked(const QModelIndex& idx)
{
    m_programTextEdit->setTextCursor(idx.data(LogDataModel::SourceTextPositionRole).value<TextPosition>());
    m_programTextEdit->setFocus();
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

static QString pointToString(QPoint p)
{
    return QString("(X: %1, Y: %2)").arg(QString::number(p.x()), QString::number(p.y()));
}

void MainWindow::showSegmentOrigin(SegmentId segmentId)
{
    if (m_blueprint->isSegmentValid(segmentId)) {
        m_logModel->clear();
        const Element& element = m_blueprint->elements()[segmentId.element];
        const CallStack& backtrace = element.segmentBacktraces[segmentId.segment];
        assert(!backtrace.empty());
        addBacktraceToLog(backtrace);
        showLog();
        m_programTextEdit->setTextCursor(backtrace.back().inputPosition);
        m_coordinatesWidget->setText(QString("%1 — %2, ширина %3").arg(pointToString(element.polygon[segmentId.segment]),
                                                                       pointToString(element.polygon[segmentId.segment + 1]),
                                                                       QString::number(element.width)));   // TODO: Fix numbers screen positions
    }
    else {
        hideLog();
        m_coordinatesWidget->clear();
    }
}

void MainWindow::setFontSize(int pointSize)
{
    QFont newFont = centralWidget()->font();
    newFont.setPointSize(pointSize);
    centralWidget()->setFont(newFont);
    updateLogHeight();
}

void MainWindow::increaseFontSize()
{
    setFontSize(centralWidget()->fontInfo().pointSize() + 1);
}

void MainWindow::decreaseFontSize()
{
    const int minPointSize = 4;
    setFontSize(qMax(minPointSize, centralWidget()->fontInfo().pointSize() - 1));
}

bool MainWindow::draw()
{
    try {
        setBlueprint(nullptr);
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
    assert(m_blueprint.isValid());
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
    assert(m_blueprint.isValid());
    QPrintPreviewDialog dialog(this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), m_blueprintView, SLOT(renderBlueprint(QPrinter*)));
    dialog.exec();
}

void MainWindow::showAbout()
{
  QMessageBox::about(this, QString("О программе %1").arg(appName()), aboutText());
}
