#ifndef APPINFO_H
#define APPINFO_H

#include <QString>


extern const QString appName;

QString titleText(const QString& subtitle = QString());
QString titleErrorText();

#endif // APPINFO_H
