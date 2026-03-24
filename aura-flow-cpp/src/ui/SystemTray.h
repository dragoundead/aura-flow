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
    void setModelCallback(std::function<void(QString)> onModelChange);
    void setHardwareCallback(std::function<void(QString)> onHardwareChange);
    void addModelOption(const QString& name, bool checked = false);
    void showReadyMessage();
    void showStartingMessage();

private:
    Q_ENUMS(Mode)
    void createMenu();

    QMenu *m_menu;
    QAction *m_translateAct;
    QAction *m_transcribeAct;
    QAction *m_postAct;
    QAction *m_exitAct;

    QMenu *m_modelMenu;
    QActionGroup *m_modelGroup;
    
    QMenu *m_hwMenu;
    QActionGroup *m_hwGroup;
    QAction *m_hwGpuAct;
    QAction *m_hwCpuAct;

    std::function<void(QString)> m_onModeChange;
    std::function<void(QString)> m_onModelChange;
    std::function<void(QString)> m_onHardwareChange;
};

} // namespace AuraFlow

#endif // SYSTEM_TRAY_H
