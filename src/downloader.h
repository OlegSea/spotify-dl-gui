//
// Created by olegsea on 3/13/22.
//

#ifndef SPOTIFY_DL_GUI_DOWNLOADER_H
#define SPOTIFY_DL_GUI_DOWNLOADER_H

#include "exec-stream.h"
#include <gtkmm.h>
#include <thread>
#include <mutex>

class MyWindow;

class Downloader {
public:
    Downloader();

    void download(MyWindow *caller, const Glib::ustring &args, const Glib::ustring &albums);

    void get_data(Glib::ustring *message);

    void stop_downloading();

    bool has_stopped() const;

private:
    mutable std::mutex m_Mutex;
    bool m_shall_stop;
    bool m_has_stopped;
    Glib::ustring m_message;
};

#endif //SPOTIFY_DL_GUI_DOWNLOADER_H
