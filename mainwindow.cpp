// TODO: blink status bar background when showing message
// TODO: Step-by-step drawing
// TODO: Save/load all settings

#include <exception>

#include <QAbstractItemModel>
#include <QAction>
#include <QActionGroup>
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
#include "autocadlanguage.h"
#include "blueprint.h"
#include "blueprintview.h"
#include "logdatamodel.h"
#include "mainwindow.h"
#include "program.h"
#include "programparser.h"
#include "programtextedit.h"
#include "runningprogram.h"
#include "saveimagedialog.h"
#include "searchbar.h"
#include "utils.h"


static const int maxRecentDocuments = 10;
static const int statusMessageDuration = 5000;  // ms

static QString elideFilename(QString filename)
{
    const int maxLength = 100;
    const int maxLengthWithoutEllipsis = maxLength - 3;
    return (filename.length() <= maxLength) ? filename : "..." + filename.right(maxLengthWithoutEllipsis);
}

static QString pointToString(QPoint p)
{
    return QString("(X: %1, Y: %2)").arg(prettyPrintNumber(p.x()), prettyPrintNumber(p.y()));
}


MainWindow::MainWindow(QWidget* parentArg)
    : QMainWindow(parentArg)
{
    setWindowIcon(QIcon(":/images/application-icon.png"));
    setCentralWidget(new QWidget(this));
    statusBar();  // show status bar
    initAutocadLanguages();

    m_logModel = new LogDataModel(this);

    m_programTextEdit = new ProgramTextEdit(this);
    m_programTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_searchBar = new SearchBar(this);
    m_searchBar->hide();

    m_blueprintView = new BlueprintView(this);

    m_logView = new QListView(this);
    m_logView->setModel(m_logModel);
    m_logView->hide();

    QFrame* logSeparator = new QFrame();
    logSeparator->setFrameShape(QFrame::VLine);
    logSeparator->setFrameShadow(QFrame::Sunken);
    m_selectionCoordinatesWidget = new QLabel(this);

    QLabel* m_stopCoordinatesIconWidget = new QLabel(this);
    QPixmap stopCoordinatesPixmap = QPixmap(":/images/misc-finish.png").scaledToHeight(statusBar()->height(), Qt::SmoothTransformation);
    m_stopCoordinatesIconWidget->setPixmap(stopCoordinatesPixmap);
    m_stopCoordinatesIconWidget->setMaximumHeight(statusBar()->height());

    m_stopCoordinatesWidget = new QLabel(this);
    QFont stopCoordinatesFont = m_stopCoordinatesWidget->font();
    stopCoordinatesFont.setItalic(true);
    m_stopCoordinatesWidget->setFont(stopCoordinatesFont);
    m_stopCoordinatesWidget->setToolTip("Координаты конца рисунка");

    statusBar()->addPermanentWidget(m_selectionCoordinatesWidget);
    statusBar()->addPermanentWidget(logSeparator);
    statusBar()->addPermanentWidget(m_stopCoordinatesIconWidget);
    statusBar()->addPermanentWidget(m_stopCoordinatesWidget);

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
    m_findAction = new QAction(QIcon(":/images/edit-find.png"), "&Найти", this);
    m_drawAction = new QAction(QIcon(":/images/development-draw.png"), "&Нарисовать", this);
    m_drawAndConvertAction = new QAction(QIcon(":/images/development-convert.png"), "&Конвертировать", this);
    m_increaseFontSizeAction = new QAction(QIcon(":/images/view-increase-font.png"), "&Увеличить размер шрифта", this);
    m_decreaseFontSizeAction = new QAction(QIcon(":/images/view-decrease-font.png"), "У&меньшить размер шрифта", this);
    m_flipHorizontallyAction = new QAction(QIcon(":/images/view-flip-horizontally.png"), "Отразить по &горизонтали", this);
    m_flipVerticallyAction = new QAction(QIcon(":/images/view-flip-vertically.png"), "Отразить по &вертикали", this);
    m_showTransitionsAction = new QAction(QIcon(":/images/view-show-transitions.png"), "Показывать &переходы", this);
    m_showSegmentsHighlightAction = new QAction(QIcon(":/images/view-show-highlight.png"), "Подсвечивать &выделенное", this);
    m_saveImageAction = new QAction(QIcon(":/images/image-save.png"), "Сохранить &изображение...", this);
    m_printImageAction = new QAction(QIcon(":/images/image-print.png"), "&Распечатать изображение...", this);
    m_showAboutAction = new QAction(QIcon(":/images/help-about.png"), "&О программе...", this);

    m_flipHorizontallyAction->setCheckable(true);
    m_flipVerticallyAction->setCheckable(true);
    m_showTransitionsAction->setCheckable(true);
    m_showSegmentsHighlightAction->setCheckable(true);

    m_newAction->setShortcut(QKeySequence::New);
    m_openAction->setShortcut(QKeySequence::Open);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_findAction->setShortcut(QKeySequence::Find);
    m_drawAction->setShortcut(Qt::CTRL | Qt::Key_Return);
    m_increaseFontSizeAction->setShortcut(Qt::ALT | Qt::Key_Plus);
    m_decreaseFontSizeAction->setShortcut(Qt::ALT | Qt::Key_Minus);
    m_printImageAction->setShortcut(QKeySequence::Print);

    m_fileMenu = menuBar()->addMenu("&Файл");
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addSeparator();
    m_openRecentEndAction = m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveImageAction);
    m_fileMenu->addAction(m_printImageAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_editMenu = menuBar()->addMenu("&Правка");
    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_redoAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_findAction);

    m_viewMenu = menuBar()->addMenu("&Вид");
    m_viewMenu->addAction(m_increaseFontSizeAction);
    m_viewMenu->addAction(m_decreaseFontSizeAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_flipHorizontallyAction);
    m_viewMenu->addAction(m_flipVerticallyAction);
    m_viewMenu->addAction(m_showTransitionsAction);
    m_viewMenu->addAction(m_showSegmentsHighlightAction);

    m_developmentMenu = menuBar()->addMenu("&Разработка");
    m_developmentMenu->addAction(m_drawAction);
    m_developmentMenu->addAction(m_drawAndConvertAction);
    m_developmentMenu->addSeparator();
    m_autocadLanguageSubmenu = m_developmentMenu->addMenu("&Язык команд AutoCAD");
    m_autocadLanguageSubmenu->addActions(m_autocadLanguageActionGroup->actions());

    m_helpMenu = menuBar()->addMenu("П&омощь");
    m_helpMenu->addAction(m_showAboutAction);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addAction(m_newAction);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_saveAction);
    toolbar->addSeparator();
    toolbar->addAction(m_cutAction);
    toolbar->addAction(m_copyAction);
    toolbar->addAction(m_pasteAction);
    toolbar->addSeparator();
    toolbar->addAction(m_undoAction);
    toolbar->addAction(m_redoAction);
    toolbar->addSeparator();
    toolbar->addAction(m_findAction);
    toolbar->addSeparator();
    toolbar->addAction(m_drawAction);
    toolbar->addSeparator();
    toolbar->addAction(m_flipHorizontallyAction);
    toolbar->addAction(m_flipVerticallyAction);
    toolbar->addAction(m_showTransitionsAction);
    toolbar->addAction(m_showSegmentsHighlightAction);
    toolbar->addSeparator();
    toolbar->addAction(m_saveImageAction);
    toolbar->addAction(m_printImageAction);
    addToolBar(toolbar);

    QWidget* editorWidget = new QWidget(this);

    QBoxLayout* editorLayout = new QVBoxLayout(editorWidget);
    editorLayout->setContentsMargins(0, 0, 2 /* TODO: some value from pixelmetrics */, 0);
    editorLayout->addWidget(m_programTextEdit);
    editorLayout->addWidget(m_searchBar);
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

    connect(m_findAction, SIGNAL(triggered()), m_searchBar, SLOT(startSearch()));

    connect(m_searchBar,     SIGNAL(find(QString, QTextDocument::FindFlags)),
            m_programTextEdit, SLOT(find(QString, QTextDocument::FindFlags)));

    connect(m_programTextEdit, SIGNAL(notFound()), m_searchBar, SLOT(indicateNotFound()));

    connect(document, SIGNAL(modificationChanged(bool)), this, SLOT(updateWindowTitle()));

    connect(m_programTextEdit, SIGNAL(textChanged()),                  this, SLOT(updateOnProgramTextChanged()));
    connect(m_programTextEdit, SIGNAL(selectedLinesChanged(int, int)), this, SLOT(updateOnSelectedLinesChanged(int, int)));

    connect(m_blueprintView, SIGNAL(selectedSegmentChanged(SegmentId)), this, SLOT(showSegmentOrigin(SegmentId)));

    connect(m_drawAction,           SIGNAL(triggered()), this, SLOT(draw()));
    connect(m_drawAndConvertAction, SIGNAL(triggered()), this, SLOT(drawAndConvert()));
    connect(m_saveImageAction,      SIGNAL(triggered()), this, SLOT(saveImage()));
    connect(m_printImageAction,     SIGNAL(triggered()), this, SLOT(printImage()));

    connect(m_increaseFontSizeAction, SIGNAL(triggered()), this, SLOT(increaseFontSize()));
    connect(m_decreaseFontSizeAction, SIGNAL(triggered()), this, SLOT(decreaseFontSize()));

    connect(m_flipHorizontallyAction,       SIGNAL(toggled(bool)), m_blueprintView, SLOT(setFlipHorizontally(bool)));
    connect(m_flipVerticallyAction,         SIGNAL(toggled(bool)), m_blueprintView, SLOT(setFlipVertically(bool)));
    connect(m_showTransitionsAction,        SIGNAL(toggled(bool)), m_blueprintView, SLOT(setShowTransitions(bool)));
    connect(m_showSegmentsHighlightAction,  SIGNAL(toggled(bool)), m_blueprintView, SLOT(setShowSegmentsHighlight(bool)));

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

QString MainWindow::autocadLanguageCodeName() const
{
    const QAction* checkedAction = m_autocadLanguageActionGroup->checkedAction();
    assert(checkedAction);
    return checkedAction->data().toString();
}

AutocadConvertorSettings MainWindow::autocadConvertorSettings() const
{
    AutocadConvertorSettings settings;
    settings.autocadLanguageCodeName = autocadLanguageCodeName();
    settings.sizeCoeff = 0.001;  // TODO: Make size coeff adjustable
    settings.closePolygons = true;  // TODO: Make this configurable
    return settings;
}

void MainWindow::showProgramProblem(LogDataModel::Severity severity, TextRange range, const QString& message, const CallStack& callStack)
{
    if (!range.end.valid())
        range.end = TextPosition(range.begin.line, std::numeric_limits<int>::max());
    assert(range.begin.line == range.end.line);

    m_stopCoordinatesWidget->clear();

    m_programTextEdit->indicateError(range);

    m_logModel->clear();
    m_logModel->addLine(severity, message, range.begin);
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

void MainWindow::initAutocadLanguages()
{
    m_autocadLanguageActionGroup = new QActionGroup(this);
    for (const auto& lang : AutocadLanguageFactory::singleton().allLanguages()) {
        QAction* action = new QAction(lang->uiName(), this);
        action->setCheckable(true);
        action->setData(lang->codeName());
        m_autocadLanguageActionGroup->addAction(action);
    }
}

bool MainWindow::setAutocadLanguage(QString codeName)
{
    for (QAction* action : m_autocadLanguageActionGroup->actions()) {
        if (action->data().toString() == codeName) {
            action->setChecked(true);
            return true;
        }
    }
    return false;
}

void MainWindow::loadSettings()
{
    QSettings settings;
    bool ok = false;

    int fontSize = settings.value("font_size").toInt(&ok);
    if (ok)
        setFontSize(fontSize);

    m_flipHorizontallyAction->setChecked(settings.value("flip_horizontally", false).toBool());
    m_flipVerticallyAction->setChecked(settings.value("flip_vertically", false).toBool());
    m_showTransitionsAction->setChecked(settings.value("show_transitions", false).toBool());
    m_showSegmentsHighlightAction->setChecked(settings.value("show_segments_highlight", true).toBool());
    if (!setAutocadLanguage(settings.value("autocad_command_language", "").toString()))
        setAutocadLanguage(AutocadLanguageFactory::singleton().defaultLanguage()->codeName());

    m_recentFilesNames.clear();
    int recentFilesCount = settings.beginReadArray("recent_documents");
    for (int i = 0; i < recentFilesCount; ++i) {
        settings.setArrayIndex(i);
        m_recentFilesNames.push_back(settings.value("name").toString());
    }
    settings.endArray();
    updateRecentFilesMenu();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("font_size", centralWidget()->fontInfo().pointSize());

    settings.setValue("flip_horizontally", m_flipHorizontallyAction->isChecked());
    settings.setValue("flip_vertically", m_flipVerticallyAction->isChecked());
    settings.setValue("show_transitions", m_showTransitionsAction->isChecked());
    settings.setValue("show_segments_highlight", m_showSegmentsHighlightAction->isChecked());
    settings.setValue("autocad_command_language", autocadLanguageCodeName());

    settings.beginWriteArray("recent_documents", m_recentFilesNames.size());
    for (int i = 0; i < m_recentFilesNames.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", m_recentFilesNames[i]);
    }
    settings.endArray();
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
    m_saveImageAction->setEnabled(enabled);
    m_printImageAction->setEnabled(enabled);
}

void MainWindow::updateOnProgramTextChanged()
{
    if (m_blueprint.isValid())
        m_blueprint.getMutable()->resetForwardMapping();
    const LineRange selectedLine = m_programTextEdit->selectedLines();
    updateOnSelectedLinesChanged(selectedLine.first, selectedLine.last);
}

void MainWindow::updateOnSelectedLinesChanged(int first, int last)
{
    const QString lineNumberString = stringWithInteger(last - first + 1, "%1 строке", "%1 строкaх", "%1 строкaх");
    bool ok = false;
    QPoint movement;
    m_blueprint->forwardMapping().lineIntervalMovement(first, last, ok, movement);
    if (ok)
        statusBar()->showMessage(QString("Σ перемещение в %1 = %2").arg(lineNumberString, pointToString(movement)));
    else
        statusBar()->clearMessage();
    m_blueprintView->setHighlight(m_blueprint->forwardMapping().lineIntervalSegments(first, last));
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
    m_logModel->clear();
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
    QString fileName = QFileDialog::getOpenFileName(this, titleText("Открыть файл"), QString(), "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty())
        return false;
    return doOpenDocument(fileName);
}

bool MainWindow::openRecentDocument()
{
    if (!confirmClose())
        return false;
    QAction* senderAction = qobject_cast<QAction*>(sender());
    assert(senderAction);
    QString fileName = senderAction->data().toString();
    return doOpenDocument(fileName);
}

bool MainWindow::doOpenDocument(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, titleErrorText(), QString("Не удалось открыть файл: \"%1\"").arg(fileName));
        return false;
    }

    m_programTextEdit->setPlainText(file.readAll());
    m_fileName = fileName;
    m_recentFilesNames.removeAll(fileName);
    m_recentFilesNames.push_front(fileName);
    m_recentFilesNames = m_recentFilesNames.mid(0, maxRecentDocuments);
    updateRecentFilesMenu();
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

bool MainWindow::doSaveDocument(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, titleErrorText(), QString("Не удалось открыть на запись файл: \"%1\"").arg(fileName));
        return false;
    }

    file.write(m_programTextEdit->toPlainText().toUtf8());
    m_programTextEdit->document()->clearUndoRedoStacks();
    m_programTextEdit->document()->setModified(false);
    m_fileName = fileName;
    updateWindowTitle();
    return true;
}

void MainWindow::updateOnDocumentChanged()
{
    hideLog();
    setBlueprint(nullptr);
    updateWindowTitle();
}

void MainWindow::updateStopCoordinates()
{
    if (m_blueprint.isValid())
        m_stopCoordinatesWidget->setText(pointToString(m_blueprint->stopPoint()));
    else
        m_stopCoordinatesWidget->clear();
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
        m_selectionCoordinatesWidget->setText(QString("%1 → %2, ширина %3").arg(pointToString(element.polygon[segmentId.segment]),
                                                                                pointToString(element.polygon[segmentId.segment + 1]),
                                                                                prettyPrintNumber(element.width)));   // TODO: Fix numbers screen positions
    }
    else {
        hideLog();
        m_selectionCoordinatesWidget->clear();
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

void MainWindow::updateRecentFilesMenu()
{
    qDeleteAll(m_openRecentActions);
    m_openRecentActions.clear();
    for (const QString& fileName : m_recentFilesNames) {
        QAction* action = new QAction(elideFilename(fileName), this);
        action->setData(fileName);
        m_openRecentActions.push_back(action);
        m_fileMenu->insertAction(m_openRecentEndAction, action);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentDocument()));
    }
}

bool MainWindow::draw()
{
    try {
        setBlueprint(nullptr);
        const std::unique_ptr<Program> program = ProgramParser::parseProgram(m_programTextEdit->toPlainText());
        ExecutionResult executionResult = program->execute();
        setBlueprint(std::move(executionResult.blueprint));

        const auto& executionWarning = executionResult.executionWarning;
        if (executionWarning)
            showProgramProblem(LogDataModel::Warning, executionWarning->commandTextRange(), executionWarning->what(), executionWarning->callStack());
        else
            hideLog();
        updateStopCoordinates();
        statusBar()->showMessage("Программа нарисована.", statusMessageDuration);
        return true;
    }
    catch (const ParseError& error) {
        showProgramProblem(LogDataModel::Error, TextRange(error.position(), TextPosition()), error.what(), CallStack());
    }
    catch (const ExecutionProblem& error) {
        showProgramProblem(LogDataModel::Error, error.commandTextRange(), error.what(), error.callStack());
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, titleErrorText(), error.what());
    }
    return false;
}

bool MainWindow::drawAndConvert()
{
    bool ok = draw();
    if (!ok)
        return false;
    AutocadConvertor convertor(autocadConvertorSettings());
    QString output = convertor.blueprintToAutocadCommandLineCommands(*m_blueprint);
    QApplication::clipboard()->setText(output);
    statusBar()->showMessage("Программа сконвертирована (результат в буфере обмена).", statusMessageDuration);
    return true;
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
