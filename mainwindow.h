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

    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_convertAction;

private:
    void updateWindowTitle();

private slots:
    void open();
    void save();
    void convert();
};

#endif // MAINWINDOW_H
