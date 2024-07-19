#ifndef CREATENEWUSERREQUEST_H
#define CREATENEWUSERREQUEST_H

#include "Request.h"
#include "db.h"

class CreateNewUserRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	CreateNewUserRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email{};
		QString password{};
		QString first_name{};
		QString last_name{};
		QString new_email{};
		QString new_password{};
		QString role{};
		double	initial_balance{};

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 8);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("newUser"))
			{
				QJsonObject newUserObj = dataObj["newUser"].toObject();

				if (newUserObj.contains("first_name"))
				{
					first_name = newUserObj.value("first_name").toString();
				}
				if (newUserObj.contains("last_name"))
				{
					last_name = newUserObj.value("last_name").toString();
				}
				if (newUserObj.contains("email"))
				{
					new_email = newUserObj.value("email").toString();
				}
				if (newUserObj.contains("password"))
				{
					new_password = newUserObj.value("password").toString();
				}
				if (newUserObj.contains("role"))
				{
					role = newUserObj.value("role").toString();
				}
				if (newUserObj.contains("initial_balance"))
				{
					initial_balance = newUserObj.value("initial_balance").toDouble();
				}
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

			qDebug() << "First Name: " << first_name;
			qDebug() << "Last Name: " << last_name;
			qDebug() << "Email: " << new_email;
			qDebug() << "Password: " << new_password;
			qDebug() << "Role: " << role;
			qDebug() << "Initial Balance: " << initial_balance;

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data,
										   "Unauthorized, Cannot create new user. User is not an admin");
			}

			if (new_email.isEmpty() || new_password.isEmpty() || first_name.isEmpty() || last_name.isEmpty() ||
				role.isEmpty())
			{
				return CreateErrorResponse(response, data, "Missing required fields");
			}

			// invalid role
			if (role != "admin" && role != "user")
			{
				return CreateErrorResponse(response, data, "Invalid role");
			}

			// admin can't have account and initial balance
			if (role == "admin" && initial_balance != 0)
			{
				return CreateErrorResponse(response, data, "Admin can't have account and initial balance");
			}

			// Check if there is a user with the same email
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			if (!result.isEmpty())
			{
				return CreateErrorResponse(response, data, "User already exists");
			}

			// Create the new user
			bool success = dbManager->insert("users", {{"email", new_email},
													   {"password", new_password},
													   {"first_name", first_name},
													   {"last_name", last_name},
													   {"role", role}});
			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to create new user");
			}

			if (role == "admin" && initial_balance == 0)
			{
				qDebug() << "New admin created successfully";

				data.insert("status", int(true));
				data.insert("message", "New admin created successfully");

				response.insert("Data", data);

				break;
			}

			// Get the user id from the last insert operation
			result = dbManager->select("id")->table("users")->where("email =", new_email)->exec();
			QVariant lastId = result.data(0).value("id");
			//! or using QVariant lastId = dbManager->lastInsertedId();

			// Create the account for the new user and randomly generate an account number
			int account_number = rand() % 1000000 + 100000;

			success = dbManager->insert(
				"accounts",
				{{"account_number", account_number}, {"user_id", lastId.toInt()}, {"balance", initial_balance}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to create account for the new user");
			}

			data.insert("status", int(true));
			data.insert("message", "New user created successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- CreateNewUser::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // CREATENEWUSERREQUEST_H
