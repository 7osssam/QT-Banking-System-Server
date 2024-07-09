#ifndef DBRESULT_H
#define DBRESULT_H

#include <QJsonArray>
#include <QJsonObject>

namespace DB
{

class DbResult
{
public:
	DbResult();
	DbResult(const QList<QVariant>& data);
	bool		isEmpty() const;
	QJsonObject first() const;
	int			size() const;
	QJsonObject data(int pos) const;
	QJsonArray	data() const;
	QString		toJsonString() const;

private:
	QJsonArray Data_;
};

} // namespace DB

#endif // DBRESULT_H
