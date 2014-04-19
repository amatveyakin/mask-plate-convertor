#ifndef PROGRAMBASIC_H
#define PROGRAMBASIC_H

#include <assert.h>

#include <exception>
#include <map>

#include <QPoint>

struct RunningProgram;


extern const int mainRoutineIndex;
extern const int maxRecursionDepth;
extern const int startingLineWidth;


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


struct Arguments
{
    void set(int index, int value)      { m_data[index] = value; }
    bool isSet(int index) const         { return m_data.count(index); }
    int get(int index) const            { auto it = m_data.find(index);  assert(it != m_data.end());  return it->second; }
    Arguments replaced(const Arguments& newArguments) const;

private:
    std::map<int, int> m_data;
};


class Number
{
public:
    Number() : m_type(Literal), m_data(0), m_mult(1) {}
    static Number literal(int value)            { return Number(Literal,  value, 1); }
    static Number variable(int value, int mult) { return Number(Variable, value, mult); }
    int value(const Arguments& arguments, const RunningProgram& instance) const;

private:
    enum Type { Literal, Variable };

private:
    Type m_type;
    int m_data;
    int m_mult;

private:
    Number(Type type, int data, int mult) : m_type(type), m_data(data), m_mult(mult) {}
};


struct Movement
{
    Number x, y;
    QPoint value(const Arguments& arguments, const RunningProgram& instance);
};

#endif // PROGRAMBASIC_H
