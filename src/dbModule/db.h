

#ifndef DB_H
#define DB_H

#include "dbresult.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

namespace DB
{

struct DatabaseConfig
{
	DatabaseConfig()
	{
	}
	DatabaseConfig(const QString& host, int port, const QString& username, const QString& password,
				   const QString& dbname) :
		port(port), host(host), username(username), password(password), dbName(dbname)
	{
	}
	void set(const QString& host, int port, const QString& username, const QString& password, const QString& dbname)
	{
		this->port = port;
		this->host = host;
		this->username = username;
		this->password = password;
		this->dbName = dbname;
	}
	int		port;
	QString host;
	QString username;
	QString password;
	QString dbName;
};

class DatabaseManager
{
private:
	DatabaseManager();

public:
	~DatabaseManager();
	static DatabaseManager* createInstance(bool checkDBName = false, bool newConnection = false,
										   QString* err = nullptr);
	static bool		 setDbSetting(const QString& host, int port, const QString& username, const QString& password,
								  const QString& dbname);
	static bool		 checkConnection(QString& error);
	DatabaseManager* reset();
	DatabaseManager* select(const QString& value);
	DatabaseManager* table(const QString& value);
	DatabaseManager* where(const QString& value, const QVariant& val = QVariant());
	DatabaseManager* like(const QString& value, const QString& val);
	DatabaseManager* likePost(const QString& value, const QString& val);
	DatabaseManager* likeNative(const QString& value, const QString& val);
	DatabaseManager* whereOr(const QString& value);
	DatabaseManager* join(const QString& value);
	DatabaseManager* limit(int limit);
	DatabaseManager* start(int start);
	DatabaseManager* sort(const QString& sort);
	DatabaseManager* group(const QString& group);
	QString			 getSelectQuery(const QString& select = QString());
	QSqlError		 lastError();
	QVariant		 lastInsertedId();
	QString			 lastQuery();
	DbResult		 exec();
	DbResult		 execResult(const QString& sql);
	DbResult		 get(const QString& tableName);
	bool			 exec(const QString& sqlcommand);
	bool			 insert(const QString& table, const QVariantMap& data);
	bool			 update(const QString& table, const QVariantMap& data);
	bool			 del(const QString& table);
	int				 count();
	DatabaseManager* clone();
	bool			 beginTransaction();
	bool			 commit();
	bool			 rollback();
	inline bool		 isSupportTransaction()
	{
		return isSupportTransaction_;
	}
	inline DatabaseManager* clearSelect()
	{
		Select_.clear();
		return this;
	}
	bool truncateTable(const QString& table);

private:
	static QStringList		Created_;
	static QStringList		Update_;
	static QStringList		SoftDelete_;
	bool					isSupportTransaction_ = false;
	QString					Where_;
	QString					Select_;
	QString					Join_;
	QString					Table_;
	QString					Limit_;
	QString					Start_;
	QString					Sort_;
	QString					Group_;
	QList<DatabaseManager*> Childs_;
	DatabaseManager*		Parent_;
	QString					LastQuery_;
	QVariant				InsertedId_;
	QSqlError				LastError_;

	static QSqlDatabase getDatabase();
	bool	init(const QString& host, int port, const QString& username, const QString& password, const QString& dbname,
				 bool checkDBName = false, bool newConnection = false);
	void	postQuery(QSqlQuery* query);
	QString dataToString(const QVariantMap& map);
};

} // namespace DB
#endif // DB_H
