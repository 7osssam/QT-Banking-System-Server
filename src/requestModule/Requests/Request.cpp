#include "Request.h"

bool Request::isDBConnectionValid(DB::DatabaseManager* dbManager)
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

QJsonObject Request::CreateDBConnectionError(QJsonObject& response, QJsonObject& dataObj)
{
	dataObj.insert("status", int(false));
	dataObj.insert("message", "Internal server error");

	response.insert("Data", dataObj);

	QJsonDocument responseDoc(response);
	QByteArray	  responseData = responseDoc.toJson();

	qDebug().noquote() << "<-- DB Connection Error :\n" << responseDoc.toJson(QJsonDocument::Indented);

	return response;
}

QJsonObject Request::CreateErrorResponse(QJsonObject& response, QJsonObject& dataObj, QString message)
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