#include "DbServer.h"

DbServer::DbServer(std::string serverName, std::string portNumber, std::string databaseName, std::string userName, std::string passWord)
{
    hostaddr = serverName;
    port = portNumber;  
    dbname = databaseName;
    user = userName;
    password = passWord;
    connParam = "dbname = " + dbname + " user = " + user + " password = " + password + " hostaddr = " + hostaddr + " port = " + port;
}
DbServer::DbServer(const DbServer *p)
{
    dbname = p->dbname;
    user = p->user;
    password = p->password; 
    hostaddr = p->hostaddr;
    port = p->port;
    connParam = p->connParam;
}

DbServer::~DbServer(void)
{
}
