#include "programtextedit.h"

#include <assert.h>

#include <QTextBlock>
#include <QTextLayout>


ProgramTextEdit::ProgramTextEdit(QWidget* parentArg)
    : ParentT(parentArg)
{
    m_currentLineFormat.setBackground(QColor::fromRgbF(0.2, 0.2, 0.6, 0.15));
    m_currentLineFormat.setProperty(QTextFormat::FullWidthSelection, true);

    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(clearAdditionalFormats()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateCurrentLineHighlighting()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateSelectedLines()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(updateSelectedLines()));

    updateCurrentLineHighlighting();
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
    QTextBlock errorBlock = document()->findBlockByLineNumber(range.begin.line);
    assert(errorBlock.isValid());
    const int nNewLineCharacters = 1;
    const int minSelectionLength = 1;
    const int selectionEnd = qMin(range.end.column, errorBlock.length() - nNewLineCharacters);
    const int selectionStart = qMin(range.begin.column, selectionEnd - minSelectionLength);
    const int selectionLength = selectionEnd - selectionStart;
    errorBlock.layout()->setAdditionalFormats({ { selectionStart, selectionLength, m_errorFormat } });
    setTextCursor(range.begin);

}

void ProgramTextEdit::setTextCursor(TextPosition position)
{
    QTextBlock block = document()->findBlockByLineNumber(position.line);
    QTextCursor newCursor(block);
    newCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position.column);
    setTextCursor(newCursor);
}

int ProgramTextEdit::positionToLine(int position) const
{
    return document()->findBlock(position).firstLineNumber();
}

void ProgramTextEdit::updateCurrentLineHighlighting()
{
    QTextEdit::ExtraSelection selection;
    selection.format = m_currentLineFormat;
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    setExtraSelections(QList<QTextEdit::ExtraSelection>() << selection);
}

void ProgramTextEdit::clearAdditionalFormats()
{
    for (QTextBlock block = document()->begin(); block != document()->end(); block = block.next())
        block.layout()->clearAdditionalFormats();
    updateCurrentLineHighlighting();
}

void ProgramTextEdit::updateSelectedLines()
{
    LineRange range = selectedLines();
    emit selectedLinesChanged(range.first, range.last);
}
