#include "mythread.h"

MyThread::MyThread(QThread *parent): QThread(parent)
{

}

void MyThread::run() {
    this->exec();
}
