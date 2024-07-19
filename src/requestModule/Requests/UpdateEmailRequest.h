#ifndef UPDATEEMAILREQUEST_H
#define UPDATEEMAILREQUEST_H

#include "Request.h"
#include "db.h"

class UpdateEmailRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	UpdateEmailRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;
		QString new_email;

		QJsonObject response;

		QJsonObject data;

		response.insert("Response", 12);

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

			if (dataObj.contains("new_email"))
			{
				new_email = dataObj.value("new_email").toString();
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

			// change the password of that specific user
			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			// Check if the new email is already associated with another account
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			if (!result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Email already exists");
			}

			// Update the email
			bool success = dbManager->where("email = ", email)->update("users", {{"email", new_email}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to update email");
			}

			data.insert("status", int(true));
			data.insert("message", "Email updated successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- UpdateEmail::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // UPDATEEMAILREQUEST_H
