# Sequence Diagrams

## ServerHandler Diagrams:
### Sequence diagram : ServerHandler initialization and destruction

<!-- image of Diagram from plantuml server -->
![ServerHandler](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/ServerHandler_init.puml)

### Sequence diagram : `ServerHandler` processes `readyRead` event

<!-- image of Diagram from plantuml server -->
![ServerHandler_readyRead](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/ServerHandler_readyRead.puml)

### Sequence diagram : `ServerHandler` runs the `run` method

<!-- image of Diagram from plantuml server -->
![ServerHandler_run](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/ServerHandler_run.puml)

## RequestManager Diagrams:
### Sequence diagram : `RequestManager` processes requests

<!-- image of Diagram from plantuml server -->
![RequestManager](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/RequestManager_requests.puml)

## Server Diagrams:
### Sequence diagram : Initialization and destruction of `Server`

<!-- image of Diagram from plantuml server -->
![Server](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/Server_init.puml)

### Sequence diagram : `Server` starts and stops

<!-- image of Diagram from plantuml server -->
![Server_start_stop](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/Server_start_stop.puml)

### Sequence diagram : `Server` processes `incomingConnection`

<!-- image of Diagram from plantuml server -->
![Server_incomingConnection](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/Server_incomingConnection.puml)

## Request Diagrams:
### Sequence diagram : `Request` processes a request

<!-- image of Diagram from plantuml server -->
![Request](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/Request.puml)

### Sequence diagram : `Request` base class handles database connection and error response

<!-- image of Diagram from plantuml server -->
![Request_base](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/Request_base.puml)

## DatabaseManager Diagrams:
### Sequence diagram : `DatabaseManager` initialization and connection check

<!-- image of Diagram from plantuml server -->
![DatabaseManager_init](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/DatabaseManager_init.puml&fmt=png)

## Sequence diagram : `DatabaseManager` processes queries (Example: select query) and returns results

<!-- image of Diagram from plantuml server -->
![DatabaseManager_query](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/7osssam/QT-Banking-System-Server/master/docs/sequence-diagrams/Modules/DatabaseManager_query.puml&fmt=png)