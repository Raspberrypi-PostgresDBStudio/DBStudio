#ifndef DBTREEVIEW_H
#define DBTREEVIEW_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>

class DbStudio;

class DbTreeView : public Gtk::TreeView
{
    private:
        bool on_button_press_event(GdkEventButton* button_event) override;
        sigc::signal<void, GdkEventButton*> m_signal_click;
        
    public:
        DbTreeView(void);
        virtual ~DbTreeView(void);
        sigc::signal<void, GdkEventButton*> signal_on_button_click();
};

#endif
