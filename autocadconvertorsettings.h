#ifndef AUTOCADLANGUAGESETTINGS_H
#define AUTOCADLANGUAGESETTINGS_H

enum class AutocadLanguageCode {
    En,
    Ru,
};

struct AutocadConvertorSettings
{
    AutocadLanguageCode autocadLanguageCode = AutocadLanguageCode::En;
    double sizeCoeff = 1.;
    bool closePolygons = false;
};

#endif // AUTOCADLANGUAGESETTINGS_H
