#include "DbMessage.h"

DbMessage::DbMessage(void)
{
    dialog = new Gtk::Dialog();
    dialog->set_size_request(500,40);
    dialog->set_resizable(false);
    dialog->set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    dialog->set_decorated(false);
    dialog->get_vbox()->pack_start(lblComment, Gtk::PACK_SHRINK);
    dialog->set_skip_taskbar_hint(true);
    dialog->show_all_children();
}

DbMessage::~DbMessage(void)
{
    delete dialog;
}

void DbMessage::show(DbStudio& owner, Glib::ustring comment)
{
    dialog->set_transient_for((Gtk::Window&) owner);
    dialog->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    lblComment.set_markup("<span foreground=\"blue\" size=\"x-large\"><b>" + comment + "</b></span>");
    dialog->show();
    dialog->present();
}

void DbMessage::hide(void)
{
    dialog->hide();
}

