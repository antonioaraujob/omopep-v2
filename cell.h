#ifndef CELL_H
#define CELL_H

#include <QList>

#include "particle.h"

/**
 * @brief Clase Cell que abstrae una celda de la rejilla
 *
 * Una Cell almacenará una lista de particulas de acuerdo a los valores de las funciones
 * objetivo en unos subintervalos específicos.
 */
class Cell
{

private:

    /**
    * @brief Lista de particulas cuyos valores de funciones objetivo caen en los
    * subintervalos correspondiente a la celda.
    */
    QList<Particle *> cellParticleList;

    /**
     * @brief Coordenada x de la celda (subintervalo de descubierta)
     */
    int f1;

    /**
     * @brief Coordenada y de la celda (subintervalo de latencia)
     */
    int f2;

    /**
     * @brief Contador de particulas personal y vecinos
     *
     * Almacena el numero de particulas de la celda mas la cantidad de particulas
     * de las celdas vecinas izquierda y derecha
     */
    int neighboursParticlesCount;

public:

    /**
     * @brief Constructor de la clase Cell
     */
    Cell();

    /**
     * @brief Desstructor de la clase Cell
     */
    ~Cell();

    /**
     * @brief Retorna el número de particulas de la celda
     *
     * @return Número de particulas de la celda
     */
    int getCount();

    /**
     * @brief Retorna la particula de la lista con indice el valor pasado como argumento.
     * @param index indice del individuo en la lista de individuos de Cell
     * @return Individuo con indice pasado como argumento
     */
    Particle * getParticle(int index);


    /**
     * @brief Agrega un individuo a la lista de individuos de la celda
     * @param individual individuo a agregar a la lista.
     * @param indexF1 indice del subintervalo para F1
     * @param indexF1 indice del subintervalo para F2
     */
    void addParticle(Particle * particle,  int indexF1, int indexF2);

    /**
     * @brief Retorna la lista de particulas de la Cell
     *
     * @return Lista de particulas de la Cell
     */
    QList<Particle *> getParticleList();

    /**
     * @brief Sobrecarga del operador de asignacion para una celda
     * @param cell a asignar
     * @return Cell asignada
     */
    Cell& operator = (const Cell &cell);

    /**
    * @brief Retorna el valor del indice del subintervalo de F1 dentro de la rejilla
    * @return valor del indice del subintervalo de F1 dentro de la rejilla
    */
    int getSubintervalF1();

    /**
    * @brief Retorna el valor del indice del subintervalo de F2 dentro de la rejilla
    * @return valor del indice del subintervalo de F1 dentro de la rejilla
    */
    int getSubintervalF2();

    /**
     * @brief Retorna una particula aletoria de la celda
     * @return una particula aletoria de la celda
     */
    Particle * getRandomParticle();

    /**
     * @brief Asigna el contador de particulas local y de vecinos
     * @param count contador a asignar
     */
    void setNeighboursParticlesCount(int count);

    /**
     * @brief Retorna el contador de particulas local y de vecinos
     * @return el contador de particulas local y de vecinos
     */
    int getNeighboursParticlesCount();


};

#endif // CELL_H
