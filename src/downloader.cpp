//
// Created by olegsea on 3/13/22.
//

#include "downloader.h"
#include "window.h"
#include <sstream>
#include <chrono>

Downloader::Downloader() : m_Mutex(), m_shall_stop(false),
                           m_has_stopped(false),
                           m_message("") {
}

void Downloader::get_data(Glib::ustring *message) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (message)
        *message = m_message;
}

void Downloader::stop_downloading() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = true;
}

bool Downloader::has_stopped() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_has_stopped;
}

void Downloader::download(MyWindow *caller, const Glib::ustring &args, const Glib::ustring &albums) {
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_has_stopped = false;
        m_message = "";
    }

    exec_stream_t es;
    try {
        es.start("spotify_dl", args + albums);
        std::string s;
        while (std::getline(es.out(), s).good()) {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_message = s;
            caller->notify();
        }
        while (std::getline(es.err(), s).good()) {
            std::cout << s << "\n";
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_message = s;
            caller->notify();
        }
    }
    catch (std::exception const &e) {
    }

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_shall_stop = false;
        m_has_stopped = true;
    }

    caller->notify();
}