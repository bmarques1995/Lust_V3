// MyWindowFollower.h
#pragma once

#include <QDockWidget>
#include <QWindow>
#include <memory>

class GameContainer : public QDockWidget {
public:
    GameContainer(std::shared_ptr<QWindow> targetWindow, QWidget* parent = nullptr);
	~GameContainer() = default;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void updateFromWindow();

private:
    std::shared_ptr<QWindow> m_Window;
};