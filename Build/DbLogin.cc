#include "DbLogin.h"

DbLogin::DbLogin(void) :
lblDbname(),
txtDbname(),
lblUser(),
txtUser(),
lblPassword(),
txtPassword(),
lblHostaddr(),
txtHostaddr(),
lblPort(),
txtPort()
{
    lblDbname.set_text_with_mnemonic("_Database: ");
    txtDbname.set_max_length(50);
    lblUser.set_text_with_mnemonic("_Username: ");
    txtUser.set_max_length(50);
    lblPassword.set_text_with_mnemonic("_Password: ");
    txtPassword.set_max_length(50);
    txtPassword.set_invisible_char('*');
    txtPassword.set_visibility(false);
    lblHostaddr.set_text_with_mnemonic("_Host Address: ");
    txtHostaddr.set_max_length(50);
    txtHostaddr.set_text("127.0.0.1");
    lblPort.set_text_with_mnemonic("_Port: ");
    txtPort.set_max_length(50); 
    txtPort.set_text("5432");
    btnConnect.set_label("Connect");
    BtnQuit.set_label("Cancel");
    
    Glib::RefPtr<Gtk::CssProvider> css_connect = Gtk::CssProvider::create();
    css_connect->load_from_data("button {background-image: image(cyan);}\
                                 button:hover {background-image: image(green); color: white;}\
                                 button:focus {background-image: image(green); color: white;}\
                                 button:active {background-image: image(brown); color: white;}");
    btnConnect.get_style_context()->add_provider(css_connect, GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    Glib::RefPtr<Gtk::CssProvider> css_quit = Gtk::CssProvider::create();
    css_quit->load_from_data("button {background-image: image(salmon);}\
                              button:hover {background-image: image(red); color: white;}\
                              button:focus {background-image: image(red); color: white;}\
                              button:active {background-image: image(brown); color: white;}");
    BtnQuit.get_style_context()->add_provider(css_quit, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

DbLogin::~DbLogin(void)
{
}

bool DbLogin::getLogin(DbStudio& p)
{
    dialog = new Gtk::Dialog();

    Gtk::Grid m_grid;

    txtDbname.set_activates_default(true);

    // left, top, width, height
    m_grid.attach(lblDbname, 0, 0, 1, 1);
    m_grid.attach(txtDbname, 1, 0, 3, 1);
    m_grid.attach(lblUser, 0, 1, 1, 1);
    m_grid.attach(txtUser, 1, 1, 3, 1);
    m_grid.attach(lblPassword, 0, 2, 1, 1);
    m_grid.attach(txtPassword, 1, 2, 3, 1);
    m_grid.attach(lblHostaddr, 0, 3, 1, 1);
    m_grid.attach(txtHostaddr, 1, 3, 3, 1);
    m_grid.attach(lblPort, 0, 4, 1, 1);
    m_grid.attach(txtPort, 1, 4, 3, 1);

    m_grid.attach(btnConnect,1, 5, 1, 1);
    m_grid.attach(BtnQuit,   2, 5, 1, 1);

    btnConnect.signal_clicked().connect(sigc::mem_fun(*this, &DbLogin::on_connect));
    BtnQuit.signal_clicked().connect(sigc::mem_fun(*this, &DbLogin::on_connect_quit));

    dialog->get_vbox()->pack_start(m_grid, true, true, 0);

    dialog->set_transient_for((Gtk::Window&)p);
    dialog->set_title("Open Database Connection");
    dialog->set_size_request(300,200);
    dialog->set_resizable(false);
    dialog->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);
    dialog->set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    dialog->set_skip_taskbar_hint(true);
    dialog->show_all_children();

    dialog->run();
    delete dialog;

    if(!(dbname.empty() || user.empty() || password.empty() || hostaddr.empty() || port.empty()))
    {
        connParam = "dbname = <dbName> user = " + user + " password = " + password + " hostaddr = " + hostaddr + " port = " + port;
        return true;
    }
    return false;
}

void DbLogin::on_connect_quit(void)
{
    dialog->hide();
}

void DbLogin::on_connect(void)
{
    dbname = txtDbname.get_text();
    user = txtUser.get_text();
    password = txtPassword.get_text();
    hostaddr = txtHostaddr.get_text();
    port = txtPort.get_text();
    dialog->hide();
}
