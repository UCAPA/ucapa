/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) <year> <copyright holders>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define _GLIBCXX_USE_NANOSLEEP 1

#include <chrono>

#include <QImage>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QTime>

#include <ardrone.h>
#include <vector3.h>

#include "qdronedialog.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void takeOffLand();
    void emergency();

    void forward();
    void backward();
    void left();
    void right();
    void turnLeft();
    void turnRight();
    void up();
    void down();

    void stay();
    void hoverMode();
    void switchCamera();

    void configDrone();

    void ledAnim();
    void flyAnim();

    void help();
    void about();

    void closeEvent(QCloseEvent* evt);

    void calibration();
    
protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void timerEvent(QTimerEvent * event);
    virtual void paintEvent(QPaintEvent* event);

private:
    Ui::MainWindow *m_ui;

    ucapa::ARDrone m_drone;

    bool m_keepDroneInertia;

    bool m_warning;
    std::chrono::steady_clock::time_point m_warningTime;

    QSet<int> m_keyPressed;
    int m_idTimerControl;
    QTime m_controlStartTime;

    // Video
    QPixmap* m_videoPixmap;
    QPixmap m_noSignalImage;
    ucapa::ARDrone::VIDEO_CAMERA m_activeCamera;

    /**
     * @brief Convert to pixmap an image represented by a buffer of uint8_t
     * @param frameBuffer buffer representing the image (organised in R, G, B)
     * @param width width of the image
     * @param height height of the image
     * @return pixmap corresponding to the buffer
     */
    QPixmap convertFrameToPixmap(uint8_t *frameBuffer, int width, int height);

    /**
     * @brief convert the state of the drone mask to character chain
     * @param mask of the drone state to convert
     * @return character chain "true" or "false"
     */
    const char* droneStateToText(ucapa::Navdata::STATE_MASK mask);
};

#endif // MAINWINDOW_H
