// TODO: Adopt a normal unit test framework

#include <assert.h>

#include <QFile>
#include <QResource>
#include <QTextStream>

#include "autocadconvertor.h"
#include "autocadlanguage.h"
#include "blueprint.h"
#include "program.h"
#include "programparser.h"
#include "runningprogram.h"

namespace {

// TODO: Test different settings
AutocadConvertorSettings autocadConvertorSettings()
{
    AutocadConvertorSettings settings;
    settings.autocadLanguageCodeName = "en";
    settings.sizeCoeff = 0.000001;
    settings.closePolygons = true;
    return settings;
}

QString convert(QString programText)
{
    const std::unique_ptr<Program> program = ProgramParser::parseProgram(programText);
    ExecutionResult executionResult = program->execute();
    // TODO: Check executionWarning
    AutocadConvertor convertor(autocadConvertorSettings());
    return convertor.blueprintToAutocadCommandLineCommands(*executionResult.blueprint);
}

QString readFile(QString filename) {
    QFile file(filename);
    bool success = file.open(QIODevice::ReadOnly | QIODevice::Text);
    assert(success);
    QTextStream in(&file);
    return in.readAll();
}

void writeFile(QString filename, QString content) {
    QFile file(filename);
    bool success = file.open(QIODevice::WriteOnly | QIODevice::Text);
    assert(success);
    QTextStream out(&file);
    out << content;
}

QString getActualResultFilename(QString f) {
    QString prefix = ":/";
    assert(f.startsWith(prefix));
    f.remove(0, prefix.length());
    return f + ".x";
}

void expectSucceess(QString programFile, QString resultFile) {
    QString programText = readFile(programFile);
    QString expectedResult = readFile(resultFile);
    QString result = convert(programText);
    if (result.trimmed() != expectedResult.trimmed()) {
        writeFile(getActualResultFilename(resultFile), result);
        assert(false);
    }
}

}  // namespace

void runConversionTests() {
    // TODO: Add failure tests
    expectSucceess(":/goldens/4_a01.source", ":/goldens/4_a01.autocad");
    expectSucceess(":/goldens/7_om.source", ":/goldens/7_om.autocad");
}
