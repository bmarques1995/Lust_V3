#include "ExampleQtWindow.hpp"
#include <QCloseEvent>
#include <QMessageBox>
#include <windows.h>

ExampleQWindow::ExampleQWindow(WId externalHwnd, QWindow* parent)
    : QWindow(QWindow::fromWinId(externalHwnd)), m_backingStore(nullptr)
{
    setFlags(Qt::FramelessWindowHint);
    setSurfaceType(QSurface::RasterSurface);
    resize(800, 600);  // Optional: match your external window size

    m_backingStore = new QBackingStore(this);
}

void ExampleQWindow::manualUpdate()
{
}

void ExampleQWindow::exposeEvent(QExposeEvent* /*event*/)
{
    if (isExposed())
        manualUpdate();
}

void ExampleQWindow::resizeEvent(QResizeEvent* event)
{
    if (m_backingStore)
        m_backingStore->resize(event->size());
    OutputDebugStringA("Window Resized");
}

void ExampleQWindow::closeEvent(QCloseEvent* event)
{
    //QMessageBox::question;
    //QWidget* parent, const QString& title,
        //const QString& text, StandardButtons buttons = StandardButtons(Yes | No),
        //StandardButton defaultButton = NoButton
	QString title = "Confirm Exit";
	QString text = "Are you sure you want to quit?";
    auto reply = QMessageBox::question(nullptr, title, text);

    if (reply == QMessageBox::Yes) {
        event->accept();  // Accept close
    }
    else {
        event->ignore();  // Prevent close
    }
}

