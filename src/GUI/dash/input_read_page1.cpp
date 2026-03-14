#include "dash/input_read_page1.h"
#include "input/input_manage.h"
#include "theme/theme.h"
#include <QTimer>
#include <QPainter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

namespace {
    class NeonBar : public QWidget {
    public:
        QString label;
        float val = 0;
        int mode = 0; // 0: Axis, 1: Gyro, 2: Accel
        NeonBar(QWidget* p) : QWidget(p) { setMinimumHeight(35); }
        void updateValue(float v) { if(val != v) { val = v; update(); } }
    protected:
        void paintEvent(QPaintEvent*) override {
            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            QColor theme = ThemeManager::instance().active().primary;
            p.setBrush(QColor(15, 15, 15));
            p.setPen(QPen(QColor(45, 45, 45), 1));
            p.drawRoundedRect(rect().adjusted(1,1,-1,-1), 4, 4);
            
            float norm = 0.5f;
            if (mode == 0) norm = (val + 32768.0f) / 65535.0f;
            else if (mode == 1) norm = (val + 20.0f) / 40.0f;
            else norm = (val + 10.0f) / 20.0f;
            
            int xPos = 2 + static_cast<int>(qBound(0.0f, norm, 1.0f) * (width() - 14));
            p.setBrush(theme);
            p.setPen(Qt::NoPen);
            p.drawRect(xPos, 4, 10, height() - 8);
            
            p.setPen(Qt::white);
            p.setFont(QFont("Consolas", 10, QFont::Bold));
            p.drawText(15, 0, width()-30, height(), Qt::AlignVCenter, 
                       label + ": " + QString::number(val, 'f', (mode == 0 ? 0 : 2)));
        }
    };

    QLabel* createZoneHeader(const QString& txt, const QColor& color) {
        QLabel* l = new QLabel(txt);
        l->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 12px; margin-top: 8px; padding-bottom: 2px; border-bottom: 1px solid #333;").arg(color.name()));
        return l;
    }
}

void setup_input_read_page1_content(QWidget* p) {
    auto* main_layout = new QVBoxLayout(p);
    main_layout->setContentsMargins(30, 30, 30, 30);
    main_layout->setSpacing(15);

    QLabel* status = new QLabel();
    status->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(status);

    auto* scroll = new QScrollArea(p);
    scroll->setWidgetResizable(true);
    
    QString scrollStyle = QString(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { width: 40px; background: #111; }"
        "QScrollBar::handle:vertical { background: %1; min-height: 40px; border-radius: 5px; }"
    ).arg(ThemeManager::instance().active().primary.name());
    scroll->setStyleSheet(scrollStyle);
    
    auto* container = new QWidget();
    auto* v_box = new QVBoxLayout(container);
    v_box->setContentsMargins(0, 0, 10, 0);
    v_box->setSpacing(8);
    
    QColor theme = ThemeManager::instance().active().primary;

    v_box->addWidget(createZoneHeader("ANALOG CONTROLS", theme));
    std::vector<NeonBar*> axis_bars;
    for(int i=0; i<6; i++) {
        auto* b = new NeonBar(container);
        b->label = "AXIS " + QString::number(i);
        b->mode = 0;
        v_box->addWidget(b);
        axis_bars.push_back(b);
    }

    v_box->addWidget(createZoneHeader("GYROSCOPE DATA", theme));
    std::vector<NeonBar*> gyro_bars;
    QStringList g_labs = {"PITCH", "YAW", "ROLL"};
    for(const auto& s : g_labs) {
        auto* b = new NeonBar(container);
        b->label = s; b->mode = 1;
        v_box->addWidget(b);
        gyro_bars.push_back(b);
    }

    v_box->addWidget(createZoneHeader("ACCELEROMETER", theme));
    std::vector<NeonBar*> acc_bars;
    QStringList a_labs = {"ACCEL X", "ACCEL Y", "ACCEL Z"};
    for(const auto& s : a_labs) {
        auto* b = new NeonBar(container);
        b->label = s; b->mode = 2;
        v_box->addWidget(b);
        acc_bars.push_back(b);
    }

    v_box->addWidget(createZoneHeader("DIGITAL INPUTS", theme));
    auto* grid_w = new QWidget();
    auto* grid = new QGridLayout(grid_w);
    std::vector<QLabel*> btns;
    for(int j=0; j<32; j++) {
        auto* l = new QLabel(QString::number(j));
        l->setFixedSize(35, 35);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("background:#111; color:#555; border:1px solid #222;");
        grid->addWidget(l, j/8, j%8);
        btns.push_back(l);
    }
    v_box->addWidget(grid_w);
    
    scroll->setWidget(container);
    main_layout->addWidget(scroll);

    QTimer* t = new QTimer(p);
    QObject::connect(t, &QTimer::timeout, [=]() {
        DeviceState dev = InputManager::instance().getSlotState(0);
        if(dev.connected) {
            status->setText(QString::fromStdString(dev.name).toUpper());
            status->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(theme.name()));
            for(int j=0; j<6; j++) axis_bars[j]->updateValue(dev.axes[j]);
            for(int j=0; j<3; j++) gyro_bars[j]->updateValue(dev.gyro[j]);
            for(int j=0; j<3; j++) acc_bars[j]->updateValue(dev.accel[j]);
            for(int j=0; j<32; j++) {
                btns[j]->setStyleSheet(dev.buttons[j] ? 
                    QString("background:%1; color:black; border:1px solid white;").arg(theme.name()) : 
                    "background:#111; color:#555; border:1px solid #222;");
            }
        } else {
            status->setText("PORT 1: DISCONNECTED");
            status->setStyleSheet("color: #ff4444; font-size: 18px; font-weight: bold;");
        }
    });
    t->start(16);
}
