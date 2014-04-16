#include "appinfo.h"


extern const QString appName = "Mask plate convertor";

QString titleText(const QString& subtitle)
{
    if (subtitle.isEmpty())
        return appName;
    else
        return QString("%1 — %2").arg(subtitle, appName);
}

QString titleErrorText()
{
    return titleText("Ошибка");
}
