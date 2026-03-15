#include "OverlayWindow.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <cmath>
#include <random>

namespace AuraFlow {

OverlayWindow::OverlayWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    m_color = QColor("#4B9AFF");
    connect(&m_animTimer, &QTimer::timeout, this, &OverlayWindow::animate);

    setFixedSize(240, 52);
    centerOnScreen();
    hide();
}

void OverlayWindow::centerOnScreen() {
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeom = screen->geometry();
    move((screenGeom.width() - width()) / 2, screenGeom.height() - 110);
}

void OverlayWindow::showRecording(const QString& label) {
    m_mode = "recording";
    m_label = label;
    m_color = QColor("#FF4B4B");
    m_tick = 0;
    m_animTimer.start(40);
    show();
    raise();
}

void OverlayWindow::showProcessing() {
    m_mode = "processing";
    m_label = "Processing...";
    m_color = QColor("#4B9AFF");
    m_animTimer.start(40);
    show();
    raise();
}

void OverlayWindow::showLaunching(const QString& label) {
    m_mode = "processing"; // Reuse processing dots for launching
    m_label = label;
    m_color = QColor("#4B9AFF");
    m_animTimer.start(40);
    show();
    raise();
    QApplication::processEvents(); // Force show immediately
}

void OverlayWindow::hideIndicator() {
    m_mode = "idle";
    m_animTimer.stop();
    hide();
}

void OverlayWindow::animate() {
    m_tick++;
    if (m_tick % 2 == 0) {
        for (int i = 0; i < 5; ++i) {
            m_targetHeights[i] = 0.1f + (float)rand() / RAND_MAX * 0.9f;
        }
    }
    for (int i = 0; i < 5; ++i) {
        m_barHeights[i] += (m_targetHeights[i] - m_barHeights[i]) * 0.4f;
    }
    update();
}

void OverlayWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    QPainterPath bg;
    bg.addRoundedRect(QRectF(0, 0, w, h), h / 2.0, h / 2.0);

    QLinearGradient grad(0, 0, w, 0);
    grad.setColorAt(0, QColor(15, 15, 20, 245));
    grad.setColorAt(1, QColor(25, 25, 35, 245));
    p.fillPath(bg, grad);

    p.setPen(QPen(QColor(m_color.red(), m_color.green(), m_color.blue(), 100), 1.5));
    p.drawPath(bg);

    if (m_mode == "recording") {
        // Pulsing Dot
        float pulse = 0.8f + 0.2f * std::sin(m_tick * 0.15f);
        float dotR = 6.0f * pulse;
        p.setBrush(QColor(255, 75, 75, (int)(255 * pulse)));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QRectF(20 - dotR, h / 2.0 - dotR, dotR * 2, dotR * 2));

        // Waveform
        float bw = 3.0, gap = 4.0;
        float bx = 40;
        float maxBH = 20;
        for (int i = 0; i < 5; ++i) {
            float bhPix = std::max(4.0f, m_barHeights[i] * maxBH);
            p.setBrush(QColor(255, 255, 255, 180));
            p.drawRoundedRect(QRectF(bx + i * (bw + gap), (h - bhPix) / 2.0, bw, bhPix), bw / 2.0, bw / 2.0);
        }

        p.setPen(QColor(255, 255, 255, 240));
        p.setFont(QFont("Segoe UI", 11, QFont::DemiBold));
        p.drawText(QRectF(85, 0, w - 95, h), Qt::AlignVCenter, m_label);
    } else if (m_mode == "processing") {
        p.setPen(Qt::NoPen);
        for (int i = 0; i < 3; ++i) {
            float angle = m_tick * 0.15f + i * 2.1f;
            float x = 22 + std::cos(angle) * 7;
            float y = h / 2.0 + std::sin(angle) * 7;
            p.setBrush(QColor(75, 154, 255, 200));
            p.drawEllipse(QRectF(x - 3, y - 3, 6, 6));
        }
        p.setPen(QColor(255, 255, 255, 220));
        p.setFont(QFont("Segoe UI", 11, QFont::DemiBold));
        p.drawText(QRectF(46, 0, w - 56, h), Qt::AlignVCenter, m_label);
    }
}

} // namespace AuraFlow
