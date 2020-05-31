#ifndef AUTOCADLANGUAGE_H
#define AUTOCADLANGUAGE_H

#include <memory>
#include <vector>

#include <QString>
#include <QList>

#include "autocadconvertorsettings.h"


enum class AutocadCommand
{
    PLine,
    Width,
    Close,
};

class AutocadLanguage
{
public:
    virtual ~AutocadLanguage() {}
    virtual AutocadLanguageCode code() = 0;
    virtual QString codeName() = 0;
    virtual QString uiName() = 0;
    virtual QString commandText(AutocadCommand command) = 0;
};

class AutocadLanguageFactory
{
public:
    static AutocadLanguageFactory& singleton();

    const std::vector<std::unique_ptr<AutocadLanguage>>& allLanguages() const;
    AutocadLanguage* getLanguage(AutocadLanguageCode code) const;
    AutocadLanguage* getLanguageByName(QString codeName) const;

private:
    AutocadLanguageFactory();

private:
    std::vector<std::unique_ptr<AutocadLanguage>> m_languages;
};

#endif // AUTOCADLANGUAGE_H
