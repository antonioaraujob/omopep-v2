#include "particle.h"

#include "mainwindow.h"
#include "simulation.h"

#include <QTime>

#include "scan.h"
#include "emulator.h"


Particle::Particle(int numberOfApsDeployed, int maxSpeed)
{
    //qsrand((uint)QTime::currentTime().msec());

    // asignar el valor unico del identificador del individuo
    particleId = Simulation::getNewParticleId();


    // se deben crear los 33 parametros
    // C1,Min1,Max1,AP1,C2,Min2,Max2,AP2,...,C11,Min11,Max11,AP11


    // base de datos sqlite
    //QString database("/home/antonio/Copy/2014/pgcomp/ia/gridCells/gridCells/test_18.1.db");
    //QString database("/home/antonio/desarrollo/iaa/git/omocac/test_18.1.db");
    QString database("test_18.1.db");

    // tipo de experimento para extraer las muestras: full -> full scanning
    QString experiment("full");

    //Scan scan(database.toStdString(),experiment.toStdString());
    ScanningCampaing scan(database.toStdString(),experiment.toStdString());
    scan.init();

    //Scan::ScanResults results = scan.execute(11, 10, 30);
    //Scan::ScanResults results;
    ScanningCampaing::ScanResults results;

    //std::cout << results.size() << " results: " << std::endl;

    int randomChannel = 0;
    double minChannelTime = 0;
    double maxChannelTime = 0;
    double apsFound = 0;

    for (int i=0; i<11; i++)
    {
        randomChannel = getRandomChannel();
        parametersList.append(randomChannel);

        minChannelTime = getRandomMinChannelTime();
        maxChannelTime = getRandomMaxChannelTime();
        parametersList.append(minChannelTime);
        parametersList.append(maxChannelTime);

        //parametersList.append(getAPNumberOnChannel(numberOfApsDeployed, randomChannel));

        //qDebug("**channel: %d, min: %f, max: %f",randomChannel, minChannelTime, maxChannelTime);
        //results = scan.execute(randomChannel, minChannelTime, maxChannelTime);
        apsFound = scan.getAPs(randomChannel, minChannelTime, maxChannelTime);


        //qDebug("**numero de APs encontrados en el canal %d: %d",randomChannel, results.size());
        //std::cout << " numero de APs encontrados en el canal: " << randomChannel << ": " << results.size() << std::endl;
        //qDebug("**scan.execute(%d, %f, %f)=%d",randomChannel, minChannelTime, maxChannelTime, results.size());
        //parametersList.append(results.size());
        parametersList.append(apsFound);

        wonMatchesCounter = 0;
    }


    // lista de velocidades por parametro
    // ...
    for (int i=0; i<44; i++)
    {
        velocitityList.append(getRandomSpeed(maxSpeed));
    }


    // calcular el valor de desempeno para el individuo
    calculatePerformanceValue();

    // calcular el valor de desempeno para la descubierta
    //setPerformanceDiscovery(getRandomMaxChannelTime());
    calculateDiscoveryValue();

    // calcular el valor de desempeno para la latencia
    //setPerformanceLatency(getRandomMaxChannelTime());
    calculateLatencyValue();


    // inicializar el diccionario de canales utilizados en el vuelo en falso
    for (int i=1; i<=11;i++)
    {
        channelsUsedForFly[i]=false;
    }
}


Particle::Particle(Particle &p)
{
    particleId = p.getParticleId();
    wonMatchesCounter = p.getWonMatchesCounter();

    for (int i=0; i<44; i++)
    {
        parametersList.append(p.getParameter(i));
    }

    for (int j=0; j<44; j++)
    {
        velocitityList.append(p.getParameter(j));
    }

    // calcular el valor de desempeno para el individuo
    calculatePerformanceValue();

    // calcular el valor de desempeno para la descubierta
    //setPerformanceDiscovery(getRandomMaxChannelTime());
    calculateDiscoveryValue();

    // calcular el valor de desempeno para la latencia
    //setPerformanceLatency(getRandomMaxChannelTime());
    calculateLatencyValue();

    // inicializar el diccionario de canales utilizados en el vuelo en falso
    for (int i=1; i<=11;i++)
    {
        channelsUsedForFly[i]=false;
    }

    apMin = p.apMin;
    apMax = p.apMax;
    apSum = p.apSum;

}

int Particle::getParticleId()
{
    return particleId;
}


int Particle::getRandomChannel()
{
/*
    // el rango es 1 <= channel <= 11
    int low = 1;
    int high = 11;
    return qrand() % ((high + 1) - low) + low;
*/

    int low = 1;
    int high = 11;
    int value = 0;

    while(true)
    {
        value = qrand() % ((high + 1) - low) + low;
        if (!channelSequenceSet.contains(value))
        {
            channelSequenceSet.insert(value);
            //qDebug("canal seleccionado: %d", value);
            return value;
        }
    }
}

int Particle::getJustARandomChannel()
{
    // el rango es 1 <= channel <= 11
    int low = 1;
    int high = 11;
    return qrand() % ((high + 1) - low) + low;
}

double Particle::getRandomMinChannelTime()
{
    // el rango es 0 <= MinChannelTime <= 10 en ms
    int low = 5;
    int high = 15;
    return qrand() % ((high + 1) - low) + low;
}

double Particle::getRandomMaxChannelTime()
{

    // el rango es 10 <= MaxChannelTime <= 100
    int low = 3;
    int high = 90;
    return qrand() % ((high + 1) - low) + low;
}

double Particle::getRandomSpeed(int maxSpeed)
{
    // el rango es 1 <= randomSpeed <= maxSpeed
    int low = 1;
    int high = maxSpeed;
    return qrand() % ((high + 1) - low) + low;
}


double Particle::getAPNumberOnChannel(int numberOfApsDeployed, int channel)
{
/*
    // la proporcion de APs que operan en cada canal es tomada del articulo
    //
    if (channel == 1)
    {
        return (numberOfApsDeployed * 18)/100;
    }
    if (channel == 2)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 3)
    {
        return (numberOfApsDeployed * 3)/100;
    }
    if (channel == 4)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 5)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 6)
    {
        return (numberOfApsDeployed * 36)/100;
    }
    if (channel == 7)
    {
        return (numberOfApsDeployed * 2)/100;
    }
    if (channel == 8)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 9)
    {
        return (numberOfApsDeployed * 6)/100;
    }
    if (channel == 10)
    {
        return (numberOfApsDeployed * 6)/100;
    }
    if (channel == 11)
    {
        return (numberOfApsDeployed * 25)/100;
    }
*/
    return 0;
}

void Particle::printParticle()
{
    //qDebug("El Individual creado es el siguiente:");
    QString individualString("   ");
    for (int j=0;j<44;j++)
    {
        if ( (j == 0) || (j == 4) || (j == 8) || (j == 12) || (j == 16) ||
             (j == 20) || (j == 24) || (j == 28) || (j == 32) || (j == 36) || (j == 40) )
        {
            individualString.append("(");
            individualString.append(QString::number(parametersList.at(j)));
            individualString.append(")");
        }
        else
        {
            individualString.append(QString::number(parametersList.at(j)));
        }



        if (j!=43)
            individualString.append("-");
    }
    individualString.append("|");
    individualString.append(QString::number(getPerformanceDiscovery()));
    individualString.append("|");
    individualString.append(QString::number(getPerformanceLatency()));
    qDebug(qPrintable(individualString));
    //qDebug("Fo:%f",getPerformanceValue());
}

QString Particle::getParticleAsQString()
{
    //qDebug("El Individual creado es el siguiente:");
    QString individualString("");
    for (int j=0;j<44;j++)
    {
        individualString.append(QString::number(parametersList.at(j)));
        if (j!=43)
            individualString.append("-");
    }
    individualString.append("|");
    individualString.append(QString::number(getPerformanceDiscovery()));
    individualString.append("|");
    individualString.append(QString::number(getPerformanceLatency()));
    return individualString;
}

void Particle::setPerformanceValue(double performance)
{
    performanceValue = performance;
}


void Particle::calculatePerformanceValue()
{
    performanceValue = parametersList.at(2)+parametersList.at(5)+parametersList.at(8)+parametersList.at(11)+parametersList.at(14)+
            parametersList.at(17)+parametersList.at(20)+parametersList.at(23)+parametersList.at(26)+parametersList.at(29)+parametersList.at(32);

    // se deben calcular los valores de las funciones objetivo para el individuo


    // D = Sumatoria i=1,11 Pi*di
    performanceDiscovery = 0;


    performanceLatency = 0;


}

void Particle::calculateDiscoveryValue()
{
    /*
    double api = 0;
    double discovery = 0;
    double probOfAtLeastOneAP = 0;
    double factor = 0;

    for (int i=0; i<11; i++)
    {
        api = parametersList.at((i*4)+3);

        probOfAtLeastOneAP = probabilityExistAtLeastOneAp(i+1);

        factor = probOfAtLeastOneAP / (i+1);

        discovery = discovery + api + probOfAtLeastOneAP;
    }
    performanceDiscovery = discovery;
    */

    performanceDiscovery = getAPsAndLatencyOfAllChannels();
}

double Particle::getAPsAndLatencyOfAllChannels()
{
    double discovery = 0;

    // obtener APmin/min + APmax/max


    int channel = 0;
    double min = 0;
    double max = 0;


    // limpiar la lista de indices de descubierta por canal
    //of1IndexList.clear();


    // iterar por cada canal
    for (int c=0; c<11; c++)
    {
        channel = parametersList.at(c*4);
        min = parametersList.at(c*4+1);
        max = parametersList.at(c*4+2);

        // primer termino APmin/min
        double minAPsum = 0;

        for (int i=0; i<30; i++)
        {
            // (ch, min, 0) corresponde a los APs encontrados con minchanneltime
            //minAPsum = minAPsum + scan.getAPs(channel, min, 0);
            minAPsum = minAPsum + Emulator::emulateScan(channel, min, 0);
        }

        // valor promedio de APs encontrados con MinChannelTime
        double APmin = minAPsum/30;

        // mantener el registro de aps encontrados con min
        apMin = apMin + APmin;

        // segundo termino APmax/max
        double maxAPsum = 0;

        for (int i=0; i<30; i++)
        {
            // (ch, min, max) corresponde a los APs encontrados con maxchanneltime
            //maxAPsum = maxAPsum + scan.getAPs(channel, min, max);
            maxAPsum = maxAPsum + Emulator::emulateScan(channel, min, max);
        }

        // valor promedio de APs encontrados con MaxChannelTime
        double APmax = maxAPsum/30;

        // mantener el registro de aps encontrados con max
        apMax = apMax + APmax;

        double index = 0;

        // verificar si APmin > 0
        if (APmin > 0)
        {
            if (max == 0)
            {
                index = APmin/min;
                // agregar el indice de descubierta del canal
                //of1IndexList.append(index);

                // asignar el valor del numero de APs encontrados al canal
                setParameter(c*4+3, APmin);

                discovery = discovery + APmin/min;
            }
            else
            {
                index = APmin/min + std::abs(APmax-APmin)/max;
                // agregar el indice de descubierta del canal
                //of1IndexList.append(index);

                // asignar el valor del numero de APs encontrados al canal
                setParameter(c*4+3, APmax);

                discovery = discovery + APmin/min + std::abs(APmax-APmin)/max;
            }
        }
        else
        {
                index = APmin/min;
                // agregar el indice de descubierta del canal
                //of1IndexList.append(index);

                // asignar el valor del numero de APs encontrados al canal
                setParameter(c*4+3, APmin);

                discovery = discovery + index;
        }

    } // fin de iteracion por cada canal

    apSum = discovery;
    return discovery;
}



void Particle::calculateLatencyValue()
{
    int channel = 0;
    double minChannelTime = 0;
    double maxChannelTime = 0;
    double latency = 0;

    /*
    for (int i=0; i<11; i++)
    {
        channel = parametersList.at((i*4));
        minChannelTime = parametersList.at((i*4)+1);
        maxChannelTime = parametersList.at((i*4)+2);
        latency = latency + minChannelTime + (probabilityDelayLessThanMinCT(minChannelTime)*probabilityExistAtLeastOneAp(channel)*maxChannelTime);
    }
    performanceLatency = latency;
    */

    double aps = 0;
    for (int i=0; i<11; i++)
    {
        minChannelTime = parametersList.at((i*4)+1);

        // verificar que el valor del AP encontrados sea >0 para sumar el Max
        aps = parametersList.at((i*4)+3);
        if (aps > 0)
        {
            maxChannelTime = parametersList.at((i*4)+2);
        }
        else
        {
            maxChannelTime = 0;
        }
        latency = latency + minChannelTime + maxChannelTime;
    }
    performanceLatency = latency;
}


double Particle::getPerformanceValue()
{
    return performanceValue;
}


void Particle::setPerformanceDiscovery(double performance)
{
    performanceDiscovery = performance;
}


double Particle::getPerformanceDiscovery() const
{
    return performanceDiscovery;
}

void Particle::setPerformanceLatency(double performance)
{
    performanceLatency = performance;
}


double Particle::getPerformanceLatency() const
{
    return performanceLatency;
}




void Particle::setParameter(int i, double value)
{
    parametersList.replace(i,value);
}

double Particle::getParameter(int i) const
{
    return parametersList.at(i);
}

int Particle::getNumberOfParameters() const
{
    return parametersList.count();
}

void Particle::setWonMatchesCounter(int value)
{
    wonMatchesCounter = value;
}

int Particle::getWonMatchesCounter()
{
    return wonMatchesCounter;
}

void Particle::incrementWonMatchesCounter()
{
    wonMatchesCounter++;
}

/*
Particle& Particle::operator = (const Particle &ind)
{
    for (int i=0;i<ind.getNumberOfParameters();i++)
    {
        this->setParameter(i,ind.getParameter(i));
    }
    this->setPerformanceDiscovery(ind.getPerformanceDiscovery());
    this->setPerformanceLatency(ind.getPerformanceLatency());

    return *this;
}
*/

double Particle::probabilityExistAtLeastOneAp(int channel)
{
    //qDebug("Individual::probabilityExistAtLeastOneAp");
    double probability = 0;

    if (channel == 1)
        probability = 0.82;
    else if (channel == 2)
        probability = 0.09;
    else if (channel == 3)
        probability = 0.35;
    else if (channel == 4)
        probability = 0.1;
    else if (channel == 5)
        probability = 0.11;
    else if (channel == 6)
        probability = 0.92;
    else if (channel == 7)
        probability = 0.12;
    else if (channel == 8)
        probability = 0.13;
    else if (channel == 9)
        probability = 0.45;
    else if (channel == 10)
        probability = 0.4;
    else if (channel == 11)
        probability = 0.83;
    else
        probability = 0;

    return probability;
}

double Particle::probabilityDelayLessThanMinCT(double delay)
{
    //qDebug("Individual::probabilityDelayLessThanMinCT");

    double probability = 0;

    if (delay == 0)
        probability = 0;
    else if (delay == 1)
        probability = 0.01;
    else if (delay == 2)
        probability = 0.03;
    else if (delay == 3)
        probability = 0.5;
    else if (delay == 4)
        probability = 0.68;
    else if (delay == 5)
        probability = 0.72;
    else if (delay == 6)
        probability = 0.79;
    else if (delay == 7)
        probability = 0.82;
    else if (delay == 8)
        probability = 0.84;
    else if (delay == 9)
        probability = 0.88;
    else if (delay == 10)
        probability = 0.89;
    else if (delay == 11)
        probability = 0.9;
    else if (delay == 12)
        probability = 0.91;
    else if (delay == 13)
        probability = 0.92;
    else if (delay == 14)
        probability = 0.93;
    else if (delay == 15)
        probability = 0.94;
    else if (delay == 16)
        probability = 0.95;
    else if (delay == 17)
        probability = 0.96;
    else if (delay == 18)
        probability = 0.97;
    else if (delay == 19)
        probability = 0.98;
    else if (delay == 20)
        probability = 0.99;
    else if (delay == 21)
        probability = 0.99;
    else if (delay == 22)
        probability = 0.99;
    else if (delay == 23)
        probability = 0.99;
    else if (delay == 24)
        probability = 0.99;
    else if (delay == 25)
        probability = 1;
    else if (delay > 25)
        probability =  1;
    else
        probability = 0;

    return probability;
}

void Particle::setVelocitity(int position, double velocityValue)
{
    velocitityList.replace(position, velocityValue);
}


double Particle::getVelocity(int position)
{
    return velocitityList.at(position);
}


bool Particle::isThisParameterAChannel(int index)
{
    if ( (index == 0) || (index == 4) || (index == 8) || (index == 12) || (index == 16) ||
         (index == 20) || (index == 24) || (index == 28) || (index == 32) || (index == 36) || (index == 40) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Particle::isThisParameterAMinChannelTime(int index)
{
    if ( (index == 1) || (index == 5) || (index == 9) || (index == 13) || (index == 17) ||
         (index == 21) || (index == 25) || (index == 29) || (index == 33) || (index == 37) || (index == 41) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Particle::isThisParameterAMaxChannelTime(int index)
{
    if ( (index == 2) || (index == 6) || (index == 10) || (index == 14) || (index == 18) ||
         (index == 22) || (index == 26) || (index == 30) || (index == 34) || (index == 38) || (index == 42) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool Particle::isThisParameterAPs(int index)
{
    if ( (index == 3) || (index == 7) || (index == 11) || (index == 15) || (index == 19) ||
         (index == 23) || (index == 27) || (index == 31) || (index == 35) || (index == 39) || (index == 43) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Particle::getNewParameterAPs(int channel, double minChannelTime, double maxChannelTime)
{
    //qDebug("Mutation::getNewParameterAPs(%d, %f, %f)", channel, minChannelTime, maxChannelTime);

    // base de datos de experimentos
    QString database("test_18.1.db");
    // tipo de experimento para extraer las muestras: full -> full scanning
    QString experiment("full");
    //Scan scan(database.toStdString(),experiment.toStdString());
    ScanningCampaing scan(database.toStdString(),experiment.toStdString());
    scan.init();

    //Scan::ScanResults results;
    ScanningCampaing::ScanResults results;

    //results = scan.execute(channel, minChannelTime, maxChannelTime);

    double apsFound = 0;
    apsFound = scan.getAPs(channel, minChannelTime, maxChannelTime);

    //qDebug("** nuevo parametro AP: %d", results.size());
    //return results.size();

    return apsFound;


}

QHash<int, bool> Particle::getChannelsUsedForFly()
{
    return channelsUsedForFly;
}


bool Particle::isChannelsUsedForFly(int channel)
{
    if (channelsUsedForFly.value(channel))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Particle::markChannelUsedForFly(int channel)
{
    channelsUsedForFly[channel] = true;
}


void Particle::resetChannelsUsedForFly()
{
    // inicializar el diccionario de canales utilizados en el vuelo en falso
    for (int i=1; i<=11;i++)
    {
        channelsUsedForFly[i]=false;
    }
}

bool Particle::operator==(const Particle &p) const
{

    for (int i=0; i<getNumberOfParameters(); i++)
    {
        if (getParameter(i) != p.getParameter(i))
        {
            return false;
        }
    }
    int myId = particleId;

    int otherId = p.particleId;

    if ( myId != otherId )
    {
        return false;
    }
    return true;
}




