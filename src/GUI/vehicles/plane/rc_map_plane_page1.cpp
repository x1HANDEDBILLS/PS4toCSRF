// LOCATION: /home/user/ACE/src/GUI/vehicles/plane/rc_map_plane_page1.cpp
#include "vehicles/plane/rc_map_plane_page1.h"
#include "theme/theme.h"
#include "Core/app_state.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QDialog>

// A grid to select the hardware ID (0-22)
class SourceSelector : public QDialog {
    Q_OBJECT
public:
    SourceSelector(int current, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
        setStyleSheet("background: #0a0a0f; border: 2px solid #444;");
        auto* grid = new QGridLayout(this);
        
        for(int i = 0; i <= 23; ++i) {
            QString label = (i == 22) ? "NONE" : QString("ID %1").arg(i, 2, 10, QChar('0'));
            auto* btn = new QPushButton(label);
            btn->setFixedSize(80, 45);
            QString color = (i == current) ? "#00ffff" : "#ccc";
            btn->setStyleSheet(QString("background: #2d2d37; color: %1; border-radius: 5px; font-family: 'Consolas';").arg(color));
            
            connect(btn, &QPushButton::clicked, [this, i]() {
                done(i);
            });
            grid->addWidget(btn, i / 5, i % 5);
        }
    }
};

class MapperRow : public QWidget {
    Q_OBJECT
public:
    MapperRow(int ch_idx, QWidget* parent) : QWidget(parent), ch(ch_idx) {
        auto* l = new QHBoxLayout(this);
        QString h = ThemeManager::instance().active().hex;

        auto* name = new QLabel(QString("CH %1:").arg(ch + 1, 2, 10, QChar('0')));
        name->setStyleSheet("font-family: 'Consolas'; color: #888; font-size: 18px;");

        id_btn = new QPushButton();
        updateButton();
        id_btn->setFixedSize(140, 35);
        
        connect(id_btn, &QPushButton::clicked, this, &MapperRow::pickSource);

        l->addWidget(name);
        l->addStretch();
        l->addWidget(id_btn);
    }

    void updateButton() {
        int source = AppState::instance().channel_map[ch];
        QString h = ThemeManager::instance().active().hex;
        QString txt = (source == 22) ? "NONE" : QString("ID %1").arg(source, 2, 10, QChar('0'));
        id_btn->setText(txt);
        id_btn->setStyleSheet(QString("background:#32343c; color:%1; border:1px solid %1; border-radius:8px; font-weight:bold;").arg(h));
    }

private slots:
    void pickSource() {
        SourceSelector sel(AppState::instance().channel_map[ch], this);
        int result = sel.exec();
        if (result >= 0) {
            AppState::instance().channel_map[ch] = result;
            updateButton();
        }
    }

private:
    int ch;
    QPushButton* id_btn;
};

void setup_rc_map_plane_page1_content(QWidget* p) {
    auto* layout = new QVBoxLayout(p);
    layout->setContentsMargins(40, 20, 40, 20);
    
    // Create rows for CH 1-8 (Page 1)
    for(int i = 0; i < 8; ++i) {
        layout->addWidget(new MapperRow(i, p));
    }
    layout->addStretch();
}

#include "rc_map_plane_page1.moc"
