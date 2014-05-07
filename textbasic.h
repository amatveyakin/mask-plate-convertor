#ifndef TEXTBASIC_H
#define TEXTBASIC_H

#include <QMetaType>


struct TextPosition
{
    TextPosition() : line(-1), column(0) {}
    TextPosition(int lineArg, int columnArg) : line(lineArg), column(columnArg) {}

    bool valid() const  { return line >= 0 && column >= 0; }

    int line;
    int column;
};

struct TextRange
{
    TextRange() {}
    TextRange(TextPosition beginArg, TextPosition endArg) : begin(beginArg), end(endArg) {}

    TextPosition begin;
    TextPosition end;
};


Q_DECLARE_METATYPE(TextPosition)
Q_DECLARE_METATYPE(TextRange)

#endif // TEXTBASIC_H
