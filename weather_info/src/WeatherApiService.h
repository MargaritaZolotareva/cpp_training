#ifndef WEATHER_API_SERVICE_H
#define WEATHER_API_SERVICE_H
#include <QObject>
#include <QNetworkAccessManager>

class WeatherApiService : public QObject {
    Q_OBJECT
public:
    explicit WeatherApiService(QObject *parent = 0);
    QJsonObject getWeatherData(const QString &city);

private:
    QNetworkAccessManager manager;
};

#endif // WEATHER_API_SERVICE_H
