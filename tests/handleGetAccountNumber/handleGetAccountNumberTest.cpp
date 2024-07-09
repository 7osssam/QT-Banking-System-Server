
#include "Setup.h"

TEST_F(handleGetAccountNumberTest, onSuccess)
{
	QByteArray requestData = R"(
        {
            "Request": 2,
            "Data": {
                "email": "user1@example.com"
            }
        }
    )";

	QByteArray expectedResponse = R"(
		{
			"Response": 2,
			"Data": {
				"status": 1,
				"message": "Account number found",
				"account_number": 111
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

TEST_F(handleGetAccountNumberTest, onNoAccountFound)
{
	QByteArray requestData = R"(
        {
            "Request": 2,
            "Data": {
                "email": "user2@example.com"
            }
        }
    )";

	QByteArray expectedResponse = R"(
		{
			"Response": 2,
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

TEST_F(handleGetAccountNumberTest, onEmailNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 2,
			"Data": {
				"email": "notuser@example.com"
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 2,
			"Data": {
				"status": 0,
				"message": "email not found"
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
