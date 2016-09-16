#ifndef AUTOCADCONVERTOR_H
#define AUTOCADCONVERTOR_H

#include <QString>

class AutocadLanguage;
class Blueprint;


struct AutocadConvertorSettings
{
    QString autocadLanguageCodeName;
    double sizeCoeff = 1.;
    bool closePolygons = false;
};

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
