#include "particlerepository.h"

ParticleRepository::ParticleRepository()
{
}

ParticleRepository::~ParticleRepository()
{

}

void ParticleRepository::addParticle(Particle * p, QList<Particle *> list)
{

    //personalRepository.append(p);

    int particleId = p->getParticleId();
    //personalRepository.value(particleId).append(p);
    personalRepository.insert(particleId, list);

}


QList<Particle*> ParticleRepository::getParticlePersonalRepository(int particleId)
{
    return personalRepository.value(particleId);
}


bool ParticleRepository::isNewParticleDominatedByRepository(Particle * newParticle)
{
    qDebug("...ParticleRepository::isNewParticleDominatedByRepository");
    bool isDominated = false;

    Particle * nonDominatedParticle;

    QList<Particle*> localRepository = personalRepository.value(newParticle->getParticleId());


    for (int i = 0; i < localRepository.count(); i++)
    {
        //qDebug("   dentro del for");
        nonDominatedParticle = localRepository.at(i);
        if (particleDominate(nonDominatedParticle, newParticle))
        {
            //qDebug("  newParticle dominada por el repositorio local");
            isDominated = true;
            break;
        }
    }
    //qDebug("   salio del for");
    return isDominated;
}

bool ParticleRepository::particleDominate(Particle * xj, Particle * xi)
{
    //qDebug("... ... Simulation::individualDominate");

    // a solution xj is said to dominate another solution xi, and we write xj <| xi if both
    // the following conditions are true:
    //
    // condition a: Fi(xj) <= Fi(xi) to i E 1,2
    //
    // confition b: Exists j 1, 2 such that Fj(xj) < Fj(xi)

    bool conditionA = false;
    bool conditionB = false;


    // condition a
    if ( (xj->getPerformanceDiscovery() >= xi->getPerformanceDiscovery()) &&
         (xj->getPerformanceLatency() <= xi->getPerformanceLatency()) )
    {
        conditionA = true;
    }

    // condition b
    if ( (xj->getPerformanceDiscovery() > xi->getPerformanceDiscovery()) ||
         (xj->getPerformanceLatency() < xi->getPerformanceLatency()) )
    {
        conditionB = true;
    }

    if ( (conditionA) && (conditionB) )
    {
        return true;
    }else
    {
        return false;
    }
}


Particle * ParticleRepository::getRandomLocalFromParticle(int particleId)
{
    QList<Particle*> particleLocalList = personalRepository.value(particleId);

    int low = 0;
    int high = particleLocalList.count()-1;
    int randomPosition = qrand() % ((high + 1) - low) + low;
    return particleLocalList.at(randomPosition);
}


void ParticleRepository::addNonDominatedParticle(Particle * particle)
{

    if (!isParticleInLocalRepository(particle))
    {
        QList<Particle*> localList = personalRepository.value(particle->getParticleId());

        Particle * newParticle = new Particle(*particle);

        //QList<int> particleIdToRemove;
        QList<Particle *> particleIdToRemove;

        Particle * particleTmp;

        for (int i = 0; i < localList.count(); i++)
        {
            particleTmp = localList.at(i);

            if (particleDominate(newParticle, particleTmp))
            {
                //particleIdToRemove.append(particleTmp->getParticleId());
                particleIdToRemove.append(particleTmp);
            }
        }

        // eliminar los individuos marcados del repositorio
        for (int j = 0; j < particleIdToRemove.count(); j++)
        {
            for (int k = 0; k < localList.count(); k++)
            {
                //if (localList.at(k)->getParticleId() == particleIdToRemove.at(j))
                if (localList.at(k) == particleIdToRemove.at(j))
                {
                    localList.removeAt(k);
                    break;
                }
            }
        }

        localList.append(newParticle);
        personalRepository.insert(particle->getParticleId(),localList);
    }
}

void ParticleRepository::eliminateDominatedParticles()
{

}


bool ParticleRepository::isParticleInLocalRepository(Particle * particle)
{
    qDebug("...ParticleRepository::isParticleInLocalRepository");
    Particle * alreadyInsertedParticle;

    bool exist = false;

    QList<Particle*> localList = personalRepository.value(particle->getParticleId());

    for (int i = 0; i < localList.count(); i++)
    {
        //qDebug("   dentro del for");
        alreadyInsertedParticle = localList.at(i);

        bool sameParameters = true;

        for (int i = 0; i < particle->getNumberOfParameters(); i++)
        {
            if (particle->getParameter(i) != alreadyInsertedParticle->getParameter(i))
            {
                sameParameters = false;
                break;
            }
        }
        if ( particle->getParticleId() == alreadyInsertedParticle->getParticleId() && (sameParameters) )
        {
            exist = true;
        }
    }
    //qDebug("   antes de salir");
    return exist;

}
