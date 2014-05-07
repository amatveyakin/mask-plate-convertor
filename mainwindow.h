#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>

#include "textbasic.h"

class QLineEdit;
class QListView;
class QPlainTextEdit;
class QPushButton;
class QModelIndex;

class Blueprint;
class BlueprintView;
class CallStack;
class LogDataModel;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parentArg = nullptr);
    ~MainWindow();

private:
    QString m_fileName;
    std::unique_ptr<Blueprint> m_blueprint;
    LogDataModel* m_logModel;

    QPlainTextEdit* m_programTextEdit;
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
    void showLog();
    void hideLog();
    void updateOnLogItemClicked(const QModelIndex& idx);
    void setTextCursor(TextPosition position);
    void clearAdditionalFormats();
    bool newDocument();
    bool openDocument();
    bool saveDocument();
    bool saveDocumentAs();
    bool doSaveDocument(const QString& target);
    void convert();
    void saveImage();
    void printImage();
};

#endif // MAINWINDOW_H
