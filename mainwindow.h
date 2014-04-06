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

    QPlainTextEdit* m_programTextEdit = nullptr;
    BlueprintView* m_blueprintView = nullptr;

    QAction* m_openAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_convertAction = nullptr;

private:
    void updateWindowTitle();

private slots:
    void open();
    void save();
    void convert();
};

#endif // MAINWINDOW_H
