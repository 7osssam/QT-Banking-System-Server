#include "Setup.h"

using namespace testing;

TEST_F(handleMakeTransactionTest, onSuccess)
{
	QByteArray requestData = R"(
        {
            "Request": 5,
            "Data": {
                "from_account_number": 111,
				"to_account_number": 222,
				"to_email": "",
				"transaction_amount": 100.0
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 5,
            "Data": {
				"status": 1,
				"message": "Transaction successful"
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

	// Check if the transaction was recorded in the database

	DB::DbResult historyResult =
		dbManager_->select("amount")->table("Transactions")->where("from_account_number =", 111)->exec();

	// print the result for debugging purposes

	// Assert using a loop and matchers
	for (size_t i = 0; i < historyResult.size(); i++)
	{
		QJsonObject obj = historyResult.data(i);
		qDebug() << "Data: " << obj;
		qDebug() << "amount: " << obj.value("amount").toDouble();

		EXPECT_THAT(obj.value("amount").toDouble(), DoubleEq(100.0));
	}
}

TEST_F(handleMakeTransactionTest, onInsufficientFunds)
{
	QByteArray requestData = R"(
		{
			"Request": 5,
			"Data": {
				"from_account_number": 111,
				"to_account_number": 222,
				"to_email": "",
				"transaction_amount": 2000.0
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 5,
			"Data": {
				"status": 0,
				"message": "Insufficient balance"
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

TEST_F(handleMakeTransactionTest, onInvalidFromAccount)
{
	QByteArray requestData = R"(
		{
			"Request": 5,
			"Data": {
				"from_account_number": 999,
				"to_account_number": 222,
				"to_email": "",
				"transaction_amount": 100.0
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 5,
			"Data": {
				"status": 0,
				"message": "You don't have an account"
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

TEST_F(handleMakeTransactionTest, onInvalidToAccount)
{
	QByteArray requestData = R"(
		{
			"Request": 5,
			"Data": {
				"from_account_number": 111,
				"to_account_number": 999,
				"to_email": "",
				"transaction_amount": 100.0
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 5,
			"Data": {
				"status": 0,
				"message": "Invalid to account number"
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

// Multiple transactions
TEST_F(handleMakeTransactionTest, onMultipleTransactions)
{
	QByteArray requestData = R"(
		{
			"Request": 5,
			"Data": {
				"from_account_number": 111,
				"to_account_number": 222,
				"to_email": "",
				"transaction_amount": 200.0
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 5,
			"Data": {
				"status": 1,
				"message": "Transaction successful"
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

	// Second transaction
	requestData = R"(
		{
			"Request": 5,
			"Data": {
				"from_account_number": 222,
				"to_account_number": 111,
				"to_email": "",
				"transaction_amount": 50.0
			}
		}
	)";

	expectedResponse = R"(
		{
			"Response": 5,
			"Data": {
				"status": 1,
				"message": "Transaction successful"
			}
		}
	)";

	actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);

	// Check if the transaction was recorded in the database
	DB::DbResult historyResult = dbManager_->select("amount")
									 ->select("from_account_number")
									 ->select("to_account_number")
									 ->table("Transactions")
									 ->exec();

	QJsonObject obj = historyResult.first();
	EXPECT_THAT(obj.value("amount").toDouble(), DoubleEq(200.0));
	EXPECT_THAT(obj.value("from_account_number").toInt(), Eq(111));
	EXPECT_THAT(obj.value("to_account_number").toInt(), Eq(222));

	obj = historyResult.data(1);
	EXPECT_THAT(obj.value("amount").toDouble(), DoubleEq(50.0));
	EXPECT_THAT(obj.value("from_account_number").toInt(), Eq(222));
	EXPECT_THAT(obj.value("to_account_number").toInt(), Eq(111));

	// assert the balance of the accounts
	DB::DbResult accountResult =
		dbManager_->select("balance")->table("Accounts")->where("account_number =", 111)->exec();

	obj = accountResult.first();
	EXPECT_THAT(obj.value("balance").toDouble(), DoubleEq(850.0));

	accountResult = dbManager_->select("balance")->table("Accounts")->where("account_number =", 222)->exec();

	obj = accountResult.first();
	EXPECT_THAT(obj.value("balance").toDouble(), DoubleEq(5150.0));
}

TEST_F(handleMakeTransactionTest, onEmailInsteadOfAccountNumber)
{
	QByteArray requestData = R"(
        {
            "Request": 5,
            "Data": {
                "from_account_number": 111,
				"to_account_number": -1,
				"to_email": "user2@example.com",
				"transaction_amount": 100.0
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 5,
            "Data": {
				"status": 1,
				"message": "Transaction successful"
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

	// Check if the transaction was recorded in the database

	DB::DbResult historyResult =
		dbManager_->select("amount")->table("Transactions")->where("from_account_number =", 111)->exec();

	// print the result for debugging purposes

	// Assert using a loop and matchers
	for (size_t i = 0; i < historyResult.size(); i++)
	{
		QJsonObject obj = historyResult.data(i);
		qDebug() << "Data: " << obj;
		qDebug() << "amount: " << obj.value("amount").toDouble();

		EXPECT_THAT(obj.value("amount").toDouble(), DoubleEq(100.0));
	}
}

TEST_F(handleMakeTransactionTest, onInvalidToEmailWithNoAccount)
{
	QByteArray requestData = R"(
        {
            "Request": 5,
            "Data": {
                "from_account_number": 111,
				"to_account_number": -1,
				"to_email": "user3@example.com",
				"transaction_amount": 100.0
            }
        }
    )";

	QByteArray expectedResponse = R"(
		{
			"Response": 5,
			"Data": {
				"status": 0,
				"message": "This email is not associated with any account"
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