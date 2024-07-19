#ifndef DELETEUSERREQUEST_H
#define DELETEUSERREQUEST_H

#include "Request.h"
#include "db.h"

class DeleteUserRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	DeleteUserRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString admin_email;
		int		account_number;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 9);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				admin_email = dataObj.value("email").toString();
			}
			if (dataObj.contains("account_number"))
			{
				account_number = dataObj.value("account_number").toInt();
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

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", admin_email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data, "Unauthorized, Cannot delete user.");
			}

			// Check if the account number is valid
			result = dbManager->select("*")->table("accounts")->where("account_number =", account_number)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account number does not exist");
			}

			// Get the user id from the account number
			int user_id = result.data(0).value("user_id").toInt();

			bool success = dbManager->where("account_number = ", account_number)->del("accounts");

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to delete account");
			}

			success = dbManager->where("id = ", user_id)->del("users");

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to delete user");
			}

			data.insert("status", int(true));
			data.insert("message", "User deleted successfully");

			response.insert("Data", data);

		} while (false);

		// Send response
		qDebug().noquote() << "<-- DeleteUser::Response :\n" << QJsonDocument(response).toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // DELETEUSERREQUEST_H
