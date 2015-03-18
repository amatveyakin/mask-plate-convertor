#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QTextFormat>

#include "blueprintbasic.h"
#include "textbasic.h"

class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
class QModelIndex;

class Blueprint;
class BlueprintView;
class CallStack;
class LogDataModel;
class ProgramTextEdit;


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
    LogDataModel* m_logModel;

    ProgramTextEdit* m_programTextEdit;
    BlueprintView* m_blueprintView;
    QListView* m_logView;
    QLabel* m_coordinatesWidget;

    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_developmentMenu;
    QMenu* m_helpMenu;

    QAction* m_newAction;
    QAction* m_openAction;
    QList<QAction*> m_openRecentActions;
    QAction* m_openRecentEndAction;
    QAction* m_saveAction;
    QAction* m_saveAsAction;
    QAction* m_exitAction;
    QAction* m_cutAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_drawAction;
    QAction* m_drawAndConvertAction;
    QAction* m_increaseFontSizeAction;
    QAction* m_decreaseFontSizeAction;
    QAction* m_flipHorizontallyAction;
    QAction* m_flipVerticallyAction;
    QAction* m_showTransitionsAction;
    QAction* m_saveImageAction;
    QAction* m_printImageAction;
    QAction* m_showAboutAction;

protected:
    void closeEvent(QCloseEvent* ev);

private:
    void showProgramError(TextRange range, const QString& message, const CallStack& callStack);
    void addBacktraceToLog(const CallStack& callStack);
    void setBlueprint(std::unique_ptr<Blueprint> newBlueprint);
    bool confirmClose();

private slots:
    void loadSettings();
    void saveSettings();
    void updateWindowTitle();
    void setBlueprintActionsEnabled(bool enabled);
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
