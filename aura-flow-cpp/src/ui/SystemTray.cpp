#include "SystemTray.h"
#include <QApplication>
#include <QActionGroup>
#include <QPainter>

namespace AuraFlow {

SystemTray::SystemTray(QObject *parent) : QSystemTrayIcon(parent) {
    // Generate a simple blue dot icon
    QPixmap pix(64, 64);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor("#4B9AFF"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(10, 10, 44, 44);
    p.end();

    setIcon(QIcon(pix));
    setToolTip("Aura Flow (C++)");

    createMenu();
}

void SystemTray::createMenu() {
    m_menu = new QMenu();

    QMenu *modeMenu = m_menu->addMenu("Mode");
    m_translateAct = new QAction("Translate to English", this);
    m_translateAct->setCheckable(true);

    m_transcribeAct = new QAction("Transcribe (Original)", this);
    m_transcribeAct->setCheckable(true);
    m_transcribeAct->setChecked(true); // Default to Transcribe

    m_postAct = new QAction("📝 Post for Telegram", this);
    m_postAct->setCheckable(true);

    QActionGroup *group = new QActionGroup(this);
    group->addAction(m_translateAct);
    group->addAction(m_transcribeAct);
    group->addAction(m_postAct);
    group->setExclusive(true);

    modeMenu->addAction(m_translateAct);
    modeMenu->addAction(m_transcribeAct);
    modeMenu->addAction(m_postAct);

    connect(m_translateAct, &QAction::triggered, [this]() { if (m_onModeChange) m_onModeChange("translate"); });
    connect(m_transcribeAct, &QAction::triggered, [this]() { if (m_onModeChange) m_onModeChange("transcribe"); });
    connect(m_postAct, &QAction::triggered, [this]() { if (m_onModeChange) m_onModeChange("post"); });

    m_menu->addSeparator();
    m_exitAct = m_menu->addAction("Exit");
    connect(m_exitAct, &QAction::triggered, qApp, &QApplication::quit);

    setContextMenu(m_menu);
}

void SystemTray::setModeCallbacks(std::function<void(QString)> onModeChange) {
    m_onModeChange = onModeChange;
}

void SystemTray::showStartingMessage() {
    showMessage("Aura Flow", "Запуск... Загрузка модели ИИ.", QSystemTrayIcon::Information, 2000);
}

void SystemTray::showReadyMessage() {
    showMessage("Aura Flow", "Готов! Режим: Оригинал (RU).", QSystemTrayIcon::Information, 3000);
}

} // namespace AuraFlow
