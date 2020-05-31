#include "appinfo.h"


QString appName()
{
    return "Mask plate convertor";
}

QString companyName()
{
    return "AMM Soft";
}


QList<int> appVersion()
{
  // TODO: extract version from git!
  return QList<int>() << 1 << 4 << 2;
}

QString appVersionString()
{
  QString versionString;
  for (int x : appVersion())
    versionString += QString::number(x) + '.';
  return versionString.left(versionString.length() - 1);
}


QString titleText(const QString& subtitle)
{
    if (subtitle.isEmpty())
        return appName();
    else
        return QString("%1 — %2").arg(subtitle, appName());
}

QString titleErrorText()
{
    return titleText("Ошибка");
}

QString aboutText()
{
    return QString("%1, версия %2.\n\n"
                   "Автор — Матвеякин Андрей.\n\n"
                   "Программа распространяется бесплатно по принципу «как есть»: автор не несёт ответственности за возможный ущерб, нанесённый в результате работы приложения.\n\n"
                   "Программа написана на C++/Qt."
                   ).arg(appName()).arg(appVersionString());
}

QString readmeText()
{
    return "При конвертации для русифицированного AutoCAD выбирать русский язык в \"Настройках AutoCAD\".\n\n"
           "Коэффициент масштабирования 1 — конвертация в микронах; "
           "коэффициент масштабирования 0.000001 — конвертация в метрах.\n\n"
           "Топологический рисунок располагать в центре рамки 127×127 мм.";
}
