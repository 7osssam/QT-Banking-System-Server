#ifndef UPDATEPASSWORDREQUEST_H
#define UPDATEPASSWORDREQUEST_H

#include "Request.h"
#include "db.h"

class UpdatePasswordRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	UpdatePasswordRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;
		QString new_password;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 13);

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

			if (dataObj.contains("new_password"))
			{
				new_password = dataObj.value("new_password").toString();
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

			// Validate user credentials
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.data(0).value("id").toInt();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			// Update the password
			bool success = dbManager->where("email = ", email)->update("users", {{"password", new_password}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to update password");
			}

			data.insert("status", int(true));
			data.insert("message", "Password updated successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- UpdatePassword::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // UPDATEPASSWORDREQUEST_H
