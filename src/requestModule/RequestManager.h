// RequestManager.h
#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QMutex>
#include <QByteArray>
#include <QJsonParseError>
#include <QDebug>
#include "RequestHandler.h"

class RequestManager
{
private:
	QMutex*		   Mutex_;
	RequestHandler requestHandler_;

public:
	enum AvailableRequests
	{
		Login = 1,
		GetAccountnumber,
		GetBalance,
		GetTransactionsHistory,
		MakeTransaction,
		TransferAmount, // not used for now
		GetDatabase,
		CreateNewUser,
		DeleteUser,
		UpdateUser,
		// Special requests
		UserInit,
		UpdateEmail,
		UpdatePassword,
		JsonParseError = -1
	};

	RequestManager(QMutex* Mutex)
	{
		this->Mutex_ = Mutex;
	}

	QByteArray makeRequest(QByteArray data)
	{
		int				requestNumber;
		QJsonParseError parseError;
		QJsonDocument	jsonDoc = QJsonDocument::fromJson(data, &parseError);

		qDebug().noquote() << "--> Request received: " << jsonDoc.toJson(QJsonDocument::Indented);

		QJsonObject jsonObjResponse;

		QJsonObject jsonObjRequest = jsonDoc.object();

		if (parseError.error == QJsonParseError::NoError)
		{
			requestNumber = jsonObjRequest.value("Request").toInt();
		}
		else
		{
			requestNumber = -1;
			qCritical() << "JSON Parse Error: " << parseError.errorString();
		}

		switch (requestNumber)
		{
			case Login:
			{
				jsonObjResponse = requestHandler_.handleLogin(jsonObjRequest, *Mutex_);
			}
			break;
			case GetAccountnumber:
			{
				jsonObjResponse = requestHandler_.handleGetAccountNumber(jsonObjRequest, *Mutex_);
			}
			break;
			case GetBalance:
			{
				jsonObjResponse = requestHandler_.handleGetBalance(jsonObjRequest, *Mutex_);
			}
			break;
			case GetTransactionsHistory:
			{
				jsonObjResponse = requestHandler_.handleGetTransactionsHistory(jsonObjRequest, *Mutex_);
			}
			break;
			case MakeTransaction:
			{
				jsonObjResponse = requestHandler_.handleMakeTransaction(jsonObjRequest, *Mutex_);
			}
			break;
			case TransferAmount:
			{
				// jsonObjResponse = requestHandler_.handleTransferAmount(jsonObjRequest, *Mutex_);
			}
			break;
			case GetDatabase:
			{
				jsonObjResponse = requestHandler_.handleGetDatabase(jsonObjRequest, *Mutex_);
			}
			break;
			case CreateNewUser:
			{
				jsonObjResponse = requestHandler_.handleCreateNewUser(jsonObjRequest, *Mutex_);
			}
			break;
			case DeleteUser:
			{
				jsonObjResponse = requestHandler_.handleDeleteUser(jsonObjRequest, *Mutex_);
			}
			break;
			case UpdateUser:
			{
				jsonObjResponse = requestHandler_.handleUpdateUser(jsonObjRequest, *Mutex_);
			}
			break;
			case UserInit:
			{
				jsonObjResponse = requestHandler_.handleUserInit(jsonObjRequest, *Mutex_);
			}
			break;
			case UpdateEmail:
			{
				jsonObjResponse = requestHandler_.handleUpdateEmail(jsonObjRequest, *Mutex_);
			}
			break;
			case UpdatePassword:
			{
				jsonObjResponse = requestHandler_.handleUpdatePassword(jsonObjRequest, *Mutex_);
			}
			break;
			case JsonParseError:
			{
				jsonObjResponse = requestHandler_.handleJsonParseError(parseError);
			}
			default:
			{
				qCritical() << "Invalid request: " << requestNumber;
			}
			break;
		}

		QJsonDocument responseDoc(jsonObjResponse);
		QByteArray	  responseByteArr = responseDoc.toJson();

		return responseByteArr;
	}

	~RequestManager()
	{
	}
};

#endif // REQUESTMANAGER_H
