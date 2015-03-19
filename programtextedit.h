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

    LineRange selectedLines() const;

    void replaceLines(LineRange linesRange, QString newText);

    void indicateError(TextRange range);

    using ParentT::setTextCursor;

public slots:
    void setTextCursor(TextPosition position);

signals:
    void selectedLinesChanged(int first, int last);

private:
    QTextCharFormat m_currentLineFormat;
    QTextCharFormat m_errorFormat;

private:
    int positionToLine(int position) const;

private slots:
    void updateCurrentLineHighlighting();
    void clearAdditionalFormats();
    void updateSelectedLines();
};

#endif // PROGRAMTEXTEDIT_H
