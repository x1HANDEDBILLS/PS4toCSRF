#include "vehicles/plane/main_plane_dash.h"
#include "theme/theme.h"
#include <QVBoxLayout>
MainPlaneDash::MainPlaneDash(QWidget *parent) : QFrame(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *layout = new QVBoxLayout(this);
    header_label = new QLabel("// PLANE ACTIVE", this);
    telemetry_placeholder = new QLabel("FLIGHT HUD: HORIZON & ALTIMETER", this);
    header_label->setAlignment(Qt::AlignCenter);
    telemetry_placeholder->setAlignment(Qt::AlignCenter);
    layout->addStretch(); layout->addWidget(header_label);
    layout->addWidget(telemetry_placeholder); layout->addStretch();
    refresh_theme();
}
void MainPlaneDash::refresh_theme() {
    QString silver = ThemeManager::instance().active().secondary.name();
    QString accent = ThemeManager::instance().active().hex;
    header_label->setStyleSheet(QString("color: %1; font-family: 'Consolas'; font-size: 22px; font-weight: bold;").arg(silver));
    telemetry_placeholder->setStyleSheet(QString("color: %1; font-family: 'Consolas'; font-size: 30px; border: 2px solid %2; padding: 20px; background: rgba(0, 0, 0, 40);").arg(silver, accent));
}
MainPlaneDash::~MainPlaneDash() {}
