#ifndef PARTICLEREPOSITORY_H
#define PARTICLEREPOSITORY_H

#include <QList>
#include <QHash>


#include "particle.h"

/**
 * @brief Clase que abstrae el repositorio de particulas.
 *
 * Se mantiene una tabla hash con el identificador unico de cada particula
 * y su correspondiente lista de posiciones. Esta tabla corresponde al
 * repositorio local de posiciones no dominadas.
 *
 *
 */
class ParticleRepository
{

private:

    /**
     * @brief Tabla hash para mantener el identificador de la particula y su correspondiente
     * lista de posiciones
     */
    QHash<int, QList<Particle*> > personalRepository;

public:

    /**
     * @brief Constructor de la clase ParticleRepository
     */
    ParticleRepository();

    /**
     * @brief Destructor de la clase ParticleRepository
     */
    ~ParticleRepository();

    /**
     * @brief Agrega la particula pasada como primer argumento en la lista de particulas
     * pasada como segundo argumento
     * @param p particula a agregar
     * @param list lista de particulas donde se agrega la particula
     */
    void addParticle(Particle * p, QList<Particle*> list);

    /**
     * @brief Retorna la lista de particulas para la particula con ID pasado como argumento
     * @param particleId identificador de la particula
     * @return lista de particulas para la particula con ID pasado como argumento
     */
    QList<Particle*> getParticlePersonalRepository(int particleId);


    /**
     * @brief Retorna si la particula pasada como argumento esta dominada por alguna existente
     * en el repositorio
     * @param newParticle particula a evaluar
     * @return si la particula pasada como argumento esta dominada por alguna existente
     */
    bool isNewParticleDominatedByRepository(Particle * newParticle);


    /**
     * @brief Retorna si el primer argumento domina al segundo
     * @param xj particula
     * @param xi particula
     * @return Retorna si el primer argumento domina al segundo
     */
    bool particleDominate(Particle * xj, Particle * xi);

    /**
     * @brief Retorna una particula aleatoria del repositorio local de la particula
     * @param particleId particula para seleccionar una posicion
     * @return una particula aleatoria del repositorio local de la particula
     */
    Particle * getRandomLocalFromParticle(int particleId);


    void addNonDominatedParticle(Particle * particle);


    void eliminateDominatedParticles();

    bool isParticleInLocalRepository(Particle * particle);

};

#endif // PARTICLEREPOSITORY_H
