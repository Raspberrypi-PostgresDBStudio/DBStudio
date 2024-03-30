#include "DbServerList.h"

DbServerList::DbServerList(void)
{
    front = end = nullptr;
    numEntries = 0;
}

DbServerList::~DbServerList(void)
{
    clear();
}

void DbServerList::addFront(DbServer *d)
{
    node *temp = new node();
    temp->db = d;
    temp->prev = nullptr;
    temp->next = front;

    // List is empty
    if(front == nullptr)
        end = temp;     
    else
        front->prev = temp;
        
    front = temp;
    numEntries++;
}

void DbServerList::addEnd(DbServer *d)
{
    node *temp;
    temp = new node();
    temp->db = d;
    temp->prev = end;
    temp->next = nullptr;

    // if list is empty
    if(end == nullptr)
        front = temp;
    else
        end->next = temp;   
    end = temp;
    numEntries++;
}

void DbServerList::deleteDatabase(int position)
{
    if(position < numEntries)
    {
        node *n = front;
        int i = 0;
        while(n != nullptr)
        {
            if(i == position)
            {
                if(n->prev == nullptr)
                {
                    front = n->next; //the next node will be front of list
                    front->prev = nullptr;
                }
                // if node to be deleted is last node of list
                else if(n->next == nullptr)
                {
                    end = n->prev;   // the previous node will be last of list
                    end->next = nullptr;
                }
                else
                {
                    //previous node's next will point to current node's next
                    n->prev->next = n->next;
                    //next node's prev will point to current node's prev
                    n->next->prev = n->prev;
                }
                //delete node
                delete n->db;
                delete n;
                numEntries--;
                return;
            }
            i++;
            n = n->next;
        }
    }
    return; 
}

DbServer *DbServerList::searchDatabaseElement(const Glib::ustring& database)
{
    node *n = front;
    for(int i = 0 ; i < numEntries ; i++)
    {
        if(n->db->databaseName() == database)
        {
            return n->db;
        }
        n = n->next;
    }
    return nullptr;
}

DbServer *DbServerList::searchServerElement(const Glib::ustring& server)
{
    node *n = front;
    for(int i = 0 ; i < numEntries ; i++)
    {
        if(n->db->serverName() == server)
        {
            return n->db;
        }
        n = n->next;
    }
    return nullptr;
}
DbServer *DbServerList::searchServerDatabaseElement(const Glib::ustring& server, const Glib::ustring& database)
{
    node *n = front;
    for(int i = 0 ; i < numEntries ; i++)
    {
        if(n->db->serverName() == server && n->db->databaseName() == database)
        {
            return n->db;
        }
        n = n->next;
    }
    return nullptr;
}

int DbServerList::getServerDatabaseIndex(const Glib::ustring& c)
{
    node *n = front;
    for(int i = 0 ; i < numEntries ; i++)
    {
        std::stringstream ss;
        ss << n->db->serverName() << "." << n->db->databaseName();
        if(ss.str() == c)
        {
            return i;
        }
        n = n->next;
    }
    return -1;
}

int DbServerList::count(void)
{
    return numEntries;
}

DbServer *DbServerList::operator[](int position)
{
    node *n = front;
    for(int i = 0 ; i < numEntries ; i++)
    {
        if(i == position)
        {
            return n->db;
        }
        n = n->next;
    }
    return nullptr;
}

void DbServerList::clear(void)
{
    node* n = front;  
    node* next;  
  
    while (n != nullptr)  
    {  
        next = n->next;  
        delete n->db;
        delete n;
        n = next;  
    }  
    numEntries = 0;
    front = end = nullptr;  
}

