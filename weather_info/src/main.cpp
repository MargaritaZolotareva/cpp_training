#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <WeatherApiService.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QDebug>
#include <QtMath>
#include <QSettings>

const QString compassDirections[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };

QString degreeToCompass(const double degree)
{
    int direction = qFloor((degree / 22.5) + 0.5);
    return compassDirections[(direction % 16)];
}

QJsonArray parseWeatherData(const QJsonObject &weatherData, const int numberOfDays)
{
    int dayStart = QDate::currentDate().day();
    int dayFinish = dayStart + numberOfDays;
    QJsonArray weatherArray = weatherData.value("list").toArray();
    QJsonArray parsedWeatherData;
    for (int i = 0; i < weatherArray.count(); ++i) {
        QDateTime weatherDate = QDateTime::fromString(weatherArray[i].toObject().value("dt_txt").toString(), "yyyy-MM-dd HH:mm:ss");
        if (weatherDate.date().day() >= dayStart && weatherDate.date().day() < dayFinish) {
            QJsonObject weatherObject;
            QJsonObject currentObject = weatherArray[i].toObject();
            QJsonObject mainObject = currentObject.value("main").toObject();
            QJsonObject windObject = currentObject.value("wind").toObject();
            QJsonObject descriptionObject = currentObject.value("weather").toArray()[0].toObject();
            weatherObject.insert("temperature", QString::number(mainObject.value("temp").toDouble() - 273.15));
            weatherObject.insert("humidity", QString::number(mainObject.value("humidity").toInt()));
            weatherObject.insert("windSpeed", QString::number(windObject.value("speed").toDouble()));
            weatherObject.insert("windDegree", degreeToCompass(windObject.value("deg").toDouble()));
            weatherObject.insert("description", descriptionObject.value("description"));
            weatherObject.insert("date", weatherDate.toString(Qt::RFC2822Date));
            parsedWeatherData.append(weatherObject);
        }
    }

    return parsedWeatherData;
}

void printWeatherData(const QJsonArray &weatherData, const QString &city)
{
    qDebug() << "\n";
    qDebug() << "Weather for " + city + ":";

    for (int i = 0; i < weatherData.count(); ++i) {
        QJsonObject currentWeatherData = weatherData[i].toObject();
        qDebug() << "\n";
        qDebug() << currentWeatherData.value("date").toString();
        qDebug() << "Cloudiness: " + currentWeatherData.value("description").toString();
        qDebug() << "Temperature: " + currentWeatherData.value("temperature").toString() + QString::fromUtf8("°C");
        qDebug() << "Humidity: " + currentWeatherData.value("humidity").toString() + "%";
        qDebug() << "Wind: " + currentWeatherData.value("windSpeed").toString() + "m/s, " + currentWeatherData.value("windDegree").toString();
    }
}

void saveQueryInUserSettings(QSettings &settings, const QString &city, const QString &numberOfDays)
{
    settings.setValue("city", city);
    settings.setValue("numberOfDays", numberOfDays);
}

void processWeatherData(WeatherApiService &weatherApiService, const QString &city, const int &numberOfDays, QSettings &settings)
{
    QJsonObject weatherData = weatherApiService.getWeatherData(city);
    if (weatherData.empty()) {
        qDebug("Empty data returned");
        return;
    }
    QJsonArray parsedWeatherData = parseWeatherData(weatherData, numberOfDays);
    printWeatherData(parsedWeatherData, city);
    saveQueryInUserSettings(settings, city, QString::number(numberOfDays));
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    QSettings settings("Xsolla", "Weather Info");

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption cityOption(QStringList() << "city",
        QCoreApplication::translate("main", "Gets weather from API for city <city>."),
        QCoreApplication::translate("main", "city"));
    parser.addOption(cityOption);

    QCommandLineOption currentDayOption(QStringList() << "current-day",
        QCoreApplication::translate("main", "Gets weather for current day."));
    parser.addOption(currentDayOption);

    QCommandLineOption numberOfDaysOption(QStringList() << "days",
        QCoreApplication::translate("main", "Gets weather for <number> days (up to 5 days)."),
        QCoreApplication::translate("main", "number"));
    parser.addOption(numberOfDaysOption);
    parser.process(app);
    WeatherApiService weatherApiService;

    if (parser.isSet(cityOption)) {
        if (parser.isSet(numberOfDaysOption)) {
            QString city = parser.value(cityOption);
            int numberOfDays = parser.value(numberOfDaysOption).toInt();
            if (numberOfDays > 5 || numberOfDays < 1) {
                qDebug("Number of days should be from 1 to 5 inclusively");
                app.quit();
            }
            processWeatherData(weatherApiService, city, numberOfDays, settings);
        }
        else if (parser.isSet(currentDayOption)) {
            QString city = parser.value(cityOption);
            int numberOfDays = 1;
            processWeatherData(weatherApiService, city, numberOfDays, settings);
        }
        else if (settings.contains("numberOfDays")) {
            QString city = parser.value(cityOption);
            int numberOfDays = settings.value("numberOfDays").toInt();
            if (numberOfDays > 5 || numberOfDays < 1) {
                qDebug("Number of days should be from 1 to 5 inclusively");
                app.quit();
            }
            processWeatherData(weatherApiService, city, numberOfDays, settings);
        }
        else {
            QString city = parser.value(cityOption);
            int numberOfDays = 1;
            processWeatherData(weatherApiService, city, numberOfDays, settings);
        }
    }
    else if (parser.isSet(numberOfDaysOption)) {
        if (settings.contains("city")) {
            QString city = settings.value("city").toString();
            int numberOfDays = parser.value(numberOfDaysOption).toInt();
            if (numberOfDays > 5 || numberOfDays < 1) {
                qDebug("Number of days should be from 1 to 5 inclusively");
                app.quit();
            }
            processWeatherData(weatherApiService, city, numberOfDays, settings);
        }
        else {
            qDebug("City must be specified");
            app.quit();
        }
    }
    else if (parser.isSet(currentDayOption)) {
        if (settings.contains("city")) {
            QString city = settings.value("city").toString();
            int numberOfDays = 1;
            processWeatherData(weatherApiService, city, numberOfDays, settings);
        }
        else {
            qDebug("City must be specified");
            app.quit();
        }
    }
    else if (settings.contains("city") && settings.contains("numberOfDays")) {
        QString city = settings.value("city").toString();
        int numberOfDays = settings.value("numberOfDays").toInt();
        if (numberOfDays > 5 || numberOfDays < 1) {
            qDebug("Number of days should be from 1 to 5 inclusively");
            app.quit();
        }
        processWeatherData(weatherApiService, city, numberOfDays, settings);
    }
    else {
        qDebug("City and number of days must be specified");
        app.quit();
    }

    app.quit();
}
