#ifndef GLOBALREPOSITORY_H
#define GLOBALREPOSITORY_H

#include <QList>

#include "particle.h"


/**
 * @brief Clase que modela el repositorio global de la optimizacion multiobjetivo
 * por enjambre de particulas
 */
class GlobalRepository
{

private:

    /**
     * @brief Lista de particulas no dominadas
     *
     * Corresponde a la solución del algoritmo PSO
     */
    QList<Particle *> nonDominatedParticlesList;




public:

    /**
     * @brief GlobalRepository
     */
    GlobalRepository();

    /**
     * @brief Destructor de la clase
     */
    ~GlobalRepository();


    /**
     * @brief Retorna la lista con las particulas no dominadas del repositorio global
     * @return Lista de particulas no dominadas del repositorio global
     */
    QList<Particle *> getRepositoryList();

    /**
     * @brief Agregar la particula pasada como argumento
     *
     * @param p particula no dominada que se debe agregar al repositorio global
     */
    void addNonDominatedParticle(Particle * p);

    /**
     * @brief Retorna si la particula pasada como argumento ya se encuentra en el repositorio
     * @param particle particula a verificar si ya existe en el repositorio
     * @return si la particula pasada como argumento ya se encuentra en el repositorio
     */
    bool isParticleInGlobalRepository(Particle * particle);


    /**
     * @brief Retorna si el primer argumento domina al segundo
     * @param xj particula
     * @param xi particula
     * @return Retorna si el primer argumento domina al segundo
     */
    bool particleDominate(Particle * xj, Particle * xi);

    /**
     * @brief Retorna si la particula pasada como argumento esta dominada por alguna del
     * repositorio
     * @param newParticle particula a evaluar si es dominada
     * @return si la particula pasada como argumento esta dominada por alguna del repositorio
     */
    bool isNewParticleDominatedByGlobalRepository(Particle * newParticle);

    /**
     * @brief Elimina las particulas dominadas del repositorio
     */
    void eliminateDominatedParticles();


    /**
     * @brief Retorna una particula aleatoria del repositorio
     * @return una particula aleatoria del repositorio
     */
    Particle * getRandomParticle();

    /**
     * @brief Retorna una particula lider de acuerdo al algoritmo modificado de seleccion
     * @return una particula lider de acuerdo al algoritmo modificado de seleccion
     */
    Particle * getLeader();

};

#endif // GLOBALREPOSITORY_H
