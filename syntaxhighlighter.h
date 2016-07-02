#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "textbasic.h"


class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument* parent);

public slots:
    void setError(TextRange range);
    void clearError();

protected:
    virtual void highlightBlock(const QString& line) override;

private:
    QTextCharFormat m_lineNumberFormat;
    QTextCharFormat m_routineDeclarationFormat;
    QTextCharFormat m_routineCallFormat;
    QTextCharFormat m_errorFormat;

    TextRange m_errorRange;

    int highlightAllOccurrences(const QString& line, const QString& pattern, QTextCharFormat format);
    void highlightError();
};

#endif // SYNTAXHIGHLIGHTER_H
