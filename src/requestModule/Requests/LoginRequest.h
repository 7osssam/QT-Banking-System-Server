// LoginRequest.h
#ifndef LOGINREQUEST_H
#define LOGINREQUEST_H

#include "Request.h"
#include "db.h"

class LoginRequest : public Request
{
private:
	//RequestHandler* handler_;
	DB::DatabaseManager* dbManager = nullptr;

public:
	LoginRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 1);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("password"))
			{
				password = dataObj.value("password").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid(dbManager))
			{
				return CreateDBConnectionError(response, data);
			}

			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.data(0).value("id").toInt();
			QJsonObject	 obj = result.data(0);

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			data.insert("status", int(true));
			data.insert("message", "Login successful");
			data.insert("first_name", obj.value("first_name").toString());
			data.insert("role", obj.value("role").toString());

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- Login::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // LOGINREQUEST_H
