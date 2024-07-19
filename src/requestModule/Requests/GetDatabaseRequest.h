#ifndef GETDATABASEREQUEST_H
#define GETDATABASEREQUEST_H

#include "Request.h"
#include "db.h"

class GetDatabaseRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	GetDatabaseRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QJsonObject response;
		QJsonObject data;

		QString admin_email;

		response.insert("Response", 7);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				admin_email = dataObj.value("email").toString();
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
			DB::DbResult result = dbManager->select("role")->table("Users")->where("email =", admin_email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data, "Unauthorized, Cannot get database. User is not an admin");
			}

			// Get all users from the database
			result = dbManager->select("*")->table("Users")->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "No data found");
			}

			QJsonArray userList;

			for (int i = 0; i < result.size(); ++i)
			{
				QJsonObject userObj;
				QJsonObject obj = result.data(i);

				userObj.insert("account_number", QJsonValue::Null);
				userObj.insert("first_name", obj.value("first_name").toString());
				userObj.insert("last_name", obj.value("last_name").toString());
				userObj.insert("email", obj.value("email").toString());
				userObj.insert("role", obj.value("role").toString());
				userObj.insert("balance", QJsonValue::Null);

				userList.append(userObj);
			}

			// Get all accounts from the database that are associated with the users
			result = dbManager->select("A.account_number, A.balance, U.email")
						 ->table("Accounts A")
						 ->join("JOIN Users U ON A.user_id = U.id")
						 ->exec();

			if (!result.isEmpty())
			{
				for (int i = 0; i < result.size(); ++i)
				{
					QJsonObject obj = result.data(i);

					for (int j = 0; j < userList.size(); ++j)
					{
						QJsonObject userObj = userList[j].toObject();
						if (userObj.value("email").toString() == obj.value("email").toString())
						{
							userObj.insert("account_number", obj.value("account_number").toInt());
							userObj.insert("balance", obj.value("balance").toDouble());
							userList[j] = userObj;
							break;
						}
					}
				}
			}

			data.insert("status", int(true));
			data.insert("message", "Database fetched successfully");
			data.insert("users", userList);

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetDatabase::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // GETDATABASEREQUEST_H
