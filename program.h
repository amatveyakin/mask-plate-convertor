#ifndef PROGRAM_H
#define PROGRAM_H

#include <memory>
#include <map>

class Blueprint;
class Routine;

class Program
{
public:
    Program();
    ~Program();

    const Routine* mainRoutine() const;
    const Routine* subroutine(int index) const;

    Blueprint execute() const;

private:
    std::unique_ptr<Routine> m_mainRoutine;
    std::map<int, std::unique_ptr<Routine>> m_subroutines;

private:
    friend class ProgramCommand;
};

#endif // PROGRAM_H
