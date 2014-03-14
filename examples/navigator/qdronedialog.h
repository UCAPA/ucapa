#ifndef QDRONEDIALOG_H
#define QDRONEDIALOG_H

#include <QDialog>

#include "vector3.h"

namespace Ui {
class QDroneDialog;
}

class QDroneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QDroneDialog(double moveSpeed, double verticalSpeed, double rotationSpeed, double altitude, bool isWithHool, bool flyMode, QWidget *parent = 0);
    ~QDroneDialog(){}

    float getMoveSpeed() const;
    float getRotationSpeed() const;
    float getVerticalSpeed() const;

    float getAltitudeMax() const;
    bool getIsWithWool() const;
    bool getFlyMode() const;

private:
    Ui::QDroneDialog *ui;
};

#endif // QDRONEDIALOG_H
