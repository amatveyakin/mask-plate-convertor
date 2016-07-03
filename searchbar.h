#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QTextDocument>
#include <QWidget>

class QLineEdit;
class QToolButton;


class SearchBar : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBar(QWidget* parent = nullptr);

public slots:
    void startSearch();
    void indicateNotFound();
    void clearNotFoundIndication();

signals:
    void find(QString text, QTextDocument::FindFlags options);

private:
    QLineEdit* m_searchTextLineEdit = nullptr;
    QToolButton* m_findForwardButton = nullptr;
    QToolButton* m_findBackwardButton = nullptr;
    QToolButton* m_closeButton = nullptr;

    QAction* m_findForwardAction = nullptr;
    QAction* m_findBackwardAction = nullptr;
    QAction* m_closeAction = nullptr;

private slots:
    void emitFindForward();
    void emitFindBackward();
};

#endif // SEARCHBAR_H
