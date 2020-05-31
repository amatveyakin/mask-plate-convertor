#include "autocadlanguage.h"


class AutocadLanguageEnglish : public AutocadLanguage
{
public:
    AutocadLanguageCode code() override { return AutocadLanguageCode::En; }
    QString codeName() override { return "en"; }
    QString uiName() override { return "&Английский"; }
    QString commandText(AutocadCommand command) override {
        switch(command) {
        case AutocadCommand::PLine: return "_pline";
        case AutocadCommand::Width: return "width";
        case AutocadCommand::Close: return "close";
        }
        abort();
    }
};

class AutocadLanguageRussian : public AutocadLanguage
{
public:
    AutocadLanguageCode code() override { return AutocadLanguageCode::Ru; }
    QString codeName() override { return "ru"; }
    QString uiName() override { return "&Русский"; }
    QString commandText(AutocadCommand command) override {
        switch(command) {
        case AutocadCommand::PLine: return "плиния";
        case AutocadCommand::Width: return "ширина";
        case AutocadCommand::Close: return "замкнуть";
        }
        abort();
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

AutocadLanguage* AutocadLanguageFactory::getLanguage(AutocadLanguageCode code) const
{
    for (const auto& lang : m_languages) {
        if (lang->code() == code)
            return lang.get();
    }
    return nullptr;
}

AutocadLanguage* AutocadLanguageFactory::getLanguageByName(QString codeName) const {
    for (const auto& lang : m_languages) {
        if (lang->codeName() == codeName)
            return lang.get();
    }
    return nullptr;
}

AutocadLanguageFactory::AutocadLanguageFactory()
{
    m_languages.push_back(std::make_unique<AutocadLanguageEnglish>());
    m_languages.push_back(std::make_unique<AutocadLanguageRussian>());
}
