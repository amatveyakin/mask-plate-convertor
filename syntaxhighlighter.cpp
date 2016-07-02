#include <assert.h>

#include <QFont>

#include "syntaxhighlighter.h"


SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    m_lineNumberFormat.setForeground(Qt::gray);

    m_routineDeclarationFormat.setForeground(Qt::blue);
    m_routineDeclarationFormat.setFontWeight(QFont::Bold);

    m_routineCallFormat.setForeground(Qt::blue);

    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    m_errorFormat.setBackground(QColor::fromRgbF(0.5, 0.0, 0.0, 0.10));
}

void SyntaxHighlighter::setError(TextRange range)
{
    m_errorRange = range;
    rehighlight();
}

void SyntaxHighlighter::clearError()
{
    setError(TextRange());
}

void SyntaxHighlighter::highlightBlock(const QString& line) {
    highlightAllOccurrences(line, "^N[0-9]+", m_lineNumberFormat);
    if (!highlightAllOccurrences(line, "^L[0-9]{2}$", m_routineDeclarationFormat))
        highlightAllOccurrences(line, "L[0-9]{2}", m_routineCallFormat);
    highlightError();
}

int SyntaxHighlighter::highlightAllOccurrences(const QString& line, const QString& pattern, QTextCharFormat format)
{
    QRegExp regexp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
    int index = line.indexOf(regexp);
    int numHighlighted = 0;
    while (index >= 0) {
        int length = regexp.matchedLength();
        setFormat(index, length, format);
        ++numHighlighted;
        index = line.indexOf(regexp, index + length);
    }
    return numHighlighted;
}

void SyntaxHighlighter::highlightError()
{
    if (m_errorRange.valid()) {
        assert(m_errorRange.begin.line == m_errorRange.end.line);
        if (currentBlock().firstLineNumber() == m_errorRange.begin.line)
            setFormat(m_errorRange.begin.column, m_errorRange.end.column - m_errorRange.begin.column, m_errorFormat);
    }
}
