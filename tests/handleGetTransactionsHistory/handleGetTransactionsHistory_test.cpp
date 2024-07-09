#include "setup.h"

TEST_F(handleGetTransactionsHistoryTest, onSuccess)
{
	QByteArray requestData = R"(
        {
            "Request": 4,
            "Data": {
                "account_number": 1000000000
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 4,
            "Data": {
				"status": 1,
                "account_number": 1000000000,
                "List": [
                    {
                        "to_account_number": 1000000001,
                        "transaction_amount": 100.0,
                        "transaction_type": "-",
                        "created_at": "2021-01-01T00:00:00.000"
                    },
                    {
                        "to_account_number": 1000000002,
                        "transaction_amount": 130.0,
                        "transaction_type": "-",
                        "created_at": "2021-01-03T00:00:00.000"
                    }
                ],
                "message": "Transaction history retrieved"
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

TEST_F(handleGetTransactionsHistoryTest, onAccountNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 4,
			"Data": {
				"account_number": 999999999
				}
			}
		)";

	QByteArray expectedResponse = R"(
		{
			"Response": 4,
			"Data": {
				"status": 0,
				"message": "No transactions found"
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
