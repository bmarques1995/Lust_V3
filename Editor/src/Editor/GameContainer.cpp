#include "GameContainer.hpp"
#include <QTimer>
#include <QEvent>

GameContainer::GameContainer(std::shared_ptr<QWindow> targetWindow, QWidget* parent)
    : QDockWidget(parent), m_Window(targetWindow)
{
    setWindowTitle("Dock Following QWindow");
    setAllowedAreas(Qt::NoDockWidgetArea); // Undocked by design
    setFeatures(QDockWidget::NoDockWidgetFeatures); // Prevent docking interaction

    if (m_Window) {
        m_Window->installEventFilter(this);
        updateFromWindow();
    }
}

void GameContainer::updateFromWindow()
{
    if (!m_Window)
        return;

    QRect geom = m_Window->geometry();
    QWidget* fw = this->window(); // QMainWindow usually
    if (fw) {
        // Reposition relative to parent if needed
        this->setGeometry(fw->mapFromGlobal(geom.topLeft()).x(),
            fw->mapFromGlobal(geom.topLeft()).y(),
            geom.width(),
            geom.height());
    }
}

bool GameContainer::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_Window.get()) {
        updateFromWindow();
    }

    return QDockWidget::eventFilter(obj, event);
}