#ifndef EXECUTIONTHREAD_H
#define EXECUTIONTHREAD_H

#include <QThread>


/**
 * @brief Clase ExecutionThread que abstrae un thread para eejecutar el algoritmo PSO
 * y permitir que la interfaz grafica no se quede congelada.
 */
class ExecutionThread : public QThread
{
    Q_OBJECT

protected:
    void run();

public:
    ExecutionThread();


};

#endif // EXECUTIONTHREAD_H
