#include <QAction>
#include <QBoxLayout>
#include <QLineEdit>
#include <QToolButton>

#include "colorutils.h"
#include "searchbar.h"


SearchBar::SearchBar(QWidget* parent)
    : QWidget(parent)
{
    m_findForwardAction = new QAction(this);
    m_findForwardAction->setShortcutContext(Qt::WindowShortcut);
    m_findForwardAction->setShortcut(QKeySequence::FindNext);
    m_findForwardAction->setToolTip("Найти далее");
    m_findBackwardAction = new QAction(this);
    m_findBackwardAction->setShortcutContext(Qt::WindowShortcut);
    m_findBackwardAction->setShortcut(QKeySequence::FindPrevious);
    m_findBackwardAction->setToolTip("Найти ранее");
    m_closeAction = new QAction(this);
    m_closeAction->setShortcutContext(Qt::WindowShortcut);
    m_closeAction->setShortcut(Qt::Key_Escape);
    m_closeAction->setToolTip("Скрыть строку поиска");

    m_searchTextLineEdit = new QLineEdit(this);
    m_searchTextLineEdit->setPlaceholderText("Искать в тексте программы");

    m_findForwardButton = new QToolButton(this);
    m_findForwardButton->setDefaultAction(m_findForwardAction);
    m_findForwardButton->setText("▼");
    m_findBackwardButton = new QToolButton(this);
    m_findBackwardButton->setDefaultAction(m_findBackwardAction);
    m_findBackwardButton->setText("▲");
    m_closeButton = new QToolButton(this);
    m_closeButton->setDefaultAction(m_closeAction);
    m_closeButton->setText("❌");

    QBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_searchTextLineEdit);
    mainLayout->addWidget(m_findForwardButton);
    mainLayout->addWidget(m_findBackwardButton);
    mainLayout->addWidget(m_closeButton);

    connect(m_searchTextLineEdit, SIGNAL(textChanged(QString)), this, SLOT(clearNotFoundIndication()));

    connect(m_searchTextLineEdit, SIGNAL(returnPressed()), this, SLOT(emitFindForward()));
    connect(m_findForwardAction,  SIGNAL(triggered(bool)), this, SLOT(emitFindForward()));
    connect(m_findBackwardAction, SIGNAL(triggered(bool)), this, SLOT(emitFindBackward()));

    connect(m_closeAction, SIGNAL(triggered(bool)), this, SLOT(hide()));
}

void SearchBar::startSearch()
{
    clearNotFoundIndication();
    show();
    m_searchTextLineEdit->selectAll();
    m_searchTextLineEdit->setFocus();
}


void SearchBar::indicateNotFound()
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, mixColors(pal.color(QPalette::Base), Qt::red, 0.7));
    m_searchTextLineEdit->setPalette(pal);
}

void SearchBar::clearNotFoundIndication()
{
    m_searchTextLineEdit->setPalette(palette());
}

void SearchBar::emitFindForward()
{
    clearNotFoundIndication();
    emit find(m_searchTextLineEdit->text(), 0);
}

void SearchBar::emitFindBackward()
{
    clearNotFoundIndication();
    emit find(m_searchTextLineEdit->text(), QTextDocument::FindBackward);
}
