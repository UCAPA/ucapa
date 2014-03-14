#include "qdronedialog.h"
#include "ui_qdronedialog.h"

QDroneDialog::QDroneDialog(double moveSpeed, double verticalSpeed, double rotationSpeed, double altitude, bool isWithHool, bool flyMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDroneDialog)
{
    ui->setupUi(this);

    ui->moveSpeed->setValue(moveSpeed);
    ui->verticalSpeed->setValue(verticalSpeed);
    ui->rotationSpeed->setValue(rotationSpeed);

    ui->maximumAltitude->setValue(altitude);
    ui->withHool->setChecked(isWithHool);
    ui->flyMode->setChecked(flyMode);
}

float QDroneDialog::getMoveSpeed() const
{
    return ui->moveSpeed->value();
}

float QDroneDialog::getRotationSpeed() const
{
    return ui->rotationSpeed->value();
}

float QDroneDialog::getVerticalSpeed() const
{
    return ui->verticalSpeed->value();
}

float QDroneDialog::getAltitudeMax() const
{
    return ui->maximumAltitude->value();
}

bool QDroneDialog::getIsWithWool() const
{
    return ui->withHool->isChecked();
}

bool QDroneDialog::getFlyMode() const
{
    return ui->flyMode->isChecked();
}
