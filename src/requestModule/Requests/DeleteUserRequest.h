/**
 * @file DeleteUserRequest.h
 *
 * @brief The DeleteUserRequest class handles the deletion of users.
 * @image html DeleteUserRequest.svg
 */

#ifndef DELETEUSERREQUEST_H
#define DELETEUSERREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The DeleteUserRequest class handles the deletion of users.
 *
 * This class processes requests to delete users, ensuring that the request
 * originates from an admin and that the account number provided is valid.
 * It performs database operations to remove the user and their associated account.
 */
class DeleteUserRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the DeleteUserRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	DeleteUserRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// Log to database log table (if needed)
	}

	/**
     * @brief Executes the request to delete a user.
     *
     * This method processes the JSON request to delete a user. It validates
     * the input data, checks the database connection, and ensures that the request
     * is made by an admin. If all validations pass, it deletes the user and their
     * account from the database.
     *
     * @param jsonObj The JSON object containing the request data.
     * @param m The mutex to lock during the execution.
     * @return A JSON object containing the response data.
     */
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

			QJsonObject obj = result.first();

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
			int user_id = result.first().value("user_id").toInt();

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
