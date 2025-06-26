#include "ThreeWayDialog.hpp"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

ThreeWayDialog::ThreeWayDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    connectSignals();
}

void ThreeWayDialog::setupUi()
{
    setWindowTitle("Close Program");

    auto* label = new QLabel("Do you want to: ", this);
    auto* buttonA = new QPushButton("Save and Close", this);
    auto* buttonB = new QPushButton("Close Without Save", this);
    auto* buttonC = new QPushButton("Cancel", this);

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