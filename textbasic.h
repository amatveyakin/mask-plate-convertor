#ifndef TEXTBASIC_H
#define TEXTBASIC_H

#include <QMetaType>


struct LineRange
{
    int first = -1;  // inclusive
    int last = 0;  // inclusive (!)

    bool isValid() const { return first >= 0 && last >= 0 && last >= first; }
};

struct TextPosition
{
    int line = -1;
    int column = 0;

    TextPosition() {}
    TextPosition(int lineArg, int columnArg) : line(lineArg), column(columnArg) {}

    bool valid() const  { return line >= 0 && column >= 0; }
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
