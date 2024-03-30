#ifndef DBLOGIN_H
#define DBLOGIN_H

#include <gtkmm.h>
#include <gtkmm/dialog.h>

class DbStudio;

class DbLogin
{
    private:
        Glib::ustring dbname;
        Glib::ustring user;
        Glib::ustring password; 
        Glib::ustring hostaddr;
        Glib::ustring port;
        Glib::ustring connParam;

        //Widgets
        Gtk::Dialog *dialog;
        Gtk::Label lblDbname;
        Gtk::Entry txtDbname;
        Gtk::Label lblUser;
        Gtk::Entry txtUser;
        Gtk::Label lblPassword;
        Gtk::Entry txtPassword;
        Gtk::Label lblHostaddr;
        Gtk::Entry txtHostaddr;
        Gtk::Label lblPort;
        Gtk::Entry txtPort;
        Gtk::Button btnConnect, BtnQuit;
        
        //Signal handlers: 
        void on_connect_quit(void);
        void on_connect(void);

    public:
        DbLogin(void);
        virtual ~DbLogin(void);
        bool getLogin(DbStudio& p);
        const Glib::ustring& getServerName(void) { return hostaddr; }
        const Glib::ustring& getDatabaseName(void) { return dbname; }
        const Glib::ustring& getUserName(void) { return user; }
        const Glib::ustring& getPassword(void) { return password; }
        const Glib::ustring& getPort(void) { return port; }        
        const Glib::ustring& getConnectionString(void) { return connParam; }
};
#endif
