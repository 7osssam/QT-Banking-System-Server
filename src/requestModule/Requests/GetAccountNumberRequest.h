#ifndef GETACCOUNTNUMBERREQUEST_H
#define GETACCOUNTNUMBERREQUEST_H

#include "Request.h"
#include "db.h"

class GetAccountNumberRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	GetAccountNumberRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 2);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
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

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("A.account_number")
						 ->table("Users U")
						 ->join("JOIN Accounts A ON U.id = A.user_id")
						 ->where("U.email =", email)
						 ->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "No account found");
			}

			QJsonObject obj = result.data(0);

			data.insert("status", int(true));
			data.insert("message", "Account number found");
			data.insert("account_number", obj.value("account_number").toInt());

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetAccountNumber::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // GETACCOUNTNUMBERREQUEST_H
