#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>

class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class Blueprint;
class BlueprintView;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parentArg = nullptr);
    ~MainWindow();

private:
    QString m_fileName;
    std::unique_ptr<Blueprint> m_blueprint;

    QPlainTextEdit* m_programTextEdit;
    BlueprintView* m_blueprintView;

    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_convertAction;

protected:
    void closeEvent(QCloseEvent* event);

private:
    bool confirmClose();

private slots:
    void updateWindowTitle();
    bool newDocument();
    bool openDocument();
    bool saveDocument();
    bool saveDocumentAs();
    bool doSaveDocument(const QString& target);
    void convert();
};

#endif // MAINWINDOW_H
