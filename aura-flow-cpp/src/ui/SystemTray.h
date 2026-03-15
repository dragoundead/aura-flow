#ifndef SYSTEM_TRAY_H
#define SYSTEM_TRAY_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

namespace AuraFlow {

class SystemTray : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit SystemTray(QObject *parent = nullptr);

    void setModeCallbacks(std::function<void(QString)> onModeChange);

private:
    void createMenu();

    QMenu *m_menu;
    QAction *m_translateAct;
    QAction *m_transcribeAct;
    QAction *m_postAct;
    QAction *m_exitAct;

    std::function<void(QString)> m_onModeChange;
};

} // namespace AuraFlow

#endif // SYSTEM_TRAY_H
