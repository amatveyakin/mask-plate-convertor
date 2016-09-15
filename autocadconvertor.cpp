// TODO: Add golden tests: (program -> Autocad)

#include "autocadconvertor.h"
#include "autocadlanguage.h"
#include "blueprint.h"


AutocadConvertor::AutocadConvertor(AutocadConvertorSettings settings)
{
    m_autocadLanguage = AutocadLanguageFactory::singleton().getLanguage(settings.autocadLanguageCodeName);
    assert(m_autocadLanguage);
    m_sizeCoeff = settings.sizeCoeff;
}

QString AutocadConvertor::blueprintToAutocadCommandLineCommands(const Blueprint& blueprint) const
{
    // TODO: Finish closed polygons with a special command (optionally (?))
    QString result;
    for (const Element& curElement : blueprint.elements()) {
        for (int i = 0; i < curElement.polygon.size(); ++i) {
            const QString coords = QString("%1,%2").arg(QString::number(curElement.polygon[i].x() * m_sizeCoeff, 'f'),
                                                        QString::number(curElement.polygon[i].y() * m_sizeCoeff, 'f'));
            if (i == 0) {
                result.append(QString("%1 %2 %3 %4 %4\n").arg(
                                  m_autocadLanguage->commandText(AutocadCommand::PLine),
                                  coords,
                                  m_autocadLanguage->commandText(AutocadCommand::Width),
                                  QString::number(curElement.width * m_sizeCoeff)));
            } else {
                result.append(QString("%1\n").arg(coords));
            }
        }
        result.append("\n");
    }
    return result;
}
