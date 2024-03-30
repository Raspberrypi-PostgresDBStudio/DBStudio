#ifndef DBSERVERLIST_H
#define DBSERVERLIST_H

#include "DbServer.h"

class DbServerList
{
    private:
        struct node
        {
            DbServer *db;
            struct node *next;
            struct node *prev;
        } *front, *end;
        int numEntries;
        
    public:
        DbServerList(void);
        virtual ~DbServerList(void);
        void addFront(DbServer *d);
        void addEnd(DbServer *db);
        void deleteDatabase(int position);
        DbServer *searchDatabaseElement(const Glib::ustring& database);
        DbServer *searchServerElement(const Glib::ustring& server);
        DbServer *searchServerDatabaseElement(const Glib::ustring& server, const Glib::ustring& database);
        int getServerDatabaseIndex(const Glib::ustring& c);
        int count(void);
        DbServer *operator[](int i);
        void clear(void);
};

#endif
