#include "programtextedit.h"

#include <assert.h>

#include <QTextBlock>
#include <QTextLayout>


ProgramTextEdit::ProgramTextEdit(QWidget *parentArg)
    : ParentT(parentArg)
{
    m_currentLineFormat.setBackground(QColor::fromRgbF(0.2, 0.2, 0.6, 0.15));
    m_currentLineFormat.setProperty(QTextFormat::FullWidthSelection, true);

    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(clearAdditionalFormats()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateCurrentLineHighlighting()));

    updateCurrentLineHighlighting();
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
    QTextLayout::FormatRange errorRange;
    errorRange.start = selectionStart;
    errorRange.length = selectionLength;
    errorRange.format = m_errorFormat;
    errorBlock.layout()->setAdditionalFormats(errorBlock.layout()->additionalFormats() << errorRange);
    setTextCursor(range.begin);

}

void ProgramTextEdit::setTextCursor(TextPosition position)
{
    QTextBlock block = document()->findBlockByLineNumber(position.line);
    QTextCursor newCursor(block);
    newCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position.column);
    setTextCursor(newCursor);
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

