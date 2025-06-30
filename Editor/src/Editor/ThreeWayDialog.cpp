#include "ThreeWayDialog.hpp"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCoreApplication>

ThreeWayDialog::ThreeWayDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    connectSignals();
}

void ThreeWayDialog::setupUi()
{
    setWindowTitle(QCoreApplication::translate("Messages", "Action"));

    auto* label = new QLabel(QCoreApplication::translate("Messages", "Placeholder"), this);
    auto* buttonA = new QPushButton(QCoreApplication::translate("Messages", "Full Confirm"), this);
    auto* buttonB = new QPushButton(QCoreApplication::translate("Messages", "Confirm"), this);
    auto* buttonC = new QPushButton(QCoreApplication::translate("Messages", "Cancel"), this);
    buttonA->setStyleSheet("padding: 8px 8px;");
    buttonB->setStyleSheet("padding: 8px 8px;");
    buttonC->setStyleSheet("padding: 8px 8px;");

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonA);
    buttonLayout->addWidget(buttonB);
    buttonLayout->addWidget(buttonC);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addLayout(buttonLayout);

    // Store signals locally so they connect to this
    connect(buttonA, &QPushButton::clicked, this, [this]() {
        result = OptionA;
        accept();
        });
    connect(buttonB, &QPushButton::clicked, this, [this]() {
        result = OptionB;
        accept();
        });
    connect(buttonC, &QPushButton::clicked, this, [this]() {
        result = OptionC;
        accept();
        });
}

void ThreeWayDialog::connectSignals()
{
}

ThreeWayDialog::Result ThreeWayDialog::getResult() const
{
    return result;
}