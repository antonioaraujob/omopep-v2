#ifndef EMULATOR_H
#define EMULATOR_H

#include <QObject>

#include "scan.h"

class Emulator : public QObject
{
    Q_OBJECT


private:

    static ScanningCampaing * scan;

    static double value;

public:
    explicit Emulator(QObject *parent = 0);



    static double emulateScan(double c, double min, double max);


    static double test();

signals:

public slots:
};

#endif // EMULATOR_H
