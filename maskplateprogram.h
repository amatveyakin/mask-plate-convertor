#ifndef MASKPLATEPROGRAM_H
#define MASKPLATEPROGRAM_H

#include <QString>

class MaskPlateProgram
{
public:
    MaskPlateProgram();
    ~MaskPlateProgram();

    void processLine(const QString &line);
    QString convert() const;
};

#endif // MASKPLATEPROGRAM_H
