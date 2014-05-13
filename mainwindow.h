#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QTextFormat>

#include "blueprint.h"
#include "textbasic.h"

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
    BlueprintPtr m_blueprint;
    LogDataModel* m_logModel;

    ProgramTextEdit* m_programTextEdit;
    BlueprintView* m_blueprintView;
    QListView* m_logView;

    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_convertAction;
    QAction* m_flipHorizontallyAction;
    QAction* m_flipVerticallyAction;
    QAction* m_saveImageAction;
    QAction* m_printImageAction;

protected:
    void closeEvent(QCloseEvent* ev);

private:
    void showProgramError(TextRange range, const QString& message, const CallStack& callStack);
    void setBlueprint(std::unique_ptr<Blueprint> newBlueprint);
    bool confirmClose();

private slots:
    void updateWindowTitle();
    void setBlueprintActionsEnabled(bool enabled);
    void showLog();
    void hideLog();
    void updateOnLogItemClicked(const QModelIndex& idx);
    bool newDocument();
    bool openDocument();
    bool saveDocument();
    bool saveDocumentAs();
    bool doSaveDocument(const QString& target);
    void updateOnDocumentChanged();
    void convert();
    void saveImage();
    void printImage();
};

#endif // MAINWINDOW_H
