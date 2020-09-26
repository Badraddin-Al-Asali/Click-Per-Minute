#include "myaudio.h"

MyAudio::MyAudio(QObject *parent):
    QObject(parent),
    deviceinfo(QAudioDeviceInfo::defaultOutputDevice()),
    audioOut(Q_NULLPTR),
    audioReady(false)
{

    testFile.setFileName("ping.raw");
    if (!testFile.open(QIODevice::ReadOnly)) return;
    buff = testFile.readAll();
    testFile.close();
    buffSize = buff.size();

    testFile.setFileName("pingHigh.raw");
    if (!testFile.open(QIODevice::ReadOnly)) return;
    buffHigh = testFile.readAll();
    testFile.close();
    buffHighSize = buffHigh.size();

    testFile.setFileName("pingLow.raw");
    if (!testFile.open(QIODevice::ReadOnly)) return;
    buffLow = testFile.readAll();
    testFile.close();
    buffLowSize = buffLow.size();

    QObject::connect(this, &MyAudio::doWite, this, &MyAudio::myWrite);

    auto supportedSampleRates(deviceinfo.supportedSampleRates());
    int sampleRate(0);
    if(!supportedSampleRates.empty()){
        sampleRate = *supportedSampleRates.begin();
    }
    /*for (auto i(supportedSampleRates.begin()), e(supportedSampleRates.end()); i != e; ++i) {
        qDebug() << "supportedSampleRates" << *i;
    }*/
    auto supportedChannelCounts(deviceinfo.supportedChannelCounts());
    int channelCount(0);
    for (auto i(supportedChannelCounts.begin()), e(supportedChannelCounts.end()); i != e; ++i) {
        if(*i == 2){
            channelCount = 2;
        }
    }
    auto supportedSampleSizes(deviceinfo.supportedSampleSizes());
    int sampleSize(0);
    if(!supportedSampleSizes.empty()){
        sampleSize = *supportedSampleSizes.begin();
    }
    /*for (auto i(supportedSampleSizes.begin()), e(supportedSampleSizes.end()); i != e; ++i) {
        qDebug() << "supportedSampleSizes" << *i;
    }*/
    auto supportedCodecs(deviceinfo.supportedCodecs());
    QString codec;
    for (auto i(supportedCodecs.begin()), e(supportedCodecs.end()); i != e; ++i) {
        if(*i == "audio/pcm"){
            codec = *i;
        }
    }
    auto supportedByteOrders(deviceinfo.supportedByteOrders());
    QAudioFormat::Endian byteOrder;
    for (auto i(supportedByteOrders.begin()), e(supportedByteOrders.end()); i != e; ++i) {
        if(*i == QAudioFormat::Endian::LittleEndian){
            byteOrder = *i;
        }
    }
    auto supportedSampleTypes(deviceinfo.supportedSampleTypes());
    QAudioFormat::SampleType sampleType;
    for (auto i(supportedSampleTypes.begin()), e(supportedSampleTypes.end()); i != e; ++i) {
        if(*i == QAudioFormat::SampleType::SignedInt){
            sampleType = *i;
        }
    }

    /*format.setSampleRate(sampleRate);
    format.setChannelCount(channelCount);
    format.setSampleSize(sampleSize);*/
    format.setSampleRate(24000);
    format.setChannelCount(2);
    format.setSampleSize(16);

    format.setCodec(codec);
    format.setByteOrder(byteOrder);
    format.setSampleType(sampleType);

    audioReady = deviceinfo.isFormatSupported(format);
    audioOut = new QAudioOutput(deviceinfo, format);
    //audioOut->setNotifyInterval(100);
    //QObject::connect(audioOut, SIGNAL(notify()), this, SLOT(myWrite()));
    auIObuffer = audioOut->start();
}

MyAudio::~MyAudio(){
    QObject::disconnect(this, &MyAudio::doWite, this, &MyAudio::myWrite);
    //QObject::disconnect(audioOut, SIGNAL(notify()), this, SLOT(myWrite()));
    audioOut->stop();
    while(QAudio::State::StoppedState != audioOut->state()){
        QThread::msleep(25);
    }
    delete audioOut;
}

bool MyAudio::isAudioReady(){
    return audioReady;
}

void MyAudio::myWrite(const QByteArray &buff, const int &buffSize){
    int periodSize = audioOut->periodSize(); // Check the ideal chunk size, in bytes
    const char * buffPtr = buff.data();
    int tempBuffSize(buffSize);
    int totalLength(0);
    do{
        if(audioOut->bytesFree() >= periodSize) {
            if(periodSize > tempBuffSize) {
                auIObuffer->write(buffPtr, periodSize);
                buffPtr += periodSize;
                tempBuffSize -= periodSize;
                totalLength += periodSize;
            }
            else {
                auIObuffer->write(buffPtr, tempBuffSize);
                buffPtr += tempBuffSize;
                tempBuffSize -= tempBuffSize;
                totalLength += tempBuffSize;
            }
        }
        //else {
            //QThread::msleep(10);
        //}
    }while(tempBuffSize > 0);
}

void MyAudio::playSound(const myPitch &pitch){
    if(audioReady) {
        switch (pitch) {
        case myPitch::High:
                emit doWite(buffHigh, buffHighSize);
            break;
        case myPitch::Normal:
                emit doWite(buff, buffSize);
            break;
        case myPitch::Low:
                emit doWite(buffLow, buffLowSize);
            break;
        }
    }
}

void MyAudio::setVolume(const int &volume){
    if(audioReady) {
        //qreal linearVolume = QAudio::convertVolume(volume / qreal(100.0), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
        //audioOut->setVolume(linearVolume);
        audioOut->setVolume(volume / 100.0);
    }
}

int MyAudio::getVolume() const{
    return audioOut->volume() * 100.0;
}
