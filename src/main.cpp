#include <gtkmm.h>
#include "window.h"


int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.olegsea.spotify_dl_gui");

    return app->make_window_and_run<MyWindow>(argc, argv);
}
