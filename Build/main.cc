#include <gtkmm/main.h>
#include "DbStudio.h"

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
    DbStudio window;
    Gtk::Main::run(window);

    return 0;
}

