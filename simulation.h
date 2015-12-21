#ifndef SIMULATION_H
#define SIMULATION_H


#include <QList>

#include "particle.h"
#include "globalrepository.h"
#include "particlerepository.h"
#include "grid.h"


/**
 * @brief Clase que modela la simulacion para el algoritmo cultural
 */
class Simulation
{

private:

    /**
     * @brief Parametro cognitivo
     */
    int cognitive;

    /**
     * @brief Parametro social
     */
    int social;

    /**
     * @brief Parametro inertia
     */
    double inertia;

    /**
     * @brief Parametro maxima velocidad
     */
    int maxSpeed;

    /**
     * @brief particles
     */
    int particles;

    /**
     * @brief iterations
     */
    int iterations;

    /**
     * @brief currentIteration
     */
    int currentIteration;

    /**
     * @brief Contador de identificador de una particula
     *
     * Usado para asignar un identificador unico para cada particula que se creo en la simulacion.
     */
    static int particleIdCounter;


    /**
     * @brief Lista de particulas
     */
    QList<Particle *> particleList;


    /**
     * @brief Repositorio global de particulas no dominadas
     */
    GlobalRepository * gRepository;

    /**
     * @brief Repositorio local de particulas no dominadas
     */
    ParticleRepository * pRepository;

    /**
     * @brief Mejor particula global
     */
    Particle * bestGlobal;

    /**
     * @brief Mejor particula local
     */
    Particle * bestLocal;

    /**
     * @brief Rejilla de particulas globales no dominadas
     */
    Grid * nGrid;

    /**
     * @brief limite inferior de la funcion objetivo 1
     */
    double lF1;

    /**
     * @brief limite superior de la funcion objetivo 1
     */
    double uF1;

    /**
     * @brief limite inferior de la funcion objetivo 2
     */
    double lF2;

    /**
     * @brief limite superior de la funcion objetivo 2
     */
    double uF2;

    /**
     * @brief Numero de subintervalos para la grid
     */
    int gridSubintervalsNumber;

    /**
     * @brief Verdadero si se va a utilizar el algoritmo de seleccion de lider modificado
     * del repositorio global de particulas
     */
    bool selectionModified;


    /**
     * @brief retorna el indice de la celda que se debe usar para seleccionar una particula
     * @param populatedCells lista de celdas
     * @return indice de la celda que se debe usar para seleccionar una particula
     */
    int getIndexOfCellToSelectParticle(QList<Cell *> populatedCells);

public:

    /**
     * @brief Constructor de la clase Simulation
     *
     * @param cognitiveParameter valor del parametro cognitivo
     * @param socialParameter valor del parametro social
     * @param inertiaParameter valor del parametro de inercia
     * @param maxSpeed valor del parametro velocidad maxima
     * @param particles valor del parametro numero de particulas
     * @param maxIterations valor del parametro numero de iteraciones
     ** @param selection especifica si se usa el algoritmo de seleccion de lider modificado
     * @param subintervalsGrid numero de subintervalos para la grid
     */
    Simulation(int cognitiveParameter, int socialParameter, double inertiaParameter, int maxSpeedParameter,
               int particlesParameter, int maxIterations, bool selection, int subintervalsGrid);

    /**
     * @brief Destructor de la clase
     */
    ~Simulation();

    /**
     * @brief Inicializa las particulas de la simulacion
     */
    void initializeParticles();

    /**
     * @brief Obtiene los limites inferiores y superiores de las funciones objetivo 1 y 2
     * de acuerdo al contenido de la lista de particulas iniciales
     */
    void getInitialGridBoundaries();

    /**
     * @brief Obtiene los limites inferiores y superiores de las funciones objetivo 1 y 2
     * de acuerdo al contenido de al repositorio global de particulas
     */
    void getNewGridBoundaries();

    /**
     * @brief Inicializa la rejilla
     */
    void intializeGrid();

    /**
     * @brief Ejecuta el proceso de actualizacion de las particulas
     * (ejecucion de las iteraciones del algoritmo)
     */
    void updateParticles();


    /**
     * @brief Retorna un nuevo identificador para una particula creado
     * @return Retorna un nuevo identificador para una particula creado
     */
    static int getNewParticleId();

    /**
     * @brief Incrementa el contador de iteraciones
     */
    void incrementIteration();

    /**
     * @brief Retorna el contador de iteraciones con el valor actual
     * @return el contador de iteraciones con el valor actual
     */
    int getCurrentIteration();

    /**
     * @brief Retorna si se debe detener la simulacion porque se cumplio la condicion
     * de parada
     * @return si se debe detener la simulacion porque se cumplio la condicion de parada
     */
    bool stopEvolution();

    /**
     * @brief Retorna el parametro cognitivo
     * @return parametro cognitivo
     */
    int getCognitiveParameter();

    /**
     * @brief Retorna el parametro social
     * @return parametro social
     */
    int getSocialParameter();

    /**
     * @brief Retorna el parametro de inertia
     * @return parametro de inertia
     */
    double getInertiaParameter();

    /**
     * @brief Retorna el parametro maxima velocidad
     * @return parametro maxima velocidad
     */
    int getMaxSpeedParameter();

    /**
     * @brief Retorna un valor aleatorio entre [0,1]
     * @return valor aleatorio entre [0,1]
     */
    double getRandomUniform();

    /**
     * @brief Imprime las particulas del repositorio global
     */
    void printGlobalRepository();

    /**
     * @brief Retorna el repositorio global
     * @return el repositorio global
     */
    GlobalRepository * getGlobalRepository();

    /**
     * @brief Establece la seleccion de lider por el algoritmo modificado
     * @param selection seleccion
     */
    void setSelectionModified(bool selection);

    /**
     * @brief Retorna si se utiliza la seleccion de lider por el algoritmo modificado
     * @return si se utiliza la seleccion de lider por el algoritmo modificado
     */
    bool getSelectionModified();

    /**
     * @brief Agrega la particula pasada como argumento a la grid.
     *
     * Si la particula cae fuera de los rangos de la grid la reconstruye primero antes de
     * agregarla
     * @param newParticle particula a agregar en la grid
     */
    void addParticleToGrid(Particle * newParticle);

    /**
     * @brief Actualiza el contenido de la grid con la lista de particulas del
     * repositorio global
     * @param globalRepositoryList lista de particulas no dominadas del repositorio global
     */
    void updateGrid(QList<Particle *> globalRepositoryList);

    /**
     * @brief Retorna un lider de acuerdo a la modificacion del algoritmo con el uso de la rejilla
     */
    Particle *getLeader();

    /**
     * @brief Retorna un numero aleatorio entero entre el intervalo pasado como argumento
     * @param l valor inferior del rango
     * @param h valor superior del rango
     * @return numero aleatorio entero entre el intervalo pasado como argumento
     */
    int getRandom(int l, int h);
};

#endif // SIMULATION_H
