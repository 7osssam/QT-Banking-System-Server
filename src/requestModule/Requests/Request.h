// Request.h
#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonObject>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutexLocker>
#include "db.h"

class Request
{
protected:
	bool isDBConnectionValid(DB::DatabaseManager* dbManager)
	{
		if (dbManager == nullptr)
		{
			qDebug() << "Failed to create instance";
			return false;
		}

		QString dbError;

		if (!DB::DatabaseManager::checkConnection(dbError))
		{
			qDebug() << "Failed to connect to db" << dbError;
			return false;
		}

		return true;
	}

	QJsonObject CreateDBConnectionError(QJsonObject& response, QJsonObject& dataObj)
	{
		dataObj.insert("status", int(false));
		dataObj.insert("message", "Internal server error");

		response.insert("Data", dataObj);

		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		qDebug().noquote() << "<-- DB Connection Error :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject CreateErrorResponse(QJsonObject& response, QJsonObject& dataObj, QString message)
	{
		dataObj.insert("status", int(false));
		dataObj.insert("message", message);

		response.insert("Data", dataObj);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		qDebug().noquote() << "<-- Error Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

public:
	virtual ~Request()
	{
	}
	virtual QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) = 0;
};

#endif // REQUEST_H
