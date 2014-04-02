#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    QLineEdit* m_fileNameLineedit = 0;
    QPushButton* m_browseButton = 0;
    QPushButton* m_convertButton = 0;

private slots:
    void browse();
    void convert();
};

#endif // MAINWINDOW_H
