#include "autocadconvertor.h"
#include "blueprint.h"


QString blueprintToAutocadCommandLineCommands(const Blueprint* blueprint)
{
    // TODO: Should closed polygons be finished by a special command?
    const double sizeCoeff = 0.001;
    QString result;
    for (const Element& curElement : blueprint->elements()) {
        for (int i = 0; i < curElement.polygon.size(); ++i) {
            QString coords = QString("%1,%2").arg(QString::number(curElement.polygon[i].x() * sizeCoeff, 'f'), QString::number(curElement.polygon[i].y() * sizeCoeff, 'f'));
            if (i == 0)
                result.append(QString("_pline %1 width %2 %2\n").arg(coords, QString::number(curElement.width * sizeCoeff)));
            else
                result.append(QString("%1\n").arg(coords));
        }
        result.append("\n");
    }
    return result;
}
