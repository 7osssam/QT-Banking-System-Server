# Modules Sequence Diagrams

## ServerHandler Diagrams:
### Sequence diagram : ServerHandler initialization and destruction

![ServerHandler](docs/diagrams/plantuml/sequence-diagrams/Modules/ServerHandler_init.svg)

### Sequence diagram : `ServerHandler` processes `readyRead` event

![ServerHandler_readyRead](docs/diagrams/plantuml/sequence-diagrams/Modules/ServerHandler_readyRead.svg)

### Sequence diagram : `ServerHandler` runs the `run` method

![ServerHandler_run](docs/diagrams/plantuml/sequence-diagrams/Modules/ServerHandler_run.svg)

## RequestManager Diagrams:
### Sequence diagram : `RequestManager` processes requests

![RequestManager](docs/diagrams/plantuml/sequence-diagrams/Modules/RequestManager_requests.svg)

## Server Diagrams:
### Sequence diagram : Initialization and destruction of `Server`

![Server](docs/diagrams/plantuml/sequence-diagrams/Modules/Server_init.svg)

### Sequence diagram : `Server` starts and stops

![Server_start_stop](docs/diagrams/plantuml/sequence-diagrams/Modules/Server_start_stop.svg)

### Sequence diagram : `Server` processes `incomingConnection`

![Server_incomingConnection](docs/diagrams/plantuml/sequence-diagrams/Modules/Server_incomingConnection.svg)

## Request Diagrams:
### Sequence diagram : `Request` processes a request

![Request](docs/diagrams/plantuml/sequence-diagrams/Modules/Request.svg)

### Sequence diagram : `Request` base class handles database connection and error response

![Request_base](docs/diagrams/plantuml/sequence-diagrams/Modules/Request_base.svg)

## DatabaseManager Diagrams:
### Sequence diagram : `DatabaseManager` initialization and connection check

![DatabaseManager_init](docs/diagrams/plantuml/sequence-diagrams/Modules/DatabaseManager_init.svg)

## Sequence diagram : `DatabaseManager` processes queries (Example: select query) and returns results

![DatabaseManager_query](docs/diagrams/plantuml/sequence-diagrams/Modules/DatabaseManager_query.svg)