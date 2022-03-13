//
// Created by olegsea on 3/13/22.
//

#include "window.h"

MyWindow::MyWindow() : m_main_box(Gtk::Orientation::VERTICAL), m_client_id_label("Client ID"),
                       m_client_secret_label("Client Secret"), m_directory_label("Output Directory"),
                       m_link_label("Link"), m_directory_button("Select"), m_links_button("Add"),
                       m_start_button("Start"),
                       m_downloader(), m_downloader_thread(nullptr) {
    m_main_box.set_margin(10);
    set_child(m_main_box);


    m_main_box.append(m_client_id_box);
    m_main_box.append(m_client_secret_box);
    m_main_box.append(m_directory_box);
    m_main_box.append(m_links_box);
    m_main_box.append(m_links_window);
    m_main_box.append(m_checkboxes_grid);
    m_main_box.append(m_start_button);
    m_main_box.append(m_output_window);

    m_client_id_entry.set_margin(10);
    m_client_id_entry.set_hexpand();
    m_client_id_label.set_margin(10);
    m_client_id_box.append(m_client_id_entry);
    m_client_id_box.append(m_client_id_label);

    m_client_secret_entry.set_margin(10);
    m_client_secret_entry.set_hexpand();
    m_client_secret_entry.set_visibility(false);
    m_client_secret_label.set_margin(10);
    m_client_secret_box.append(m_client_secret_entry);
    m_client_secret_box.append(m_client_secret_label);

    m_directory_entry.set_margin(10);
    m_directory_entry.set_hexpand();
    m_directory_entry.set_text(std::filesystem::current_path().u8string());
    m_directory_button.set_margin(10);
    m_directory_button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_directory_button_clicked));
    m_directory_box.append(m_directory_entry);
    m_directory_box.append(m_directory_button);
    m_directory_box.append(m_directory_label);

    m_links_entry.set_margin(10);
    m_links_entry.set_hexpand();
    m_links_button.set_margin(10);
    m_links_button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_link_button_clicked));
    m_links_box.append(m_links_entry);
    m_links_box.append(m_links_button);
    m_links_box.append(m_link_label);

    m_links_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_links_window.set_expand();
    m_links_window.set_margin(10);
    m_links_window.set_child(m_links_text);
    m_links_buffer = Gtk::TextBuffer::create();
    m_links_text.set_editable(false);
    m_links_text.set_buffer(m_links_buffer);

    checkbox_k.set_margin(10);
    checkbox_k.set_label("Keep original playlist ordering");
    checkbox_k.signal_toggled().connect(
            sigc::bind(sigc::mem_fun(*this, &MyWindow::on_checkbox_checked), &checkbox_k, &flags_k));
    m_checkboxes_grid.attach(checkbox_k, 0, 0);
    checkbox_m.set_margin(10);
    checkbox_m.set_label("Don't convert downloaded songs to mp3");
    checkbox_m.signal_toggled().connect(
            sigc::bind(sigc::mem_fun(*this, &MyWindow::on_checkbox_checked), &checkbox_m, &flags_m));
    m_checkboxes_grid.attach(checkbox_m, 0, 1);
    checkbox_w.set_margin(10);
    checkbox_w.set_label("Avoid overwriting existing files");
    checkbox_w.signal_toggled().connect(
            sigc::bind(sigc::mem_fun(*this, &MyWindow::on_checkbox_checked), &checkbox_w, &flags_w));
    m_checkboxes_grid.attach(checkbox_w, 1, 0);
    checkbox_V.set_margin(10);
    checkbox_V.set_label("Verbose mode");
    checkbox_V.signal_toggled().connect(
            sigc::bind(sigc::mem_fun(*this, &MyWindow::on_checkbox_checked), &checkbox_V, &flags_V));
    m_checkboxes_grid.attach(checkbox_V, 1, 1);

    m_start_button.set_margin(10);
    m_start_button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_start_button_clicked));

    m_output_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_output_window.set_margin(10);
    m_output_window.set_expand();
    m_output_window.set_child(m_output_text);
    m_output_buffer = Gtk::TextBuffer::create();
    m_output_text.set_editable(false);
    m_output_text.set_buffer(m_output_buffer);

    set_title("spotify-dl-gui");
    set_default_size(800, 600);

    m_dispatcher.connect(sigc::mem_fun(*this, &MyWindow::on_notification_from_downloader_thread));

#ifdef _WIN32
    config_path = "credentials.conf";
#else
    Glib::ustring home = getenv("HOME");
    config_path = home + "/.config/spotify_dl_gui.conf";
#endif

    std::string line;
    std::ifstream config_file(config_path);
    if (config_file.is_open()) {
        std::string id;
        std::string secret;
        std::getline(config_file, id);
        std::getline(config_file, secret);
        m_client_id_entry.set_text(id);
        m_client_secret_entry.set_text(secret);
        config_file.close();
    } else
        std::cout << config_path << "\n";
}

MyWindow::~MyWindow()
= default;

void MyWindow::on_directory_button_clicked() {
    auto dialog = new Gtk::FileChooserDialog("Please choose a directory",
                                             Gtk::FileChooser::Action::SELECT_FOLDER);
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->signal_response().connect(sigc::bind(
            sigc::mem_fun(*this, &MyWindow::on_directory_dialog_response), dialog));

    // Add response buttons to the dialog:
    dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    dialog->add_button("Select", Gtk::ResponseType::OK);

    // Show the dialog and wait for a user response:
    dialog->show();
}

void MyWindow::on_directory_dialog_response(int response_id, Gtk::FileChooserDialog *dialog) {
    // Handle the response:
    switch (response_id) {
        case Gtk::ResponseType::OK: {
            m_directory_entry.set_text(dialog->get_file()->get_path());
            break;
        }
        case Gtk::ResponseType::CANCEL: {
            break;
        }
        default: {
            std::cout << "Unexpected button clicked." << std::endl;
            break;
        }
    }
    delete dialog;
}

void MyWindow::update_links() {
    Glib::ustring _result;
    for (const auto& i: links) {
        _result.append(i);
        _result.append("\n");
    }
    m_links_buffer->set_text(_result);
}

void MyWindow::on_link_button_clicked() {
    links.push_back(m_links_entry.get_text());
    update_links();
    m_links_entry.set_text("");
}

void MyWindow::on_checkbox_checked(Gtk::CheckButton *checkbox, bool *flag) {
    *flag = checkbox->get_active();
}

void MyWindow::update_output() {
    Glib::ustring _result;
    for (const auto& i: output) {
        _result.append(i);
        _result.append("\n");
    }
    m_output_buffer->set_text(_result);
}

void MyWindow::on_start_button_clicked() {
    Glib::ustring args;
    if (flags_k)
        args.append("-k ");
    if (flags_m)
        args.append("-m ");
    if (flags_w)
        args.append("-w ");
    if (flags_V)
        args.append("-V ");

    args.append("-o " + m_directory_entry.get_text() + " ");

    if (!m_downloader_thread) {
        std::string env = m_client_id_entry.get_text();
        char *env_cstr = &env[0];
        setenv("SPOTIPY_CLIENT_ID", env_cstr, 0);

        env = m_client_secret_entry.get_text();
        env_cstr = &env[0];
        setenv("SPOTIPY_CLIENT_SECRET", env_cstr, 0);

        std::ofstream config_file;
        config_file.open(config_path, std::ofstream::trunc);
        config_file << m_client_id_entry.get_text() << "\n" << m_client_secret_entry.get_text();
        config_file.close();

        output.clear();
        update_output();

        Glib::ustring _links = "-l ";
        for (const auto& i: links) {
            _links.append(i);
            _links.append(" ");
        }

        m_downloader_thread = new std::thread(
                [this, args, _links] {
                    m_downloader.download(this, args, _links);
                });
        update_start_button();
    }
}

void MyWindow::notify() {
    m_dispatcher.emit();
}

void MyWindow::on_notification_from_downloader_thread() {
    if (m_downloader_thread && m_downloader.has_stopped()) {
        // Work is done.
        if (m_downloader_thread->joinable())
            m_downloader_thread->join();
        delete m_downloader_thread;
        m_downloader_thread = nullptr;
        update_start_button();
    }

    Glib::ustring msg;
    m_downloader.get_data(&msg);
    output.push_back(msg);
    update_output();
}

void MyWindow::update_start_button() {
    const bool thread_is_running = m_downloader_thread != nullptr;
    m_start_button.set_sensitive(!thread_is_running);
}
