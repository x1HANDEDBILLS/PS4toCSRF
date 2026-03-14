// LOCATION: /home/user/ACE/include/theme/theme.h
#ifndef THEME_H
#define THEME_H
#include <QObject>
#include <QString>
#include <QColor>
#include <vector>

struct ThemeConfig {
    QString name;
    QString hex;
    QColor primary;
    QColor secondary;
    QColor bg_light;
    QColor bg_dark;
};

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();
    void next();
    void set_theme(const QString &name);
    const ThemeConfig& active() const { return active_theme; }

signals:
    void themeChanged();

private:
    ThemeManager();
    void init();
    std::vector<ThemeConfig> themes;
    ThemeConfig active_theme;
};
#endif
