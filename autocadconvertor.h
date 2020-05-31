#ifndef AUTOCADCONVERTOR_H
#define AUTOCADCONVERTOR_H

#include <QString>

#include "autocadconvertorsettings.h"

class AutocadLanguage;
class Blueprint;


class AutocadConvertor
{
public:
    AutocadConvertor(AutocadConvertorSettings settings);

    QString blueprintToAutocadCommandLineCommands(const Blueprint& blueprint) const;

private:
    AutocadLanguage* m_autocadLanguage;
    double m_sizeCoeff;
    bool m_closePolygons;
};

#endif // AUTOCADCONVERTOR_H
