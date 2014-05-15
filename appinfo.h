#ifndef APPINFO_H
#define APPINFO_H

#include <QString>


extern const QString appName;
extern const QString companyName;

QString titleText(const QString& subtitle = QString());
QString titleErrorText();

#endif // APPINFO_H
