#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QTimer>

#include <QScreen>
#include <QDebug>
#include "myaudio.h"
#include "mythread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mouseLeftPress();
private slots:
    void metronomeTimeout();
    void on_pushButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QElapsedTimer myTimer, myMetronomeTimer;
    QTimer *metronome;

    size_t averageClicksSize;
    size_t index;
    const qint64 minute;

    qint64 bpm;
    qint64 bpmDminute;
    qint64 marginOfError;
    qint64 targetLess;
    qint64 targetMore;

    std::vector<qint64> averageClicks;

    MyAudio myAudio;
    MyAudio::myPitch metronomePitch;
    MyThread audioThread;
    void applaySettings();
};
#endif // MAINWINDOW_H
