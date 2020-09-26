#ifndef MYAUDIO_H
#define MYAUDIO_H

#include <QObject>
#include <QIODevice>
#include <QAudioOutput>
#include <QDebug>
#include <QThread>
#include <QFile>
class MyAudio : public QObject
{
    Q_OBJECT
public:
    enum myPitch { High, Normal, Low };
    MyAudio(QObject *parent = Q_NULLPTR);
    ~MyAudio();
    bool isAudioReady();
    void playSound(const myPitch &pitch);
    void setVolume(const int &volume);
    int getVolume() const;
private slots:
    void myWrite(const QByteArray &buff, const int &buffSize);
private:

    QAudioOutput *audioOut;
    QAudioFormat format;
    QAudioDeviceInfo deviceinfo;
    QIODevice* auIObuffer;
    QByteArray buff, buffHigh, buffLow;
    int buffSize, buffHighSize, buffLowSize;
    bool audioReady;

    QFile testFile;
signals:
    void doWite(const QByteArray &buff, const int &buffSize);
};

#endif // MYAUDIO_H
