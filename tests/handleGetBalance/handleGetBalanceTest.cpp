#include "Setup.h"

TEST_F(handleGetBalanceTest, onSuccess)
{
	QByteArray requestData = R"(
		{
			"Request": 3,
			"Data": {
				"account_number": 111
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 3,
			"Data": {
				"status": 1,
				"balance": 150.50,
				"message": "Balance fetched successfully"
			}
		}
	)";

	QByteArray actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	QJsonDocument expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);
}

TEST_F(handleGetBalanceTest, onAccountNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 3,
			"Data": {
				"account_number": 999
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 3,
			"Data": {
				"status": 0,
				"message": "Account not found"
			}
		}
	)";

	QByteArray actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	QJsonDocument expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);
}