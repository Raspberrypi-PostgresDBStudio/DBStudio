#include "DbTreeview.h"

DbTreeView::DbTreeView(void)
{
}

DbTreeView::~DbTreeView(void)
{
}

sigc::signal<void, GdkEventButton*> DbTreeView::signal_on_button_click()
{
    return m_signal_click;
}

bool DbTreeView::on_button_press_event(GdkEventButton* button_event)
{
    bool return_value = false;

    return_value = TreeView::on_button_press_event(button_event);

    if( (button_event->type == GDK_BUTTON_PRESS) && (button_event->button == 3) )
    {
        m_signal_click.emit(button_event);
    }
    return return_value;
}
