# Sequence Diagrams

## ServerHandler Diagrams:
### Sequence diagram : ServerHandler initialization and destruction

```plantuml
@startuml
actor User
participant "ServerHandler" as SH
participant "RequestManager" as RM

User -> SH: new ServerHandler()
activate SH
SH -> RM: new RequestManager()
activate RM
RM -> SH: return
SH -> User: ServerHandler created

User -> SH: delete ServerHandler()
activate SH
SH -> RM: delete RequestManager()
deactivate RM
SH -> User: ServerHandler destroyed
deactivate SH
@enduml
```

### Sequence diagram : `ServerHandler` processes `readyRead` event
```plantuml
@startuml
actor Client
participant "QTcpSocket" as Socket
participant "ServerHandler" as SH
participant "QCryptographicHash" as Hash
participant "RequestManager" as RM

Client -> Socket: send data
Socket -> SH: readyRead()
activate SH
SH -> Socket: readAll()
SH -> Hash: compute hash(requestData)
Hash -> SH: return computedHash

alt Hash verification failed
    SH -> SH: log "Hash verification failed"
else Hash verification succeeded
    SH -> SH: log "Hash verification succeeded"
    SH -> RM: makeRequest(requestData)
    activate RM
    RM -> SH: return response
    SH -> Socket: write(response)
    SH -> Socket: waitForBytesWritten()
    deactivate RM
end
deactivate SH
@enduml
```

### Sequence diagram : `ServerHandler` runs the `run` method
```plantuml
@startuml
actor User
participant "QTcpSocket" as Socket
participant "ServerHandler" as SH
participant "QEventLoop" as Loop

User -> SH: run()
activate SH
SH -> Socket: new QTcpSocket()
activate Socket
alt setSocketDescriptor failed
    SH -> SH: log "Failed to set socket descriptor"
    SH -> Socket: delete QTcpSocket
    deactivate Socket
else setSocketDescriptor succeeded
    SH -> Loop: new QEventLoop()
    activate Loop
    SH -> Socket: connect(disconnected, Loop.quit)
    SH -> Socket: connect(readyRead, ServerHandler.readyRead)
    SH -> Loop: exec()
    deactivate Loop
end
deactivate SH
@enduml
```
## RequestManager Diagrams:
### Sequence diagram : `RequestManager` processes requests
```plantuml
@startuml
actor "ServerHandler" as SH
participant "RequestManager" as RM
participant "QJsonDocument" as JsonDoc
participant "QJsonParseError" as JsonError
participant "Request" as Request

SH -> RM: makeRequest(data)
activate RM
RM -> JsonDoc: fromJson(data)
alt JSON parse error
    RM -> SH: handleJsonParseError(parseError)
else JSON parse succeeded
    JsonDoc -> RM: return jsonObjRequest
    RM -> RM: parse request number
    alt request not found
        RM -> SH: log "Request not found"
    else request found
        RM -> Request: execute(jsonObjRequest, Mutex)
        activate Request
        Request -> RM: return response
        RM -> SH: return responseByteArr
        deactivate Request
    end
end
deactivate RM
@enduml
```

## Server Diagrams:
### Sequence diagram : Initialization and destruction of `Server`

```plantuml
@startuml
actor User
participant "Server" as S
participant "QThreadPool" as TP
participant "QMutex" as M

User -> S: new Server()
activate S
S -> TP: new QThreadPool()
activate TP
TP -> S: return
S -> M: new QMutex()
activate M
M -> S: return
S -> User: Server created

User -> S: delete Server()
activate S
S -> TP: clear()
S -> TP: deleteLater()
deactivate TP
S -> User: Server destroyed
deactivate S
@enduml
```

### Sequence diagram : `Server` starts and stops

```plantuml
@startuml
actor Admin
participant "Server" as S

Admin -> S: start(port)
activate S
alt listen failed
    S -> S: log "Failed to start server"
else listen succeeded
    S -> S: log "Server started on port"
end
deactivate S

Admin -> S: quit()
activate S
S -> S: close()
S -> Admin: Server Stopped
deactivate S
@enduml
```

### Sequence diagram : `Server` processes `incomingConnection`

```plantuml
@startuml
actor Client
participant "Server" as S
participant "ServerHandler" as SH
participant "QThreadPool" as TP

Client -> S: new connection
activate S
S -> SH: new ServerHandler()
activate SH
SH -> S: setAutoDelete(true)
S -> TP: start(SH)
deactivate SH
deactivate S
@enduml
```

## Request Diagrams:
### Sequence diagram : `Request` processes a request

```plantuml
@startuml
actor "ServerHandler" as SH
participant "RequestManager" as RM
participant "QJsonDocument" as JsonDoc
participant "QJsonParseError" as JsonError
participant "Request" as Req

SH -> RM: makeRequest(data)
activate RM
RM -> JsonDoc: fromJson(data)
alt JSON parse error
    RM -> SH: handleJsonParseError(parseError)
else JSON parse succeeded
    JsonDoc -> RM: return jsonObjRequest
    RM -> RM: parse request number
    alt request not found
        RM -> SH: log "Request not found"
    else request found
        RM -> Req: execute(jsonObjRequest, Mutex)
        activate Req
        Req -> RM: return response
        RM -> SH: return responseByteArr
        deactivate Req
    end
end
deactivate RM
@enduml
```

### Sequence diagram : `Request` base class handles database connection and error response

```plantuml
@startuml
participant "Request" as R
participant "DB::DatabaseManager" as DB
participant "QJsonObject" as JsonObj
participant "QJsonDocument" as JsonDoc

R -> DB: checkConnection()
alt DB connection is null
    R -> R: log "Failed to create instance"
    return false
else DB connection failed
    R -> DB: checkConnection()
    return false with dbError
else DB connection successful
    return true
end

R -> JsonObj: CreateDBConnectionError(response, dataObj)
activate JsonObj
JsonObj -> JsonDoc: construct JSON response
JsonDoc -> JsonObj: return response
JsonObj -> R: return response
deactivate JsonObj

R -> JsonObj: CreateErrorResponse(response, dataObj, message)
activate JsonObj
JsonObj -> JsonDoc: construct JSON response
JsonDoc -> JsonObj: return response
JsonObj -> R: return response
deactivate JsonObj
@enduml
```
## DatabaseManager Diagrams:
### Sequence diagram : `DatabaseManager` initialization and connection check

```plantuml
@startuml
actor Admin
participant "DatabaseManager" as DM
participant "dbConfig" as Config
participant "QSqlDatabase" as DB
participant "QSqlQuery" as Query

Admin -> DM: setDbSetting(host, port, username, password, dbname)
activate DM
DM -> Config: set(host, port, username, password, dbname)
Config -> DM: return
DM -> Admin: DB settings configured
deactivate DM

Admin -> DM: createInstance(checkDBName, newConnection, &err)
activate DM
alt dbConfig not set
    DM -> Admin: log "Database configuration not set"
    DM -> Admin: return nullptr
    deactivate DM
else dbConfig set
    DM -> DM: new DatabaseManager()
    activate DM
    DM -> DB: getDatabase()
    activate DB
    alt DB is open
        DM -> DB: close()
    end
    DM -> DB: setHostName(host)
    DM -> DB: setPort(port)
    DM -> DB: open()
    DB -> Query: CREATE DATABASE IF NOT EXISTS dbname
    deactivate DB
    DM -> DB: setDatabaseName(dbname)
    DM -> DB: setUserName(username)
    DM -> DB: setPassword(password)
    DM -> DB: open()
    alt open successful
        DM -> DM: return true
    else open failed
        DM -> Admin: log "Error opening database connection"
        DM -> Admin: return nullptr
    end
    deactivate DM
end

Admin -> DM: checkConnection(&error)
activate DM
DM -> DM: createInstance(true, false, &err)
alt createInstance failed
    DM -> Admin: log err
    DM -> Admin: return false
else createInstance successful
    DM -> Admin: return true
end
deactivate DM
@enduml
```

## Sequence diagram : `DatabaseManager` processes queries (Example: select query) and returns results

```plantuml
@startuml
actor Client
participant "DatabaseManager" as DM
participant "QSqlQuery" as Query
participant "QSqlRecord" as Record
participant "DbResult" as Result

Client -> DM: select("*").table("users").where("email =", email).exec()
activate DM
DM -> Query: getDatabase()
activate Query
alt database is not open
    Query -> DM: log "Database not open"
    return
else database is open
    Query -> DM: prepare and execute query
    alt query execution failed
        Query -> DM: log error
        DM -> Client: return DbResult(empty)
        deactivate DM
    else query execution succeeded
        Query -> Record: record()
        activate Record
        Record -> DM: while query.next()
        DM -> Result: create DbResult from query result
        deactivate Record
        DM -> Client: return DbResult(result)
        deactivate DM
    end
end
deactivate Query
@enduml
```
