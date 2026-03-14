// LOCATION: /home/user/ACE/include/vehicles/plane/rc_map_plane_panel.h
#ifndef RC_MAP_PLANE_PANEL_H
#define RC_MAP_PLANE_PANEL_H

#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
#include "widgets/frame.h"
#include "widgets/button.h"

class RCMapPlanePanel : public QFrame {
    Q_OBJECT
public:
    explicit RCMapPlanePanel(QWidget *parent = nullptr);
    ~RCMapPlanePanel();
    void refresh_theme();

private slots:
    void next_page();
    void prev_page();

private:
    void update_page_ui();
    // Functions to build your specific Mapping UI pages
    void setup_sources_page(QWidget* p);
    void setup_mixing_page(QWidget* p);

    Frame *content_frame;
    QLabel *header_title;
    QLabel *status_label;
    QLabel *page_label;
    QStackedWidget *pages;
    Button *prev_btn;
    Button *next_btn;
    QStringList page_titles;
};

#endif