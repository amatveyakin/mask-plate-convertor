// TODO: Add golden tests: (program -> Autocad)

#include "autocadconvertor.h"
#include "autocadlanguage.h"
#include "blueprint.h"


AutocadConvertor::AutocadConvertor(AutocadConvertorSettings settings)
{
    m_autocadLanguage = AutocadLanguageFactory::singleton().getLanguage(settings.autocadLanguageCodeName);
    assert(m_autocadLanguage);
    m_sizeCoeff = settings.sizeCoeff;
    m_closePolygons = settings.closePolygons;
}

QString AutocadConvertor::blueprintToAutocadCommandLineCommands(const Blueprint& blueprint) const
{
    QString result;
    for (const Element& curElement : blueprint.elements()) {
        if (!curElement.laserOn)
            continue;
        const QPolygon& polygon = curElement.polygon;
        for (int i = 0; i < polygon.size(); ++i) {
            const QString coords = QString("%1,%2").arg(QString::number(polygon[i].x() * m_sizeCoeff, 'f'),
                                                        QString::number(polygon[i].y() * m_sizeCoeff, 'f'));
            if (i == 0) {
                result.append(QString("%1 %2 %3 %4 %4\n").arg(
                                  m_autocadLanguage->commandText(AutocadCommand::PLine),
                                  coords,
                                  m_autocadLanguage->commandText(AutocadCommand::Width),
                                  QString::number(curElement.width * m_sizeCoeff, 'f')));
            } else if (m_closePolygons && i == polygon.size() - 1 && polygon[i] == polygon[0]) {
                result.append(QString("%1\n").arg(
                                  m_autocadLanguage->commandText(AutocadCommand::Close)));
            } else {
                result.append(QString("%1\n").arg(coords));
            }
        }
        result.append("\n");
    }
    return result;
}
