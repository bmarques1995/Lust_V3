#pragma once

#include <QWindow>
#include <QBackingStore>
#include <QPainter>
#include <QExposeEvent>
#include <QResizeEvent>

class ExampleQWindow : public QWindow
{
   // Q_OBJECT

public:
    explicit ExampleQWindow(WId externalHwnd, QWindow* parent = nullptr);
    void manualUpdate();

protected:
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    QBackingStore* m_backingStore;
};