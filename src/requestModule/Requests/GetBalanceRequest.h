#ifndef GETBALANCEREQUEST_H
#define GETBALANCEREQUEST_H

#include "Request.h"
#include "db.h"

class GetBalanceRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	GetBalanceRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		int accountNumber;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 3);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("account_number"))
			{
				accountNumber = dataObj.value("account_number").toInt();
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

			DB::DbResult result =
				dbManager->select("balance")->table("Accounts")->where("account_number =", accountNumber)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account not found");
			}

			QJsonObject obj = result.data(0);

			data.insert("status", int(true));
			data.insert("message", "Balance fetched successfully");
			data.insert("balance", obj.value("balance").toDouble());

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetBalance::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // GETBALANCEREQUEST_H
