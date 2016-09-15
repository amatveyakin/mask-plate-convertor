#ifndef AUTOCADCONVERTOR_H
#define AUTOCADCONVERTOR_H

#include <QString>

class AutocadLanguage;
class Blueprint;


struct AutocadConvertorSettings
{
    QString autocadLanguageCodeName;
    double sizeCoeff = 1.;
};

class AutocadConvertor
{
public:
    AutocadConvertor(AutocadConvertorSettings settings);

    QString blueprintToAutocadCommandLineCommands(const Blueprint& blueprint) const;

private:
    AutocadLanguage* m_autocadLanguage;
    double m_sizeCoeff;
};

#endif // AUTOCADCONVERTOR_H
