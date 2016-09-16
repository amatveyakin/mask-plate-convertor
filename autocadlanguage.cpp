#include "autocadlanguage.h"


class AutocadLanguageEnglish : public AutocadLanguage
{
public:
    QString codeName() override { return "en"; }
    QString uiName() override { return "&Английский"; }
    QString commandText(AutocadCommand command) override {
        switch(command) {
        case AutocadCommand::PLine: return "_pline";
        case AutocadCommand::Width: return "width";
        case AutocadCommand::Close: return "close";
        }
    }
};

class AutocadLanguageRussian : public AutocadLanguage
{
public:
    QString codeName() override { return "ru"; }
    QString uiName() override { return "&Русский"; }
    QString commandText(AutocadCommand command) override {
        switch(command) {
        case AutocadCommand::PLine: return "плиния";
        case AutocadCommand::Width: return "ширина";
        case AutocadCommand::Close: return "замкнуть";
        }
    }
};

AutocadLanguageFactory& AutocadLanguageFactory::singleton()
{
    static AutocadLanguageFactory factory;
    return factory;
}

const std::vector<std::unique_ptr<AutocadLanguage>>& AutocadLanguageFactory::allLanguages() const
{
    return m_languages;
}

AutocadLanguage* AutocadLanguageFactory::getLanguage(const QString& codeName) const
{
    for (const auto& lang : m_languages) {
        if (lang->codeName() == codeName)
            return lang.get();
    }
    return nullptr;
}

AutocadLanguage* AutocadLanguageFactory::defaultLanguage() const
{
    return m_languages.front().get();
}

AutocadLanguageFactory::AutocadLanguageFactory()
{
    m_languages.push_back(std::make_unique<AutocadLanguageEnglish>());
    m_languages.push_back(std::make_unique<AutocadLanguageRussian>());
}
