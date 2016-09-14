#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QTextFormat>

#include "blueprintbasic.h"
#include "logdatamodel.h"
#include "textbasic.h"

class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
class QModelIndex;

class Blueprint;
class BlueprintView;
class CallStack;
class ProgramTextEdit;
class SearchBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parentArg = nullptr);
    ~MainWindow();

private:
    QString m_fileName;
    QStringList m_recentFilesNames;
    BlueprintPtr m_blueprint;
    LogDataModel* m_logModel = nullptr;

    ProgramTextEdit* m_programTextEdit = nullptr;
    SearchBar* m_searchBar = nullptr;
    BlueprintView* m_blueprintView = nullptr;
    QListView* m_logView = nullptr;
    QLabel* m_selectionCoordinatesWidget = nullptr;
    QLabel* m_stopCoordinatesWidget = nullptr;

    QMenu* m_fileMenu = nullptr;
    QMenu* m_editMenu = nullptr;
    QMenu* m_viewMenu = nullptr;
    QMenu* m_developmentMenu = nullptr;
    QMenu* m_helpMenu = nullptr;

    QAction* m_newAction = nullptr;
    QAction* m_openAction = nullptr;
    QList<QAction*> m_openRecentActions;
    QAction* m_openRecentEndAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_saveAsAction = nullptr;
    QAction* m_exitAction = nullptr;
    QAction* m_cutAction = nullptr;
    QAction* m_copyAction = nullptr;
    QAction* m_pasteAction = nullptr;
    QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;
    QAction* m_findAction = nullptr;
    QAction* m_drawAction = nullptr;
    QAction* m_drawAndConvertAction = nullptr;
    QAction* m_increaseFontSizeAction = nullptr;
    QAction* m_decreaseFontSizeAction = nullptr;
    QAction* m_flipHorizontallyAction = nullptr;
    QAction* m_flipVerticallyAction = nullptr;
    QAction* m_showTransitionsAction = nullptr;
    QAction* m_showSegmentsHighlightAction = nullptr;
    QAction* m_saveImageAction = nullptr;
    QAction* m_printImageAction = nullptr;
    QAction* m_showAboutAction = nullptr;

protected:
    void closeEvent(QCloseEvent* ev);

private:
    void showProgramProblem(LogDataModel::Severity severity, TextRange range, const QString& message, const CallStack& callStack);
    void addBacktraceToLog(const CallStack& callStack);
    void setBlueprint(std::unique_ptr<Blueprint> newBlueprint);
    bool confirmClose();

private slots:
    void loadSettings();
    void saveSettings();
    void updateWindowTitle();
    void setBlueprintActionsEnabled(bool enabled);
    void updateOnProgramTextChanged();
    void updateOnSelectedLinesChanged(int first, int last);
    void updateLogHeight();
    void showLog();
    void hideLog();
    void updateOnLogItemClicked(const QModelIndex& idx);
    bool newDocument();
    bool openDocument();
    bool openRecentDocument();
    bool doOpenDocument(const QString& fileName);
    bool saveDocument();
    bool saveDocumentAs();
    bool doSaveDocument(const QString& fileName);
    void updateOnDocumentChanged();
    void updateStopCoordinates();
    void showSegmentOrigin(SegmentId segmentId);
    void setFontSize(int pointSize);
    void increaseFontSize();
    void decreaseFontSize();
    void updateRecentFilesMenu();
    bool draw();
    bool drawAndConvert();
    void saveImage();
    void printImage();
    void showAbout();
};

#endif // MAINWINDOW_H
