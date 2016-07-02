#ifndef PROGRAMTEXTEDIT_H
#define PROGRAMTEXTEDIT_H

#include <QPlainTextEdit>
#include <QTextBlock>

#include "textbasic.h"

class SyntaxHighlighter;


class ProgramTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    typedef QPlainTextEdit ParentT;

public:
    explicit ProgramTextEdit(QWidget* parentArg = 0);

    LineRange selectedLines() const;

    void replaceLines(LineRange linesRange, QString newText);

    void indicateError(TextRange range);

    using ParentT::setTextCursor;

public slots:
    void setTextCursor(TextPosition position);

signals:
    void selectedLinesChanged(int first, int last);

private:
    SyntaxHighlighter* m_syntaxHighlighter = nullptr;
    bool m_hasUnparsedModifications;
    QTextCharFormat m_currentLineFormat;
    QTextBlock m_lastCursorBlock;

    TextRange fixErrorRange(TextRange range) const;

private:
    int positionToLine(int position) const;

private slots:
    void setError(TextRange range);
    void parseProgram();
    void updateCurrentLineHighlighting();
    void updateOnContentsChanged();
    void updateOnCursorPositionChanged();
    void updateSelectedLines();
};

#endif // PROGRAMTEXTEDIT_H
