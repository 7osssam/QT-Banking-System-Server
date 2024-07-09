#include "dbresult.h"

#include <QJsonDocument> // Include the necessary header file
#include <QJsonArray>	 // Include the necessary header file

using namespace DB;

DbResult::DbResult()
{
}

DbResult::DbResult(const QList<QVariant>& data)
{
	for (const QVariant& var: data)
	{
		Data_.append(QJsonObject::fromVariantMap(var.toMap()));
	}
}

bool DbResult::isEmpty() const
{
	return Data_.isEmpty();
}

QJsonObject DbResult::first() const
{
	return data(0);
}

int DbResult::size() const
{
	return Data_.size();
}

QJsonObject DbResult::data(int pos) const
{
	if (isEmpty() || pos < 0 || pos >= size())
	{
		return QJsonObject();
	}
	return Data_[pos].toObject();
}

QJsonArray DbResult::data() const
{
	return Data_;
}

QString DbResult::toJsonString() const
{
	QJsonDocument doc(Data_);
	return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}
