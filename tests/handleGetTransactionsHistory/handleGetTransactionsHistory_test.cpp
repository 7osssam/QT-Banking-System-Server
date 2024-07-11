#include "Setup.h"

TEST_F(handleGetTransactionsHistoryTest, onSuccessUser)
{
	QByteArray requestData = R"(
        {
            "Request": 4,
            "Data": {
				"email": "user1@example.com"
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 4,
            "Data": {
				"status": 1,
                "message": "Transaction history retrieved for account number 111",
                "List": [
                    {
                        "from_account_number": 111,
                        "to_account_number": 222,
						"amount": 200.0,
                        "created_at": "2021-01-01T00:00:00.000"
                    },
                    {
                        "from_account_number": 222,
                        "to_account_number": 111,
                        "amount": 500.5,
                        "created_at": "2021-01-03T00:00:00.000"
                    }
                ]
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

TEST_F(handleGetTransactionsHistoryTest, onSuccessAdmin)
{
	QByteArray requestData = R"(
        {
            "Request": 4,
            "Data": {
				"email": "admin@mail.com"
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 4,
            "Data": {
				"status": 1,
                "message": "Transaction history retrieved for all users",
                "List": [
                    {
                        "from_account_number": 111,
                        "to_account_number": 222,
						"amount": 200.0,
                        "created_at": "2021-01-01T00:00:00.000"
                    },
                    {
                        "from_account_number": 222,
                        "to_account_number": 111,
                        "amount": 500.5,
                        "created_at": "2021-01-03T00:00:00.000"
                    },
                    {
                        "from_account_number": 222,
                        "to_account_number": 333,
                        "amount": 700.0,
                        "created_at": "2021-01-04T00:00:00.000"
                    }
                ]
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


TEST_F(handleGetTransactionsHistoryTest, onTransactionNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 4,
			"Data": {
				"email": "user4@example.com"
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

TEST_F(handleGetTransactionsHistoryTest, onAccountNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 4,
			"Data": {
				"email": "user5@example.com"
				}
			}
		)";

	QByteArray expectedResponse = R"(
		{
			"Response": 4,
			"Data": {
				"status": 0,
				"message": "No account found"
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