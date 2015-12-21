#ifndef Grid_H
#define Grid_H



#include "gridsubinterval.h"
#include "cell.h"
#include "particle.h"


/**
 * @brief Clase que modela la rejilla (grid)
 */
class Grid
{

private:

    /**
     * @brief Limite inferior de la funcion objetivo 1
     */
    double lowerF1;

    /**
     * @brief Limite superior de la funcion objetivo 2
     */
    double upperF1;

    /**
     * @brief Limite inferior de la funcion objetivo 2
     */
    double lowerF2;

    /**
     * @brief Limite superior de la funcion objetivo2
     */
    double upperF2;

    /**
     * @brief Numero de intervalos para construir la rejilla
     */
    int subIntervalNumber;

    /**
     * @brief Lista de subintervalos de la funcion objetivo 1
     */
    QList<GridSubInterval *> subIntervalListF1;

    /**
     * @brief Lista de subintervalos de la funcion objetivo 2
     */
    QList<GridSubInterval *> subIntervalListF2;

    /**
     * @brief Rejilla del espacio de creencias
     *
     * En el eje X estaran los valores de la funcion objetivo 1 descubierta
     * En el eje Y estaran los valores de la funcion objetivo 2 latencia
     */
    Cell ** ptrGrid;

    /**
     * @brief Lista de celdas de la rejilla con al menos un individuo
     */
    QList<Cell *> populatedCellList;

public:

    /**
     * @brief Constructor de la clase Grid
     *
     * @param intervals corresponde al numero de intervalos en que se divide la rejilla (intervals x intervals)
     * @param nPhenotypicPart corresponde a la parte normativa fenotipica
     */
    Grid(int subIntervals, double lF1, double uF1, double lF2, double uF2);

    /**
     * @brief Destructor de la clase Grid
     */
    ~Grid();

    /**
     * @brief Retorna el numero de intervalos para dividir la rejilla
     * @return intervalos para dividir la rejilla
     */
    int getSubIntervalNumber();

    /**
     * @brief Agrega una particula a la grid.
     *
     * Se incrementa el contador en la celda de la grid correspondiente a la particula.
     *
     * @param par particula a agregar
     */
    void addParticleToGrid(Particle * particle);

    /**
     * @brief Retorna el contador de particulas en una celda (x,y)
     *
     * @param f1 valor de la funcion objetivo 1
     * @param f2 valor de la funcion objetivo 2
     *
     * @return contador de particulas en la celda
     */
    int getCount(int f1, int f2);

    /**
     * @brief Construye los subintervalos de la rejilla para la funcion objetivo 1
     */
    void buildSubintervalsF1(double lF1, double uF1);

    /**
     * @brief Construye los subintervalos de la rejilla para la funcion objetivo 1
     */
    void buildSubintervalsF2(double lF2, double uF2);


    /**
     * @brief Retorna el indice del subintervalo de la funcion objetivo 1 a la que pertenece el
     * valor pasado como argumento.
     *
     * El indice retornado sigue la enumeracion desde 0 como en los vectores de C.
     *
     * @param value valor del cual se desea conocer el subintervalo al que pertenece
     * @return indice del subintervalo al que pertenece el valor pasado como argumento.
     */
    int getF1SubintervalIndex(double value);

    /**
     * @brief Retorna el indice del subintervalo de la funcion objetivo 2 a la que pertenece el
     * valor pasado como argumento.
     *
     * El indice retornado sigue la enumeracion desde 0 como en los vectores de C.
     *
     * @param value valor del cual se desea conocer el subintervalo al que pertenece
     * @return indice del subintervalo al que pertenece el valor pasado como argumento.
     */
    int getF2SubintervalIndex(double value);

    /**
     * @brief Imprime el contenido de la rejilla
     */
    void printGrid();

    /**
     * @brief Retorna verdadero si la particula pasada como argumento esta en la rejilla
     * @param particle Particula que se desea encontrar en la rejilla
     * @return Verdadero si la particula esta en la rejilla
     */
    bool particleInsideGrid(Particle * particle);

    /**
     * @brief Retorna el contador de la celda en la que se encuentra la particula pasada como
     * argumento
     * @param particle Particula de la que se desea determinar el contador de la celda en la rejilla
     * @return Numero de elementos en la celda a la que pertenece la particula
     */
    int getCountOfCell(Particle * particle);

    /**
    * @brief Retorna la lista de celdas pobladas (con mas de una particula) en la rejilla
    * @return Retorna la lista de celdas pobladas (con mas de una particula) en la rejilla
    */
    QList<Cell*> getPopulatedCellList();

    /**
     * @brief Retorna si la celda identificada con las coordenadas pasadas como argumento ya
     * se encuentra insertada en la lista de celdas que tienen al menos un individuo
     * @param indexF1 indice del subintervalo para F1
     * @param indexF2 indice del subintervalo para F2
     * @return si la celda identificada con las coordenadas pasadas como argumento ya
     */
    bool isCellInCellList(int indexF1, int indexF2);

    /**
     * @brief Retorna un lider de acuerdo a la modificacion del algoritmo con el uso de la rejilla
     */
    Particle * getLeader();

    int getIndexOfCellToSelectParticle(QList<Cell *> populatedCells);

    /**
     * @brief Retorna un numero aleatorio entero entre el intervalo pasado como argumento
     * @param l valor inferior del rango
     * @param h valor superior del rango
     * @return numero aleatorio entero entre el intervalo pasado como argumento
     */
    int getRandom(int l, int h);

    int getLeftAndRightCellCount(Cell * cell);






};

#endif // Grid_H
