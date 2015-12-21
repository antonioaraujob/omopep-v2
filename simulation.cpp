#include "simulation.h"
#include "particle.h"

#include <QList>
#include <QMessageBox>


/**
 * @brief Funcion de comparacion de particulas con respecto al valor de desempeno de descubrimiento
 * @param p1 Particula 1 a comparar
 * @param p2 Particula 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de descubrimiento
 */
inline static bool xLessThanF1(Particle *p1, Particle *p2)
{
    return p1->getPerformanceDiscovery() < p2->getPerformanceDiscovery();
}


/**
 * @brief Funcion de comparacion de particulas con respecto al valor de desempeno de latencia
 * @param p1 Particula 1 a comparar
 * @param p2 Particula 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de latencia
 */
inline static bool xLessThanF2(Particle *p1, Particle *p2)
{
    return p1->getPerformanceLatency() < p2->getPerformanceLatency();
}

/**
 * @brief Funcion de comparacion de celdas de particulas de la grid
 * @param c1 celda 1 a comparar
 * @param c2 celda 2 a comparar
 * @return
 */
inline static bool cellLessThan(Cell * c1, Cell *c2)
{
    //return c1->getCount() < c2->getCount();
    return c1->getNeighboursParticlesCount() < c2->getNeighboursParticlesCount();

}


/**
 * @brief Define e inicializa el miembro estatico individualIdCounter
 */
int Simulation::particleIdCounter = 0;


Simulation::Simulation(int cognitiveParameter, int socialParameter,
                       double inertiaParameter, int maxSpeedParameter,
                       int particlesParameter, int maxIterations, bool selection,
                       int subintervalsGrid)
{
    cognitive = cognitiveParameter;

    social = socialParameter;

    inertia = inertiaParameter;

    maxSpeed = maxSpeedParameter;

    particles = particlesParameter;

    iterations = maxIterations;

    currentIteration = 0;

    gRepository = new GlobalRepository();

    pRepository = new ParticleRepository();

    lF1 = 0;

    uF1 = 0;

    lF2 = 0;

    uF2 = 0;

    gridSubintervalsNumber = subintervalsGrid;

    selectionModified = selection;

}

Simulation::~Simulation()
{
}

void Simulation::initializeParticles()
{

    Particle * particle;

    // inicializacion de la poblacion
    for (int i = 0; i < particles; i++)
    {
        particle = new Particle(0, maxSpeed);
        particle->printParticle();
        //qDebug("particleId: %d", particle->getParticleId());
        particleList.append(particle);

        // verificar repositorio global
        // verificar si la particula NO ES DOMINADA por cada particula del repositorio global
        if (!gRepository->isNewParticleDominatedByGlobalRepository(particle))
        {
            gRepository->addNonDominatedParticle(particle);

            gRepository->eliminateDominatedParticles();

        }

        // agregar la posicion actual de la particula a su repositorio local
        QList<Particle*> internalParticleList;
        Particle * newParticle = new Particle(*particle);
        internalParticleList.append(newParticle);

        // agregar en el repositorio el identificador de la particula y su
        // correspondiente lista interna
        pRepository->addParticle(newParticle, internalParticleList);
    }
    qDebug("tamano de la poblacion: %d",particleList.count());
}


void Simulation::intializeGrid()
{
    getInitialGridBoundaries();
    nGrid = new Grid(gridSubintervalsNumber, lF1, uF1, lF2, uF2);

    // agregar las particulas a la grid

    Particle * auxParticle;
    for (int i = 0; i < particleList.count(); i++)
    {
        auxParticle = new Particle(*particleList.at(i));
        nGrid->addParticleToGrid(auxParticle);
    }


}

void Simulation::getInitialGridBoundaries()
{
    // ordenar los no dominados con respecto a la funcion objetivo 1 de menor a mayor
    qSort(particleList.begin(), particleList.end(), xLessThanF1);

    // tomar los limites inferior y superior
    lF1 = particleList.at(0)->getPerformanceDiscovery();
    uF1 = particleList.at(particleList.count()-1)->getPerformanceDiscovery();

    // ordenar los no dominados con respecto a la funcion objetivo 2 de menor a mayor
    qSort(particleList.begin(), particleList.end(), xLessThanF2);

    lF2 = particleList.at(0)->getPerformanceLatency();
    uF2 = particleList.at(particleList.count()-1)->getPerformanceLatency();
}

void Simulation::getNewGridBoundaries()
{
    QList<Particle*> globalRepositoryList = gRepository->getRepositoryList();
    // ordenar los no dominados con respecto a la funcion objetivo 1 de menor a mayor
    qSort(globalRepositoryList.begin(), globalRepositoryList.end(), xLessThanF1);

    // tomar los limites inferior y superior
    lF1 = globalRepositoryList.at(0)->getPerformanceDiscovery();
    uF1 = globalRepositoryList.at(globalRepositoryList.count()-1)->getPerformanceDiscovery();

    // ordenar los no dominados con respecto a la funcion objetivo 2 de menor a mayor
    qSort(globalRepositoryList.begin(), globalRepositoryList.end(), xLessThanF2);

    lF2 = globalRepositoryList.at(0)->getPerformanceLatency();
    uF2 = globalRepositoryList.at(globalRepositoryList.count()-1)->getPerformanceLatency();
}

void Simulation::updateParticles()
{
    Particle * particle;

    // iterar sobre cada particula
    for (int i = 0; i < particles; i++)
    {
        //qDebug("****** actualizando particulas******");
        particle = particleList.at(i);

        // seleccionar el mejor global
        if (selectionModified)
        {
            bestGlobal = nGrid->getLeader();
            //bestGlobal = gRepository->getRandomParticle();
        }
        else
        {
            bestGlobal = gRepository->getRandomParticle();
        }

        // seleccionar el mejor local
        bestLocal = pRepository->getRandomLocalFromParticle(particle->getParticleId());

        // iterar sobre cada componente de la particula
        for (int j = 0; j < particle->getNumberOfParameters(); j++)
        {
            double newVelocity = getInertiaParameter() + particle->getVelocity(j) +
                    (getCognitiveParameter() * getRandomUniform() * ( bestGlobal->getParameter(j) - particle->getParameter(j))) +
                    (getSocialParameter() * getRandomUniform() * (bestLocal->getParameter(j) - particle->getParameter(j)));

            particle->setVelocitity(j,newVelocity);

            if (particle->getVelocity(j) > getMaxSpeedParameter())
            {
                particle->setVelocitity(j, getMaxSpeedParameter());
            }else if (particle->getVelocity(j) < getMaxSpeedParameter()*(-1))
            {
                particle->setVelocitity(j, getMaxSpeedParameter()*(-1));
            }

            int newParameter = particle->getParameter(j) + particle->getVelocity(j);

            // ahora la actualizacion del componente en la posicion j

            // chequear que el parametro no sea el numero de AP ni que los canales se repitan
            int index = j;

            if (particle->isThisParameterAChannel(index))
            {
                //qDebug("   isThisParameterAChannel(index)");

                // verificar que: 1 <= newParameter <= 11
                if ( (newParameter <= 0) || (newParameter > 11) )
                {
                    // asignar un nuevo valor valido
                    newParameter = particle->getJustARandomChannel();

                }
                // verificar que el canal no se haya utilizado

                //QHash<int, bool> channelsUsed = particle->getChannelsUsedForFly();
                while (particle->isChannelsUsedForFly(newParameter))
                {
                    // seleccionar otro canal que no se haya seleccionado
                    newParameter = particle->getJustARandomChannel();
                }
                //channelsUsed[newParameter]=true;
                particle->markChannelUsedForFly(newParameter);


                //qDebug(qPrintable("   channel despues de mutado: "+QString::number(intYi)));
            }
            else if (particle->isThisParameterAMinChannelTime(index))
            {
                //qDebug("   isThisParameterAMinChannelTime(index)");
                if (newParameter < 0)
                {
                    newParameter = 0;
                    //qDebug("   el minChannelTime mutado esta por debajo del limite (index %d)", index);
                }
                else if (newParameter > 10)
                {
                    newParameter = 10;
                    //qDebug("   el minChannelTime mutado esta por encima del limite (index %d)", index);
                }

                //qDebug(qPrintable("   minChannelTime despues de mutado: "+QString::number(intYi)));
            }
            else if (particle->isThisParameterAMaxChannelTime(index))
            {
                //qDebug("   isThisParameterAMaxChannelTime(index)");
                if (newParameter < 10)
                {
                    newParameter = 10;
                    //qDebug("   el maxChannelTime mutado esta por debajo del limite (index %d)", index);
                }
                else if (newParameter > 100)
                {
                    newParameter = 100;
                    //qDebug("   el maxChannelTime mutado esta por encima del limite (index %d)", index);
                }

                //qDebug(qPrintable("   maxChannelTime despues de mutado: "+QString::number(intYi)));
            }
            else if (particle->isThisParameterAPs(index))
            {
                //qDebug("   isThisParameterAPs(index)");
                newParameter = particle->getNewParameterAPs(particle->getParameter(index-3),
                                           particle->getParameter(index-2),
                                           particle->getParameter(index-1));
                //qDebug(qPrintable("   APs despues de mutado: "+QString::number(intYi)));
            }

            //qDebug("oldParameter: %f - newParameter: %d", particle->getParameter(j), newParameter);
            particle->setParameter(j,newParameter);
        }

        // resetear el diccionario de canales utilizados en el vuelo
        particle->resetChannelsUsedForFly();

        // reemplazar la particula con sus componentes actualizados
        particleList.replace(i,particle);
        //particle->printParticle();



    }

    // iterar sobre cada particula
    for (int k = 0; k < particles; k++)
    {
        particle = particleList.at(k);

        // evaluar las particulas
        particle->calculateDiscoveryValue();
        particle->calculateLatencyValue();

        // actualizar los repositorios

        // verificar si la particula NO ES DOMINADA por cada particula del repositorio global
        if (!gRepository->isNewParticleDominatedByGlobalRepository(particle))
        {
            gRepository->addNonDominatedParticle(particle);

            gRepository->eliminateDominatedParticles();

            if (selectionModified)
            {
                addParticleToGrid(particle);
            }
        }

        // verificar repositorio local
        // verificar si la particula NO ES DOMINADA por cada particula de su repositorio
        if (!pRepository->isNewParticleDominatedByRepository(particle))
        {

            //pRepository->addNonDominatedParticle(particle);
            pRepository->addNonDominatedParticle(particle);

            pRepository->eliminateDominatedParticles();

        }

    }
    //qDebug("****** fin actualizando particulas******");
}


int Simulation::getNewParticleId()
{
    int newId = particleIdCounter++;
    return newId;
}

void Simulation::incrementIteration()
{
    currentIteration++;
}

int Simulation::getCurrentIteration()
{
    return currentIteration;
}

bool Simulation::stopEvolution()
{
    if (currentIteration == iterations)
        return true;
    else
        return false;
}


int Simulation::getCognitiveParameter()
{
    return cognitive;
}

int Simulation::getSocialParameter()
{
    return social;
}

double Simulation::getInertiaParameter()
{
    return inertia;
}

int Simulation::getMaxSpeedParameter()
{
    return maxSpeed;
}

double Simulation::getRandomUniform()
{
    return qrand()/double(RAND_MAX);
}

void Simulation::printGlobalRepository()
{
    for (int i = 0; i < gRepository->getRepositoryList().count(); i++)
    {
        gRepository->getRepositoryList().at(i)->printParticle();
    }
}

GlobalRepository * Simulation::getGlobalRepository()
{
    return gRepository;
}

void Simulation::setSelectionModified(bool selection)
{
    selectionModified = selection;
}

bool Simulation::getSelectionModified()
{
    return selectionModified;
}

void Simulation::addParticleToGrid(Particle * newParticle)
{
    if (nGrid->particleInsideGrid(newParticle))
    {
        Particle * auxParticle;
        auxParticle = new Particle(*newParticle);
        nGrid->addParticleToGrid(auxParticle);
    }
    else // REGENERAR LA GRID!!!
    {
        /*
        QMessageBox msg;
        QString text("La particula con id: ");
        text.append(QString::number(newParticle->getParticleId()));
        text.append(" no esta en la grid.\n¡SE DEBE REGENERAR!");
        msg.setText(text);
        msg.exec();
        */

        getNewGridBoundaries();
        delete nGrid;
        nGrid = new Grid(gridSubintervalsNumber, lF1, uF1, lF2, uF2);
        updateGrid(gRepository->getRepositoryList());
    }
}

void Simulation::updateGrid(QList<Particle *> globalRepositoryList)
{

    // Para actualizar la rejilla simplemente se incrementan los contadores de los
    // particulas no dominadas con todos los individuos recién agregados al
    // archivo externo durante la generación actual.

    qDebug("Simulation::updateGrid");
    Particle * auxParticle;

    int numParticles = globalRepositoryList.count();

    for (int i = 0; i < numParticles; i++)
    {
        auxParticle = new Particle(*globalRepositoryList.at(i));

        if(!nGrid->particleInsideGrid(auxParticle))
        {
            // TODO: revisar esto:
            qDebug("%%%%%%%% el individuo no pertenece a la grid");
            QMessageBox msg;
            msg.setText("%%%%%%%% particula no pertenece a la grid\nESTO NO DEBERIA OCURRIR!!!");
            msg.exec();
        }
        else
        {
            nGrid->addParticleToGrid(auxParticle);
        }
    }
}


Particle * Simulation::getLeader()
{
    QList<Cell *> populatedCells = nGrid->getPopulatedCellList();

    Particle * particleToReturn;

    int cellCount = 0;
    int leftCellCount = 0;
    int rightCellCount = 0;
    int totalCellCount = 0;

    for (int i = 0; i < populatedCells.count(); i++)
    {
        Cell * cell = populatedCells.at(i);
        cellCount = cell->getCount();

        // obtener el contador de la celda de la izquierda
        //leftCellCount = getLeftCellCount(cell);


        // obtener el contador de la celda de la derecha
        //rightCellCount = getRightCellCount(cell);


        // numero total de particulas para el grupo
        totalCellCount = leftCellCount + cellCount + rightCellCount;
        cell->setNeighboursParticlesCount(totalCellCount);
    }
    // ordenar la lista de particulas por grupo de celda de menor a mayor
    qSort(populatedCells.begin(), populatedCells.end(), cellLessThan);

    // lista con un solo elemento
    if (populatedCells.count() == 1)
    {
        particleToReturn = populatedCells.at(0)->getRandomParticle();
    }
    else // lista con dos o mas elementos
    {
        int cellToGetParticle = getIndexOfCellToSelectParticle(populatedCells);
        particleToReturn = populatedCells.at(cellToGetParticle)->getRandomParticle();
    }

    return particleToReturn;
}

int Simulation::getIndexOfCellToSelectParticle(QList<Cell *> populatedCells)
{
    Cell * first;
    Cell *  next;
    int indexNext = 0;

    first = populatedCells.at(0);

    QList<int> sameCountList;
    sameCountList.append(0);

    while(true)
    {
        indexNext++;
        // ultimo elemento de la lista
        if ( (indexNext) == populatedCells.count() )
        {
            break;
        }
        next = populatedCells.at(indexNext);
        if (first->getNeighboursParticlesCount() == next->getNeighboursParticlesCount())
        {
            sameCountList.append(indexNext);
        }
        else
        {
            break;
        }
    }

    return getRandom(0, sameCountList.count()-1);
}



int Simulation::getRandom(int l, int h)
{
    int low = l;
    int high = h;
    return qrand() % ((high + 1) - low) + low;
}

