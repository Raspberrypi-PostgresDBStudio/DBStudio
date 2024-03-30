#ifndef DBPROGRESS_H
#define DBPROGRESS_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>

class DbStudio;

class DbMessage
{
    private:
        Gtk::Dialog *dialog;
        Gtk::Label lblComment;
        
    public:
        DbMessage(void);
        virtual ~DbMessage(void);
        void show(DbStudio& owner, Glib::ustring comment);
        void hide(void);
};
#endif
