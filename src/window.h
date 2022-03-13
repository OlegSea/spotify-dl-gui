//
// Created by olegsea on 3/13/22.
//

#ifndef SPOTIFY_DL_GUI_WINDOW_H
#define SPOTIFY_DL_GUI_WINDOW_H


#include <gtkmm.h>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <thread>
#include <iostream>
#include <fstream>
#include "downloader.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

class MyWindow : public Gtk::Window {
public:
    MyWindow();

    ~MyWindow() override;

    void notify();

protected:
    void on_notification_from_downloader_thread(), update_start_button();

    void on_directory_button_clicked(), on_link_button_clicked(), on_start_button_clicked();

    void on_checkbox_checked(Gtk::CheckButton *checkbox, bool *flag);

    void on_directory_dialog_response(int response_id, Gtk::FileChooserDialog *dialog);

    bool flags_k = false, flags_m = false, flags_w = false, flags_V = false;

    Glib::ustring config_path;

    std::vector<Glib::ustring> links;

    void update_links(std::vector<Glib::ustring> _links);

    std::vector<Glib::ustring> output;

    void update_output(std::vector<Glib::ustring> _output);

    Gtk::Box m_main_box, m_client_id_box, m_client_secret_box, m_directory_box, m_links_box;
    Gtk::Grid m_checkboxes_grid;

    Gtk::Label m_client_id_label, m_client_secret_label, m_directory_label, m_link_label;

    Gtk::ScrolledWindow m_links_window, m_output_window;
    Gtk::TextView m_links_text, m_output_text;
    Glib::RefPtr<Gtk::TextBuffer> m_links_buffer, m_output_buffer;

    Gtk::Button m_directory_button, m_links_button, m_start_button;
    Gtk::Entry m_client_id_entry, m_client_secret_entry, m_directory_entry, m_links_entry;
    Gtk::CheckButton checkbox_k, checkbox_m, checkbox_w, checkbox_V;

    Glib::Dispatcher m_dispatcher;
    Downloader m_downloader;
    std::thread *m_downloader_thread;
};

#endif //SPOTIFY_DL_GUI_WINDOW_H
