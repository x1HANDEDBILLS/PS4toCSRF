#ifndef APP_STATE_H
#define APP_STATE_H
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class AppState {
public:
    static AppState& instance() { static AppState inst; return inst; }
    
    QString current_profile_name = "DEFAULT";
    QString rc_type = "PLANE";
    QString theme_name = "CYAN";
    float current_expo = 0.0f;
    float deadzones[8] = {0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f};
    bool expo_enabled[8] = {true, true, true, true, true, true, true, true};
    int channel_map[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    QJsonObject toJson() {
        QJsonObject obj;
        obj["profile_name"] = current_profile_name;
        obj["rc_type"] = rc_type;
        obj["theme_name"] = theme_name;
        obj["expo"] = static_cast<double>(current_expo);
        
        QJsonArray dz; for(int i=0; i<8; ++i) dz.append(static_cast<double>(deadzones[i]));
        obj["deadzones"] = dz;
        
        QJsonArray ex; for(int i=0; i<8; ++i) ex.append(expo_enabled[i]);
        obj["expo_enabled_states"] = ex;
        
        QJsonArray map; for(int i=0; i<16; ++i) map.append(channel_map[i]);
        obj["channel_map"] = map;
        return obj;
    }

    void fromJson(const QJsonObject &obj) {
        current_profile_name = obj["profile_name"].toString("DEFAULT");
        rc_type = obj["rc_type"].toString("PLANE");
        theme_name = obj["theme_name"].toString("CYAN");
        current_expo = obj["expo"].toDouble(0.0);
        
        QJsonArray dz = obj["deadzones"].toArray();
        for(int i=0; i<8 && i<dz.size(); ++i) deadzones[i] = dz[i].toDouble(0.05);
        
        if (obj.contains("expo_enabled_states")) {
            QJsonArray ex = obj["expo_enabled_states"].toArray();
            for(int i=0; i<8 && i<ex.size(); ++i) expo_enabled[i] = ex[i].toBool(true);
        }
        
        QJsonArray map = obj["channel_map"].toArray();
        if(!map.isEmpty()) {
            for(int i=0; i<16 && i<map.size(); ++i) channel_map[i] = map[i].toInt();
        }
    }
};
#endif
