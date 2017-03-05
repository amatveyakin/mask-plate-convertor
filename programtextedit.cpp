#include "programtextedit.h"

#include <assert.h>

#include <QTextBlock>
#include <QTextLayout>

#include "programparser.h"
#include "program.h"
#include "syntaxhighlighter.h"


ProgramTextEdit::ProgramTextEdit(QWidget* parentArg)
    : ParentT(parentArg)
{
    m_currentLineFormat.setBackground(QColor::fromRgbF(0.2, 0.2, 0.6, 0.15));
    m_currentLineFormat.setProperty(QTextFormat::FullWidthSelection, true);

    m_hasUnparsedModifications = true;

    m_syntaxHighlighter = new SyntaxHighlighter(document());
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateSelectedLines()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(updateSelectedLines()));

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(updateOnContentsChanged()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateOnCursorPositionChanged()));

    updateOnCursorPositionChanged();
}

LineRange ProgramTextEdit::selectedLines() const
{
    LineRange range;
    if (textCursor().hasSelection()) {
        range.first = positionToLine(textCursor().selectionStart() + 1);
        range.last = positionToLine(textCursor().selectionEnd() - 1);
    }
    if (!range.isValid())
        range.first = range.last = positionToLine(textCursor().position());
    return range;
}

void ProgramTextEdit::replaceLines(LineRange linesRange, QString newText)
{
    QTextCursor cursor(document());
    auto firstBlock = document()->findBlockByLineNumber(linesRange.first);
    int firstChar = firstBlock.position();
    auto lastBlock = document()->findBlockByLineNumber(linesRange.last);
    int lastChar = lastBlock.position() + lastBlock.length();
    cursor.setPosition(firstChar);
    cursor.setPosition(lastChar, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(newText);
}

void ProgramTextEdit::indicateError(TextRange range)
{
    range = fixErrorRange(range);
    setError(range);
    setTextCursor(range.begin);
}

void ProgramTextEdit::setTextCursor(TextPosition position)
{
    QTextBlock block = document()->findBlockByLineNumber(position.line);
    QTextCursor newCursor(block);
    newCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position.column);
    setTextCursor(newCursor);
}

void ProgramTextEdit::find(const QString& text, QTextDocument::FindFlags options)
{
    if (document()->find(text, 0, options & ~QTextDocument::FindBackward).isNull()) {
        emit notFound();
        return;
    }
    bool found = ParentT::find(text, options);
    if (!found) {
        QTextCursor cursor(document());
        if (options & QTextDocument::FindBackward) {
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }
        else {
            cursor.movePosition(QTextCursor::Start);
            setTextCursor(cursor);
        }
        found = ParentT::find(text, options);
        assert(found);
    }
}

int ProgramTextEdit::positionToLine(int position) const
{
    return document()->findBlock(position).firstLineNumber();
}

TextRange ProgramTextEdit::fixErrorRange(TextRange range) const
{
    assert(range.begin.valid());
    if (!range.end.valid())
        range.end = range.begin;
    assert(range.begin.line == range.end.line);
    QTextBlock errorBlock = document()->findBlockByLineNumber(range.begin.line);
    assert(errorBlock.isValid());
    const int maxRangeEnd = errorBlock.length() - 1;  // ignore trailing newline
    range.end.column = qMin(range.end.column, maxRangeEnd);
    if (range.begin.column >= range.end.column) {
        if (range.end.column < maxRangeEnd) {
            range.begin.column = range.end.column;
            ++range.end.column;
        }
        else {
            range.begin.column = range.end.column - 1;
        }
    }
    return range;
}

void ProgramTextEdit::setError(TextRange range)
{
    disconnect(document(), SIGNAL(contentsChanged()), this, SLOT(updateOnContentsChanged()));
    m_syntaxHighlighter->setError(range);
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(updateOnContentsChanged()));
}

void ProgramTextEdit::parseProgram()
{
    try {
        ProgramParser::parseProgram(toPlainText());
    }
    catch (const ParseError& error) {
        setError(fixErrorRange(TextRange(error.position(), TextPosition())));
    }
}

void ProgramTextEdit::updateOnContentsChanged()
{
    setError(TextRange());
    updateCurrentLineHighlighting();
}

void ProgramTextEdit::updateOnCursorPositionChanged()
{
    if (m_lastCursorBlock != textCursor().block()) {
        parseProgram();
        m_lastCursorBlock = textCursor().block();
    }
    updateCurrentLineHighlighting();
}

void ProgramTextEdit::updateCurrentLineHighlighting()
{
    QTextEdit::ExtraSelection selection;
    selection.format = m_currentLineFormat;
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    setExtraSelections(QList<QTextEdit::ExtraSelection>() << selection);
}

void ProgramTextEdit::updateSelectedLines()
{
    LineRange range = selectedLines();
    emit selectedLinesChanged(range.first, range.last);
}
