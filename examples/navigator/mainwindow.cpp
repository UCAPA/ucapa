#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_drone("d2e081a3", "be27e2e4", "d87f7e0c")
    , m_keepDroneInertia(false)
    , m_warning(false)
    , m_idTimerControl(0)
    , m_noSignalImage("textures/noSignal.png")
    , m_activeCamera(ucapa::ARDrone::FRONT_CAMERA)
{
    m_ui->setupUi(this);
    setWindowTitle(QApplication::applicationName());
    setWindowIcon(QIcon("ressources/UCAPA.png"));

    m_drone.setComputeWorldData(true);

    m_idTimerControl = startTimer(30); // Permet d'avoir 25 fps pour la mise à jour de l'horloge analogique
    m_controlStartTime = QTime();

    m_videoPixmap = new QPixmap;

    connect(m_ui->takeOffLandButton, SIGNAL(clicked()), this, SLOT(takeOffLand()));
    connect(m_ui->emergencyButton, SIGNAL(clicked()), this, SLOT(emergency()));

    connect(m_ui->forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    connect(m_ui->backwardButton, SIGNAL(clicked()), this, SLOT(backward()));
    connect(m_ui->leftButton, SIGNAL(clicked()), this, SLOT(left()));
    connect(m_ui->rightButton, SIGNAL(clicked()), this, SLOT(right()));
    connect(m_ui->turnLeftButton, SIGNAL(clicked()), this, SLOT(turnLeft()));
    connect(m_ui->turnRightButton, SIGNAL(clicked()), this, SLOT(turnRight()));
    connect(m_ui->upButton, SIGNAL(clicked()), this, SLOT(up()));
    connect(m_ui->downButton, SIGNAL(clicked()), this, SLOT(down()));

    connect(m_ui->switchCameraButton, SIGNAL(clicked()), this, SLOT(switchCamera()));
    connect(m_ui->calibButton, SIGNAL(clicked()), this, SLOT(calibration()));

    connect(m_ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(m_ui->actionConfigurate_Drone, SIGNAL(triggered()), this, SLOT(configDrone()));
    m_ui->menuSettings->addAction(m_ui->dockStates->toggleViewAction());
    m_ui->menuSettings->addAction(m_ui->dockValues->toggleViewAction());
    m_ui->menuSettings->addAction(m_ui->dockLED->toggleViewAction());
    m_ui->menuSettings->addAction(m_ui->dockFly->toggleViewAction());

    connect(m_ui->ledValid, SIGNAL(clicked()), this, SLOT(ledAnim()));
    connect(m_ui->flyValid, SIGNAL(clicked()), this, SLOT(flyAnim()));
}

MainWindow::~MainWindow()
{
    killTimer(m_idTimerControl);
    delete m_videoPixmap;
    delete m_ui;
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
    m_keyPressed += event->key();

    if (event->modifiers() == Qt::AltModifier)
    {
        switch (event->key())
        {
        case Qt::Key_I:
            m_drone.anim(ucapa::ARDrone::ANIM_FLIP_AHEAD);
            break;
        case Qt::Key_J:
            m_drone.anim(ucapa::ARDrone::ANIM_FLIP_LEFT);
            break;
        case Qt::Key_K:
            m_drone.anim(ucapa::ARDrone::ANIM_FLIP_BEHIND);
            break;
        case Qt::Key_L:
            m_drone.anim(ucapa::ARDrone::ANIM_FLIP_RIGHT);
            break;
        case Qt::Key_O:
            m_drone.anim(ucapa::ARDrone::ANIM_WAVE);
            break;
        }
    }

    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    m_keyPressed -= event->key();

    QWidget::keyReleaseEvent(event);
}

void MainWindow::timerEvent( QTimerEvent * evt )
{
    // Update control commandes
    if (evt->timerId()==m_idTimerControl)
    {
        ucapa::Vector3 move;
        float ry = 0;

        for (auto key : m_keyPressed)
        {
            switch (key) {
                case Qt::Key_Z: //forward
                    move.z = 0.25f;
                    break;
                case Qt::Key_S: //backward
                    move.z = -0.25f;
                    break;
                case Qt::Key_D: //right
                    move.x = 0.25f;
                    break;
                case Qt::Key_Q: //left
                    move.x = -0.25f;
                    break;
                case Qt::Key_Shift: //up
                    move.y = 0.5f;
                    break;
                case Qt::Key_Control: //down
                    move.y = -0.5f;
                    break;
                case Qt::Key_E: //Tr
                    ry = 0.6f;
                    break;
                case Qt::Key_A: //Tl
                    ry = -0.6f;
                    break;
            }
        }
        if(m_drone.isFlying())
        {
            if (!m_keepDroneInertia && move == ucapa::Vector3::zero && ry == 0)
                m_drone.enterHoveringMode();
            else
                m_drone.move(move, ry);
        }

        // Show the video stream in a QLabel
        if (m_drone.video().isActive())
        {
            int width = m_drone.video().getWidth();
            int height = m_drone.video().getHeight();
            // Incorrect size
            if (width == -1 || height == -1)
            {
                width = 640;
                height = 360;
            }
            uint8_t* frame = m_drone.video().getFrame();
            if (frame == nullptr)
                *m_videoPixmap = m_noSignalImage;
            else
            {
                *m_videoPixmap = convertFrameToPixmap(frame, width, height);
                if(m_warning)
                    repaint();
            }
            m_ui->videoLabel->setFixedSize(QSize(m_videoPixmap->width(), m_videoPixmap->height()));
            m_ui->videoLabel->setPixmap(*m_videoPixmap);
        }
        else
        {
            m_ui->videoLabel->setFixedSize(QSize(640, 360));
            *m_videoPixmap = m_noSignalImage;
            m_ui->videoLabel->setPixmap(*m_videoPixmap);
        }

        //update navDataValues
        m_ui->labelFly->setText(droneStateToText(ucapa::Navdata::FLY_MASK));
        m_ui->labelVideo->setText(droneStateToText(ucapa::Navdata::VIDEO_MASK));
        m_ui->labelAltitudeControle->setText(droneStateToText(ucapa::Navdata::ALTITUDE_MASK));
        m_ui->labelCamera->setText(droneStateToText(ucapa::Navdata::CAMERA_MASK));
        m_ui->labelUsb->setText(droneStateToText(ucapa::Navdata::USB_MASK));
        m_ui->labelEngine->setText(droneStateToText(ucapa::Navdata::MOTORS_MASK));
        m_ui->labelErreurcom->setText(droneStateToText(ucapa::Navdata::COM_WATCHDOG_MASK));
        m_ui->labelEmergency->setText(droneStateToText(ucapa::Navdata::EMERGENCY_MASK));
        m_ui->labelBattery->setText(QString::number(m_drone.nav().getBatteryPercentage()));
        m_ui->labelAltitude->setText(QString::number(m_drone.nav().getAltitude()));
        ucapa::Vector3 vec = m_drone.nav().getRotation();
        m_ui->labelAngleX->setText(QString::number(vec.x));
        m_ui->labelAngleY->setText(QString::number(vec.y));
        m_ui->labelAngleZ->setText(QString::number(vec.z));
        vec = m_drone.nav().getVelocity();
        m_ui->labelVelocityX->setText(QString::number(vec.x));
        m_ui->labelVelocityY->setText(QString::number(vec.y));
        m_ui->labelVelocityZ->setText(QString::number(vec.z));
        vec = m_drone.nav().getPosition();
        m_ui->labelPositionX->setText(QString::number(vec.x));
        m_ui->labelPositionY->setText(QString::number(vec.y));
        m_ui->labelPositionZ->setText(QString::number(vec.z));
    }
    else QWidget::timerEvent (evt);
}

void MainWindow::takeOffLand()
{
    if(m_drone.getState(ucapa::Navdata::FLY_MASK))
    {
        m_drone.land();
        m_ui->takeOffLandButton->setText("Take Off");
        m_ui->calibButton->setText("Flat Trim");
        m_drone.animLeds(ucapa::ARDrone::LED_ANIM_BLINK_ORANGE, 5, 4);
    }
    else if(m_drone.nav().getBatteryPercentage() > 20)
    {
        m_drone.takeOff();
        m_ui->takeOffLandButton->setText("Land");
        m_ui->calibButton->setText("Magnetometer calibration");
        m_drone.animLeds(ucapa::ARDrone::LED_ANIM_BLINK_ORANGE, 5, 4);
    }
    else
        QMessageBox::warning(this, "Low Battery exception", "The battery level is too low to take off.");
}

void MainWindow::emergency()
{
    m_drone.emergency();
}

void MainWindow::forward()
{
    cout << "forward" << endl;
    m_drone.move({0, 0, 0.25}, 0);
}

void MainWindow::backward()
{
    cout << "backward" << endl;
    m_drone.move({0, 0, -0.25}, 0);
}

void MainWindow::left()
{
    cout << "left" << endl;
    m_drone.move({-0.25, 0, 0}, 0);
}

void MainWindow::right()
{
    cout << "right" << endl;
    m_drone.move({0.25, 0, 0}, 0);
}

void MainWindow::turnLeft()
{
    cout << "turnLeft" << endl;
    m_drone.move({0, 0, 0}, -0.6f);
}

void MainWindow::turnRight()
{
    cout << "turnRight" << endl;
    m_drone.move({0, 0, 0}, 0.6f);
}

void MainWindow::up()
{
    cout << "up" << endl;
    m_drone.move({0, 0.5, 0}, 0);
}

void MainWindow::down()
{
    cout << "down" << endl;
    m_drone.move({0, -0.5, 0}, 0);
}

void MainWindow::stay()
{
    cout << "stay" << endl;
    m_drone.move({0, 0, 0}, 0);
}

void MainWindow::hoverMode()
{
    m_drone.enterHoveringMode();
}

void MainWindow::switchCamera()
{
    if (m_activeCamera == ucapa::ARDrone::FRONT_CAMERA)
        m_activeCamera = ucapa::ARDrone::BOTTOM_CAMERA;
    else
        m_activeCamera = ucapa::ARDrone::FRONT_CAMERA;
    m_drone.setCamera(m_activeCamera);
}

void MainWindow::configDrone()
{
    float moveSpeed = m_drone.getSpeed();
    float rotationSpeed = m_drone.getRotationSpeed();
    float verticalSpeed = m_drone.getVerticalSpeed();
    float altitudeMax = m_drone.getAltitudeMax();
    bool isWithHool = m_drone.getIsWithoutShell();
    bool isIndoor = m_drone.getIsOutdoor();

    // Pass rad in deg
    moveSpeed = radToDeg(moveSpeed);
    rotationSpeed = radToDeg(rotationSpeed);

    QDroneDialog* dialog = new QDroneDialog(moveSpeed, verticalSpeed, rotationSpeed, altitudeMax, isWithHool, isIndoor);
    if(dialog->exec())
    {
        m_drone.setSpeed(degToRad(dialog->getMoveSpeed()));
        m_drone.setRotationSpeed(degToRad(dialog->getRotationSpeed()));
        m_drone.setVerticalSpeed(dialog->getVerticalSpeed());
        m_drone.setAltitudeMax(dialog->getAltitudeMax());
        m_drone.setIsWithoutShell(dialog->getIsWithWool());
        m_drone.setIsOutdoor(dialog->getFlyMode());
    }
}

void MainWindow::help()
{
    QString helpKey("Controls list:\n\n");
    helpKey += "Z = forward\n";
    helpKey += "S = backward\n";
    helpKey += "D = right\n";
    helpKey += "Q = left\n";
    helpKey += "Shift = up\n";
    helpKey += "Control = down\n";
    helpKey += "E = Rotation left\n";
    helpKey += "A = Rotation right\n\n";
    helpKey += "Space = Take off/Land\n";
    helpKey += "Enter = Emergency";
    QMessageBox::information(this, "Controls Help", helpKey);
}

void MainWindow::ledAnim()
{
    m_drone.animLeds((ucapa::ARDrone::LED_ANIMATION_ID)m_ui->ledBox->currentIndex(), m_ui->frequencyBox->value(), m_ui->durationBox->value());
}

void MainWindow::flyAnim()
{
    m_drone.anim((ucapa::ARDrone::ANIMATION_ID)m_ui->flyBox->currentIndex());
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    if(m_drone.isFlying() && m_drone.getLastNavdataReception().count() < 2.0)
    {
        m_warning = true;
        m_warningTime = std::chrono::steady_clock::now();
        repaint();
        evt->ignore();
    }
}

void MainWindow::calibration()
{
    if(m_drone.isFlying())
        m_drone.calibrate();
    else
        m_drone.trim();
}

QPixmap MainWindow::convertFrameToPixmap(uint8_t *frameBuffer, int width, int height)
{
    QImage imgQt(width, height, QImage::Format_RGB32);
    unsigned char *src = (unsigned char *)frameBuffer;

    QRgb* dest = (QRgb *)imgQt.bits();
    for (int i = 0 ; i < width*height; i++)
        dest[i] = qRgb(src[3*i], src[3*i+1], src[3*i+2]);

    return QPixmap::fromImage(imgQt);
}

const char* MainWindow::droneStateToText(ucapa::Navdata::STATE_MASK mask)
{
    return m_drone.getState(mask) == 0 ? "false" : "true";
}

void MainWindow::about()
{
    QMessageBox::about( this,
                        QApplication::applicationName(),
                        "----------- " + QApplication::applicationName() + " -----------"
                        + "<p>" + tr("Version: ") + QApplication::applicationVersion() + "</p>"
                        + "<p><strong>" + tr("Created by :") + "</strong><br>"
                        + "CLAUZIER Geoffrey<br>"
                        + "MONCEL Arnaud<br>"
                        + "PASCAL Guillaume<br>"
                        + "RABERIN Alexandre</p>");
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    if(m_warning)
    {
        QMainWindow::paintEvent(event);

        QPainter painter(m_videoPixmap);

        int width = m_videoPixmap->width();
        int height = m_videoPixmap->height();
        painter.setViewport(0, 0, width, height);
        painter.setWindow(0,0,480,240);

        painter.setPen(QColor(255,0,0));
        painter.setFont(QFont("Helvetica", 16));
        painter.drawText(QPoint(50,painter.font().pointSize()+30), "Warning : You can't exit without landing.");

        std::chrono::duration<double> duration = std::chrono::steady_clock::now() - m_warningTime;
        if (duration.count()>=5.0)
            m_warning = false;
    }
}
