#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), minute(60 * 1000), averageClicksSize(0), index(0), bpm(0), marginOfError(0), targetLess(0), targetMore(0), bpmDminute(0)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    on_tabWidget_currentChanged(0);
    QSize size = qApp->screens()[0]->size();
    setGeometry((size.width() * .5) - (145 - (145 / 2)), size.height() * .6, 145, 301);
    if(myAudio.isAudioReady()){
        myTimer.start();
        myMetronomeTimer.start();
        metronome = new QTimer(this);
        applaySettings();
        connect(metronome, &QTimer::timeout, this, &MainWindow::metronomeTimeout);
        metronomePitch = MyAudio::myPitch::Normal;
        myAudio.moveToThread(&audioThread);
        audioThread.start();
        metronome->setTimerType(Qt::PreciseTimer);
        metronome->start(bpmDminute);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    metronome->stop();
    while(metronome->isActive()){
        QThread::msleep(10);
    }
    disconnect(metronome, &QTimer::timeout, this, &MainWindow::metronomeTimeout);
    audioThread.terminate();
    audioThread.wait();
    delete metronome;
}

void MainWindow::mouseLeftPress() {
    qint64 avg(0), current(minute / myTimer.restart());
    averageClicks[++index % averageClicksSize] = current;
    for(size_t i(0); i != averageClicksSize; avg += averageClicks[i++]);
    avg /= averageClicksSize;
    ui->label->setText(QString::number(avg));
    if(current < targetLess){
        metronomePitch = MyAudio::myPitch::Low;
    }
    else if(current > targetMore){
        metronomePitch = MyAudio::myPitch::High;
    }
    else {
        metronomePitch = MyAudio::myPitch::Normal;
    }
}

void MainWindow::metronomeTimeout() {
    myAudio.playSound(metronomePitch);
    qint64 current(myMetronomeTimer.restart()), diff(0);
    diff = bpmDminute - current;
    metronome->setInterval(bpmDminute - diff);
}

void MainWindow::applaySettings() {
    bpm = ui->spinBox->value();
    bpmDminute = minute / bpm;
    if(metronome->isActive()) {
        metronome->setInterval(bpmDminute);
        myMetronomeTimer.restart();
    }
    marginOfError = ui->spinBox_2->value();
    targetLess = bpm - marginOfError;
    targetMore = bpm + marginOfError;
    averageClicksSize = ui->spinBox_3->value();
    averageClicks = std::vector<qint64>(averageClicksSize, 0);
    myAudio.setVolume(ui->horizontalSlider->value());
    ui->label_6->setText(QString::number(bpm));
}

void MainWindow::on_pushButton_clicked()
{
    applaySettings();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index){
        setFixedSize(145, 301);
        ui->spinBox->setValue(bpm);
        ui->spinBox_2->setValue(marginOfError);
        ui->spinBox_3->setValue(averageClicksSize);
        ui->horizontalSlider->setValue(myAudio.getVolume());
    }
    else {
        setFixedSize(140, 100);
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->label_5->setText("Volume: %"+ QString::number(value));
}
