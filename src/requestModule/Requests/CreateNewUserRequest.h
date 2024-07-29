/**
 * @file CreateNewUserRequest.h
 *
 * @brief This file contains the declaration of the CreateNewUserRequest class, which handles requests to create new users.
 * @image html CreateNewUserRequest.svg
 */

#ifndef CREATENEWUSERREQUEST_H
#define CREATENEWUSERREQUEST_H

#include "Request.h"
#include "db.h"
#include <QRandomGenerator>

/**
 * @brief The CreateNewUserRequest class handles the creation of new users.
 *
 * This class processes requests to create new users, ensuring that the request
 * originates from an admin and that all required fields are provided. It also
 * handles database operations for adding the new user and their account if applicable.
 */
class CreateNewUserRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the CreateNewUserRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	CreateNewUserRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	/**
     * @brief Executes the request to create a new user.
     *
     * This method processes the JSON request to create a new user. It validates
     * the input data, checks the database connection, and ensures that the request
     * is made by an admin. If all validations pass, it creates a new user and their
     * account in the database.
     *
     * @param jsonObj A QJsonObject containing the request data.
     * @param m A QMutex reference for thread-safe operations.
     * @return A QJsonObject containing the response data.
     */
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

			//QJsonObject obj = result.first();
			QJsonObject obj = result.first();

			//if (obj.value("role").toString() != "admin")
			if (result.first().value("role").toString() != "admin")
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
			QVariant lastId = result.first().value("id");
			//! or using QVariant lastId = dbManager->lastInsertedId();

			// Create the account for the new user and randomly generate an account number of 6 digits
			// make sure the account number is unique
			int account_number = 0;

			// not the best way to generate unique account number but it's ok for now
			do
			{
				account_number = QRandomGenerator::global()->bounded(100000, 999999);
				result = dbManager->select("*")->table("accounts")->where("account_number =", account_number)->exec();
			} while (!result.isEmpty());

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
