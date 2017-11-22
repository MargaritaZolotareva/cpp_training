#include "WeatherApiService.h"
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>

WeatherApiService::WeatherApiService(QObject *parent)
    : QObject(parent)
{
}

QJsonObject WeatherApiService::getWeatherData(const QString &city)
{
    QUrl url("http://api.openweathermap.org/data/2.5/forecast");
    QUrlQuery query;
    query.addQueryItem("q", city);
    query.addQueryItem("cnt", "40");
    query.addQueryItem("id", "524901");
    query.addQueryItem("appid", "ab725e70b7bf32822598afe0d26c65db");

    url.setQuery(query.query());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = manager.get(request);
    QEventLoop loop;
    QJsonObject obj;

    connect(reply, &QNetworkReply::finished, [&]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Status code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Error: " << reply->error();
            qDebug() << "Error string: " << reply->errorString();
            reply->deleteLater();
            loop.quit();
            return 1;
        }
        obj = QJsonDocument::fromJson(reply->readAll()).object();
        reply->deleteLater();
        loop.quit();
    });

    loop.exec();
    return obj;
}
