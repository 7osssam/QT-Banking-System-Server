#include "RequestManager.h"

QJsonObject RequestManager::handleJsonParseError(const QJsonParseError& parseError)
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

RequestManager::RequestManager(QMutex* Mutex) : Mutex_(Mutex)
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

QByteArray RequestManager::makeRequest(QByteArray data)
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

RequestManager::~RequestManager()
{
	for (auto request: requests_)
	{
		delete request;
	}
}