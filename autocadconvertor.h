#ifndef AUTOCADCONVERTOR_H
#define AUTOCADCONVERTOR_H

#include <QString>

class Blueprint;


QString blueprintToAutocadCommandLineCommands(const Blueprint* blueprint);

#endif // AUTOCADCONVERTOR_H
