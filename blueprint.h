#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <QString>

class Blueprint
{
public:
    Blueprint();
    ~Blueprint();

    QString toAutocadCommandLineCommands() const;
    // TODO: Also convert to something that QPainter can draw
};

#endif // BLUEPRINT_H
