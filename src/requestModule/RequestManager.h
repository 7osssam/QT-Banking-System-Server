#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QMutex>
#include <QByteArray>
#include <QJsonParseError>
#include <QDebug>
#include <QMap>
#include "Requests/Request.h"
#include "Requests/LoginRequest.h"
#include "Requests/GetAccountNumberRequest.h"
#include "Requests/GetBalanceRequest.h"
#include "Requests/GetTransactionsHistoryRequest.h"
#include "Requests/MakeTransactionRequest.h"
#include "Requests/GetDatabaseRequest.h"
#include "Requests/CreateNewUserRequest.h"
#include "Requests/DeleteUserRequest.h"
#include "Requests/UpdateUserRequest.h"
#include "Requests/UserInitRequest.h"
#include "Requests/UpdateEmailRequest.h"
#include "Requests/UpdatePasswordRequest.h"

/**
 * @brief The RequestManager class handles incoming requests and dispatches them to the appropriate request handler.
 *
 * The RequestManager class is responsible for managing and processing different types of requests.
 * It maintains a map of request handlers for various request types, processes incoming request data,
 * and generates appropriate responses. The class also includes error handling for JSON parsing issues.
 */
class RequestManager
{
private:
	QMutex*				Mutex_;	   ///< Mutex for thread-safe operations.
	QMap<int, Request*> requests_; ///< Map of request handlers indexed by request type.

	/**
     * @brief Handles JSON parsing errors and creates a corresponding error response.
     *
     * @param parseError A QJsonParseError object that contains details about the parsing error.
     * @return A QJsonObject containing the error response with an error message and status.
     */
	QJsonObject handleJsonParseError(const QJsonParseError& parseError)
	{
		QJsonObject response;
		QJsonObject data;

		response.insert("Response", -1);

		data.insert("status", int(false));
		data.insert("message", "JSON Parse Error: " + parseError.errorString());

		response.insert("Data", data);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- JSON Parse Error::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

public:
	/**
     * @brief Enum representing the different types of requests handled by the RequestManager.
     *
     * This enum defines various request types and special requests that the RequestManager can handle.
     */
	enum AvailableRequests
	{
		Login = 1,				///< Request for user login.
		GetAccountnumber,		///< Request to retrieve account number.
		GetBalance,				///< Request to get account balance.
		GetTransactionsHistory, ///< Request to retrieve transaction history.
		MakeTransaction,		///< Request to make a transaction.
		TransferAmount,			///< Request to transfer amount (not used currently).
		GetDatabase,			///< Request to get database information.
		CreateNewUser,			///< Request to create a new user.
		DeleteUser,				///< Request to delete a user.
		UpdateUser,				///< Request to update user information.
		UserInit,				///< Special request for user initialization.
		UpdateEmail,			///< Special request to update user email.
		UpdatePassword,			///< Special request to update user password.
		JsonParseError = -1		///< Special error code for JSON parsing errors.
	};

	/**
     * @brief Constructs a RequestManager with the specified mutex for thread safety.
     *
     * @param Mutex A pointer to a QMutex used to ensure thread-safe operations.
     */
	RequestManager(QMutex* Mutex) : Mutex_(Mutex)
	{
		requests_[Login] = new LoginRequest();
		requests_[GetAccountnumber] = new GetAccountNumberRequest();
		requests_[GetBalance] = new GetBalanceRequest();
		requests_[GetTransactionsHistory] = new GetTransactionsHistoryRequest();
		requests_[GetDatabase] = new GetDatabaseRequest();
		requests_[MakeTransaction] = new MakeTransactionRequest();
		requests_[CreateNewUser] = new CreateNewUserRequest();
		requests_[DeleteUser] = new DeleteUserRequest();
		requests_[UpdateUser] = new UpdateUserRequest();
		requests_[UserInit] = new UserInitRequest();
		requests_[UpdateEmail] = new UpdateEmailRequest();
		requests_[UpdatePassword] = new UpdatePasswordRequest();
	}

	/**
     * @brief Processes an incoming request and generates an appropriate response.
     *
     * @param data A QByteArray containing the request data in JSON format.
     * @return A QByteArray containing the response data in JSON format.
     *
     * This method parses the incoming JSON data, determines the type of request, and delegates the processing
     * to the corresponding request handler. It also handles JSON parsing errors and generates error responses if needed.
     */
	QByteArray makeRequest(QByteArray data)
	{
		int				requestNumber;
		QJsonParseError parseError;
		QJsonDocument	jsonDoc = QJsonDocument::fromJson(data, &parseError);

		qDebug().noquote() << "--> Request received: " << jsonDoc.toJson(QJsonDocument::Indented);

		QJsonObject jsonObjResponse;

		QJsonObject jsonObjRequest = jsonDoc.object();

		do
		{
			if (parseError.error == QJsonParseError::NoError)
			{
				requestNumber = jsonObjRequest.value("Request").toInt();
			}
			else
			{
				qCritical() << "JSON Parse Error: " << parseError.errorString();
				jsonObjResponse = handleJsonParseError(parseError);
				break;
			}

			if (requests_.contains(requestNumber))
			{
				//Request* request = requests_[requestNumber];
				jsonObjResponse = requests_[requestNumber]->execute(jsonObjRequest, *Mutex_);
			}
			else
			{
				qCritical() << "Request not found";
				qCritical() << "Invalid request: " << requestNumber;
			}
		} while (false);

		QJsonDocument responseDoc(jsonObjResponse);
		QByteArray	  responseByteArr = responseDoc.toJson();

		return responseByteArr;
	}

	/**
     * @brief Destructor for the RequestManager.
     *
     * Cleans up any resources allocated by the RequestManager.
     */
	~RequestManager()
	{
		for (auto request: requests_)
		{
			delete request;
		}
	}
};

#endif // REQUESTMANAGER_H
