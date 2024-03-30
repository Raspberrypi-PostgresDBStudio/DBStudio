#ifndef DBSERVER_H
#define DBSERVER_H

#include <gtkmm.h>

class DbStudio;

class DbServer
{
    private:
        std::string dbname;
        std::string user;
        std::string password; 
        std::string hostaddr;
        std::string port;
        std::string connParam;

    public:
        DbServer(std::string serverName, std::string portNumber, std::string databaseName, std::string userName, std::string passWord);
        DbServer(const DbServer *p);
        virtual ~DbServer(void);
       
        const std::string& serverName(void) { return hostaddr; }
        const std::string& databaseName(void) { return dbname; }
        const std::string& connectionString(void) { return connParam; }
        const std::string& getUserName(void) { return user; }
        const std::string& getPassword(void) { return password; }
        const std::string& getPort(void) { return port; }
};

#endif
