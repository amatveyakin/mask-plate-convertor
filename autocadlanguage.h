#ifndef AUTOCADLANGUAGE_H
#define AUTOCADLANGUAGE_H

#include <memory>
#include <vector>

#include <QString>
#include <QList>

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
    virtual QString codeName() = 0;
    virtual QString uiName() = 0;
    virtual QString commandText(AutocadCommand command) = 0;
};

class AutocadLanguageFactory
{
public:
    static AutocadLanguageFactory& singleton();

    const std::vector<std::unique_ptr<AutocadLanguage>>& allLanguages() const;
    AutocadLanguage* getLanguage(const QString& codeName) const;
    AutocadLanguage* defaultLanguage() const;

private:
    AutocadLanguageFactory();

private:
    std::vector<std::unique_ptr<AutocadLanguage>> m_languages;
};

#endif // AUTOCADLANGUAGE_H
