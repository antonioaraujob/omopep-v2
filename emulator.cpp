#include "emulator.h"


ScanningCampaing * Emulator::scan = NULL;

double Emulator::value = 0;

Emulator::Emulator(QObject *parent) : QObject(parent)
{

    value = 0;

    // base de datos sqlite
    QString database("test_18.1.db");

    // tipo de experimento para extraer las muestras: full -> full scanning
    QString experiment("full");

    //Scan scan(database.toStdString(),experiment.toStdString());
    scan = new ScanningCampaing(database.toStdString(),experiment.toStdString(), 0);

    scan->init();
    scan->prepareIRD();
}


double Emulator::emulateScan(double c, double min, double max)
{

    /*
    // prueba de retornar el promedio de 30 ejecuciones de scanning

    int repetitions = 30;
    double total = 0;

    for (int i=0; i<repetitions; i++)
    {
        total = total + scan->getAPs(c, min, max);
    }
    double average = total/repetitions;

    return average;
    */

    double apsFounds = scan->getAPs(c, min, max);
    return apsFounds;
}

double Emulator::test()
{
    return value;
}

