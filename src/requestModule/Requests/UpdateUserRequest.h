/**
 * @file UpdateUserRequest.h
 *
 * @brief This file contains the declaration of the UpdateUserRequest class, which handles user update requests.
 * @image html UpdateUserRequest.svg
 */

#ifndef UPDATEUSERREQUEST_H
#define UPDATEUSERREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The UpdateUserRequest class handles user update requests.
 *
 * This class processes requests to update user details, including first name,
 * last name, email, and role. It ensures that the requester has the necessary
 * permissions and that the updated email is not already associated with another user.
 */
class UpdateUserRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the UpdateUserRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	UpdateUserRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// Log to database log table (if needed)
	}

	/**
     * @brief Executes the user update request.
     *
     * This method processes the JSON request to update user details. It validates
     * the requester's role, checks if the email is already associated with another
     * user, and updates the user information in the database.
     *
     * @param jsonObj The JSON object containing the request data.
     * @param m The mutex to lock during the execution.
     * @return A JSON object containing the response data.
     */
	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		int		account_number;
		QString new_first_name;
		QString new_last_name;
		QString new_email;
		QString new_role;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 10);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}
			if (dataObj.contains("account_number"))
			{
				account_number = dataObj.value("account_number").toInt();
			}
			if (dataObj.contains("newData"))
			{
				QJsonObject newDataObj = dataObj["newData"].toObject();

				if (newDataObj.contains("first_name"))
				{
					new_first_name = newDataObj.value("first_name").toString();
				}
				if (newDataObj.contains("last_name"))
				{
					new_last_name = newDataObj.value("last_name").toString();
				}
				if (newDataObj.contains("email"))
				{
					new_email = newDataObj.value("email").toString();
				}
				if (newDataObj.contains("role"))
				{
					new_role = newDataObj.value("role").toString();
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

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("users")->where("email =", email)->exec();
			QJsonObject	 obj = result.first();
			QString		 sneder_role = obj.value("role").toString();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			if (sneder_role != "admin")
			{
				return CreateErrorResponse(response, data, "Unauthorized, Cannot update user");
			}

			// Check if the account number is valid
			result = dbManager->select("*")->table("accounts")->where("account_number =", account_number)->exec();
			// Get the user id from the account number
			int user_id = result.first().value("user_id").toInt();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account number does not exist");
			}

			// check that new email is not associated with another account or user unless it is the current user
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			int new_user_id = result.first().value("id").toInt();

			qDebug() << "user_id: " << user_id;
			qDebug() << "new_user_id: " << new_user_id;

			if (!result.isEmpty() && result.first().value("id").toInt() != user_id)
			{
				return CreateErrorResponse(response, data, "Email is associated with another account");
			}

			qDebug() << "Debugging update user";
			qDebug() << "user_id: " << user_id;
			qDebug() << "new_email: " << new_email;
			qDebug() << "new_role: " << new_role;
			qDebug() << "new_first_name: " << new_first_name;
			qDebug() << "new_last_name: " << new_last_name;

			// check for the role of the user
			result = dbManager->select("role")->table("users")->where("id =", user_id)->exec();
			QString current_role = result.first().value("role").toString();

			if (current_role == "user" && new_role == "admin")
			{
				// delete the account associated bank account
				bool success = dbManager->where("account_number = ", account_number)->del("accounts");

				if (!success)
				{
					return CreateErrorResponse(response, data, "Failed to delete account");
				}
			}

			// Update the user details
			bool success = dbManager->where("id = ", user_id)
							   ->update("users", {{"first_name", new_first_name},
												  {"last_name", new_last_name},
												  {"email", new_email},
												  {"role", new_role}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to update user");
			}

			data.insert("status", int(true));
			data.insert("message", "User updated successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- UpdateUser::Response :\n" << QJsonDocument(response).toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // UPDATEUSERREQUEST_H
