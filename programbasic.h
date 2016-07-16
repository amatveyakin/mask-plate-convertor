#ifndef PROGRAMBASIC_H
#define PROGRAMBASIC_H

#include <assert.h>

#include <exception>
#include <map>

#include <QPoint>

#include "textbasic.h"

struct RunningProgram;


extern const int mainRoutineIndex;
extern const int maxRecursionDepth;
extern const int startingLineWidth;


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
    static Number literal(int value)            { return {Literal, value, 1}; }
    static Number variable(int value, int mult) { return {Variable, value, mult}; }
    Number() {}
    int value(const Arguments& arguments, const RunningProgram& instance) const;

private:
    enum Type { Literal, Variable };

private:
    Type m_type = Literal;
    int m_data = 0;
    int m_mult = 1;

private:
    Number(Type type, int data, int mult) : m_type(type), m_data(data), m_mult(mult) {}
};


struct Movement
{
    Number x, y;

    QPoint value(const Arguments& arguments, const RunningProgram& instance);
    bool argumentDependent() const;
};


struct Call
{
    Call(int routineIndexArg) : routineIndex(routineIndexArg) {}

    int routineIndex = mainRoutineIndex;
    TextPosition inputPosition;
};

class CallStack : public std::vector<Call>
{
public:
    int currentRoutine() const;
};

#endif // PROGRAMBASIC_H
