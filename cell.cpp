#include "cell.h"

Cell::Cell()
{
    f1 = 0;
    f2 = 0;
    neighboursParticlesCount = 0;
    cellParticleList.clear();
}

Cell::~Cell()
{

}

int Cell::getCount()
{
    return cellParticleList.count();
}

Particle * Cell::getParticle(int index)
{
    return cellParticleList.at(index);
}

void Cell::addParticle(Particle *particle, int indexF1, int indexF2)
{
    cellParticleList.append(particle);
    f1 = indexF1;
    f2 = indexF2;

    neighboursParticlesCount++;
}


QList<Particle *> Cell::getParticleList()
{
    return cellParticleList;
}


Cell& Cell::operator = (const Cell &cell)
{
    if (&cell != this)
    {
        this->cellParticleList = cell.cellParticleList;
        this->f1 = cell.f1;
        this->f2 = cell.f2;
        this->neighboursParticlesCount = cell.neighboursParticlesCount;
    }
    return *this;
}

int Cell::getSubintervalF1()
{
    return f1;
}

int Cell::getSubintervalF2()
{
    return f2;
}

Particle * Cell::getRandomParticle()
{
    //if (cellParticleList.count() == 0)
    //{
    //    return 0;
    //}
    int low = 0;
    int high = cellParticleList.count()-1;
    int index = qrand() % ((high + 1) - low) + low;
    return cellParticleList.at(index);
}

void Cell::setNeighboursParticlesCount(int count)
{
    neighboursParticlesCount = count;
}

int Cell::getNeighboursParticlesCount()
{
    return neighboursParticlesCount;
}
