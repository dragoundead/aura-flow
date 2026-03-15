#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QColor>

namespace AuraFlow {

class OverlayWindow : public QWidget {
    Q_OBJECT
public:
    explicit OverlayWindow(QWidget *parent = nullptr);

    Q_INVOKABLE void showRecording(const QString& label = "Recording");
    Q_INVOKABLE void showProcessing();
    Q_INVOKABLE void showLaunching(const QString& label = "Aura Flow: Launching...");
    Q_INVOKABLE void hideIndicator();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void animate();
    void centerOnScreen();

    QString m_mode = "idle";
    QString m_label;
    QColor m_color;
    int m_tick = 0;
    QTimer m_animTimer;
    float m_barHeights[5] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
    float m_targetHeights[5] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
};

} // namespace AuraFlow

#endif // OVERLAY_WINDOW_H
