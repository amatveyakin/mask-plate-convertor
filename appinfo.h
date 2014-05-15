#ifndef APPINFO_H
#define APPINFO_H

#include <QList>
#include <QString>


QString appName();
QString companyName();

QList<int> appVersion();
QString appVersionString();

QString titleText(const QString& subtitle = QString());
QString titleErrorText();
QString aboutText();

#endif // APPINFO_H
