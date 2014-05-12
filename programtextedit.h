#ifndef PROGRAMTEXTEDIT_H
#define PROGRAMTEXTEDIT_H

#include <QPlainTextEdit>

#include "textbasic.h"


class ProgramTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    typedef QPlainTextEdit ParentT;

public:
    explicit ProgramTextEdit(QWidget *parentArg = 0);

    void indicateError(TextRange range);

    using ParentT::setTextCursor;

public slots:
    void setTextCursor(TextPosition position);

private:
    QTextCharFormat m_currentLineFormat;
    QTextCharFormat m_errorFormat;

private slots:
    void updateCurrentLineHighlighting();
    void clearAdditionalFormats();
};

#endif // PROGRAMTEXTEDIT_H
