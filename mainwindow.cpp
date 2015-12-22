#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include <QValidator>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QStringListModel>
#include <QVector>


#include "particle.h"
#include "emulator.h"


/**
 * @brief Funcion de comparacion de particulas con respecto al valor de desempeno de latencia
 * @param p1 particula 1 a comparar
 * @param p2 particula 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de latencia
 */
inline static bool xLessThanLatency(Particle *p1, Particle *p2)
{
    return p1->getPerformanceLatency() < p2->getPerformanceLatency();
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(832, 717);

    // Validadores para los parametros del algoritmo

    QValidator * validatorCognitiveParameter = new QIntValidator(0, 9, this);
    ui->lineEditCognitiveParameter->setValidator(validatorCognitiveParameter);
    ui->lineEditCognitiveParameter->setToolTip("[0..9]");

    QValidator * validatorSocialParameter = new QIntValidator(0, 9, this);
    ui->lineEditSocialParameter->setValidator(validatorSocialParameter);
    ui->lineEditSocialParameter->setToolTip("[0..9]");

    QDoubleValidator * validatorInertia = new QDoubleValidator(0.0, 1.0, 2, this);
    validatorInertia->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditInertiaParameter->setValidator(validatorInertia);
    ui->lineEditInertiaParameter->setToolTip("[0..1]");

    QValidator * validatorSpeedParameter = new QIntValidator(1, 9, this);
    ui->lineEditMaxSpeedParameter->setValidator(validatorSpeedParameter);
    ui->lineEditMaxSpeedParameter->setToolTip("[1..9]");

    QValidator * validatorParticlesParameter = new QIntValidator(1, 400, this);
    ui->lineEditParticlesParameter->setValidator(validatorParticlesParameter);
    ui->lineEditParticlesParameter->setToolTip("[1..400]");

    QValidator * validatorIterationsParameter = new QIntValidator(1, 100000, this);
    ui->lineEditIterationsParameter->setValidator(validatorIterationsParameter);
    ui->lineEditIterationsParameter->setToolTip("[1..100000]");

    QValidator * validatorSubintervalsParameter = new QIntValidator(2, 10, this);
    ui->lineEditSubintervals->setValidator(validatorSubintervalsParameter);
    ui->lineEditSubintervals->setToolTip("[2..10]");

    connect(ui->pushButtonExecute, SIGNAL(clicked()), this, SLOT(executeAlgorithm()));
    connect(ui->pushButtonCompareAlgorithms, SIGNAL(clicked()), this, SLOT(compareAlgorithms()));

    connect(ui->checkBoxGrid, SIGNAL(stateChanged(int)), this, SLOT(activateGridSelection(int)));
    ui->label_9->setEnabled(false);
    ui->lineEditSubintervals->setEnabled(false);

    connect(ui->checkBoxComparation, SIGNAL(stateChanged(int)), this, SLOT(activateComparationButton(int)));
    ui->pushButtonCompareAlgorithms->setEnabled(false);

    ui->label_PSO_generico->setVisible(false);
    ui->psoGenericNumber->setVisible(false);
    ui->label_PSO_modificado->setVisible(false);
    ui->psoModifiedNumber->setVisible(false);
    ui->psoGenericTime->setVisible(false);
    ui->psoModifiedTime->setVisible(false);
    ui->label_tiempo_generico->setVisible(false);
    ui->label_tiempo_modificado->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::executeAlgorithm()
{
    qDebug("...MainWindow::executeAlgorithm()");

    if (!validateFields())
    {
        return;
    }

    // prueba de emulador
    Emulator emulator;

    //qsrand((uint)QTime::currentTime().msec());

    double inertia = ui->lineEditInertiaParameter->text().toDouble();
    qDebug("valor de inercia: %f",inertia);

    simulation = new Simulation(ui->lineEditCognitiveParameter->text().toInt(),
                                ui->lineEditSocialParameter->text().toInt(),
                                ui->lineEditInertiaParameter->text().toDouble(),
                                ui->lineEditMaxSpeedParameter->text().toInt(),
                                ui->lineEditParticlesParameter->text().toInt(),
                                ui->lineEditIterationsParameter->text().toInt(),
                                ui->checkBoxGrid->isChecked(),
                                ui->lineEditSubintervals->text().toInt());

    // inicializar las particulas
    // (la inicializacion de los repositorios (global y locales) tambien se ejecuta en este paso)
    simulation->initializeParticles();
    qDebug("***********particulas inicializadas**************");


    // prueba de imprimir los individuos inicializados del repositorio global
    //genericAlgorithmSolutions = simulation->getGlobalRepository()->getRepositoryList();
    //plotSolutions();
    //return;



    if (simulation->getSelectionModified())
    {
        qDebug("****************************simulation->getSelectionModified()***************");
        // inicializar la grid
        simulation->intializeGrid();
        qDebug("***********grid inicializada**************");
    }



    // repetir por el numero maximo de generaciones
    //do{
    while(!simulation->stopEvolution()) // fin de la repeticion
    {
        // actualizar las particulas
        simulation->updateParticles();
        qDebug("***********particulas actualizadas**************");


        // evaluar las particulas



        // incrementar el valor de la iteracion
        simulation->incrementIteration();
        qDebug("...fin de iteracion: %d", simulation->getCurrentIteration());

    //}while(!simulation->stopEvolution()); // fin de la repeticion
    }
    qDebug("...terminó la simulación.");

    simulation->printGlobalRepository();

    // poblar la lista de individuos no dominados del archivo externo
    populateListView();

    // crear directorio de resultados
    QString resultsDirectory = createResultsDirectory();

    if (simulation->getSelectionModified())
    {
        modificatedAlgorithmSolutions = simulation->getGlobalRepository()->getRepositoryList();

        // ordenar la lista en orden ascendente de acuerdo a la latencia (F2)
        qSort(modificatedAlgorithmSolutions.begin(), modificatedAlgorithmSolutions.end(), xLessThanLatency);

        // escribir en un archivo los individuos del frente de pareto encontrado en un archivo
        reportParticleAsFile(modificatedAlgorithmSolutions, resultsDirectory, "particulasFrenteParetoModificado");
    }
    else
    {
        genericAlgorithmSolutions = simulation->getGlobalRepository()->getRepositoryList();

        // ordenar la lista en orden ascendente de acuerdo a la latencia (F2)
        qSort(genericAlgorithmSolutions.begin(), genericAlgorithmSolutions.end(), xLessThanLatency);

        // escribir en un archivo los individuos del frente de pareto encontrado en un archivo
        reportParticleAsFile(genericAlgorithmSolutions, resultsDirectory, "particulasFrenteParetoGenerico");
    }



    // generar el grafico
    plotSolutions();

/*
    if (ui->checkBoxComparation->isChecked())
    {
        if (firstExecution)
        {
            firstExecution = false;
        }
        else
        {
            setupCustomPlot2(ui->customPlot);
        }

    }
    else
    {
        setupCustomPlot(ui->customPlot);
    }
*/

}


bool MainWindow::validateFields()
{
    bool validate = true;

    QMessageBox msg;
    QString message;
    message.append("Los siguientes campos deben estar asignados:\n");

    if (ui->lineEditCognitiveParameter->text().isEmpty())
    {
        message.append("Parámetro cognitivo\n");
        validate = false;
    }
    if (ui->lineEditSocialParameter->text().isEmpty())
    {
        message.append("Parámetro social\n");
        validate = false;
    }
    if (ui->lineEditInertiaParameter->text().isEmpty())
    {
        message.append("Parámetro de inercia\n");
        validate = false;
    }
    if (ui->lineEditMaxSpeedParameter->text().isEmpty())
    {
        message.append("Velocidad máxima\n");
        validate = false;
    }
    if (ui->lineEditParticlesParameter->text().isEmpty())
    {
        message.append("Número de partículas\n");
        validate = false;
    }
    if (ui->lineEditIterationsParameter->text().isEmpty())
    {
        message.append("Número de iteraciones\n");
        validate = false;
    }

    msg.setText(message);

    if (!validate)
        msg.exec();

    return validate;
}

void MainWindow::populateListView()
{
    QStringList individuals;

    QString aux;
    for (int i = 0; i < simulation->getGlobalRepository()->getRepositoryList().count(); i++)
    {
        aux.append(simulation->getGlobalRepository()->getRepositoryList().at(i)->getParticleAsQString());
        individuals << aux;
        aux.clear();
    }


    QStringListModel *model = new QStringListModel();
    model->setStringList(individuals);

    ui->listViewBestNonDominatedParticles->setModel(model);


    int numberOfParticles = simulation->getGlobalRepository()->getRepositoryList().count();
    ui->labelNonDominatedNUmber->setText(QString::number(numberOfParticles));
}


void MainWindow::setupCustomPlot(QCustomPlot *customPlot)
{
/*
    int count = simulation->getGlobalRepository()->getRepositoryList().count();
    QVector<double> discovery(count), latency(count);

    int i = 0;

    Particle * particle;
    //for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    for (int z=simulation->getGlobalRepository()->getRepositoryList().count()-1; z>=0; z-- )
    {
        particle = simulation->getGlobalRepository()->getRepositoryList().at(z);
        discovery[i] = particle->getPerformanceDiscovery();
        latency[i] = particle->getPerformanceLatency();
        i++;
    }
*/

    //customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);


    customPlot->clearGraphs();

    Particle * particle;

    qSort(genericAlgorithmSolutions.begin(), genericAlgorithmSolutions.end(), xLessThanLatency);

    int count = genericAlgorithmSolutions.count();
    QVector<double> discovery(count), latency(count);
    for (int i = 0; i < count; i++)
    {
        particle = genericAlgorithmSolutions.at(i);
        discovery[i] = particle->getPerformanceDiscovery();
        latency[i] = particle->getPerformanceLatency();
    }

    qSort(modificatedAlgorithmSolutions.begin(), modificatedAlgorithmSolutions.end(), xLessThanLatency);

    int countModified = modificatedAlgorithmSolutions.count();
    QVector<double> discoveryModified(countModified), latencyModified(countModified);
    if (ui->checkBoxGrid->isChecked())
    {
        for (int i = 0; i < countModified; i++)
        {
            particle = modificatedAlgorithmSolutions.at(i);
            discoveryModified[i] = particle->getPerformanceDiscovery();
            latencyModified[i] = particle->getPerformanceLatency();
        }
    }

    // create graph and assign data to it:
    customPlot->addGraph();



    if (ui->checkBoxGrid->isChecked())
    {
        customPlot->graph(0)->setName("PSO modificado");
        customPlot->graph(0)->setData(discoveryModified, latencyModified);
        customPlot->graph(0)->setPen(QPen(Qt::green)); // line color blue for first graph
        customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    }
    else
    {
        customPlot->graph(0)->setName("PSO generico");
        customPlot->graph(0)->setData(discovery, latency);
        customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
        customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    }

    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);



    if (ui->checkBoxComparation->isChecked())
    //if (modified)
    {
        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
        customPlot->graph(1)->setData(discoveryModified, latencyModified);
        customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
        customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::yellow, 4));
    }

    // give the axes some labels:
    customPlot->xAxis->setLabel("Indice de Descubierta (AP/s)");
    customPlot->yAxis->setLabel("Latencia (ms)");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0.5, 3);
    customPlot->yAxis->setRange(0, 600);

    customPlot->yAxis->grid()->setSubGridVisible(true);

    ui->customPlot->replot();



    // show legend:
    //customPlot->legend->setVisible(true);

/*
    // add two new graphs and set their look:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(250), y0(250), y1(250);
    for (int i=0; i<250; ++i)
    {
      x[i] = i;
      y0[i] = exp(-i/150.0)*cos(i/10.0); // exponentially decaying cosine
      y1[i] = exp(-i/150.0);             // exponential envelope
    }
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    customPlot->graph(0)->setData(x, y0);
    customPlot->graph(1)->setData(x, y1);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    customPlot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    customPlot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

*/

}

void MainWindow::setupCustomPlot2(QCustomPlot *customPlot)
{
/*
    int count = simulation->getGlobalRepository()->getRepositoryList().count();
    QVector<double> discovery(count), latency(count);

    int i = 0;

    Particle * particle;
    //for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    for (int z=simulation->getGlobalRepository()->getRepositoryList().count()-1; z>=0; z-- )
    {
        particle = simulation->getGlobalRepository()->getRepositoryList().at(z);
        discovery[i] = particle->getPerformanceDiscovery();
        latency[i] = particle->getPerformanceLatency();
        i++;
    }
*/

    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    customPlot->clearGraphs();


    Particle * particle;
    int count = genericAlgorithmSolutions.count();
    QVector<double> discovery(count), latency(count);
    for (int i = 0; i < count; i++)
    {
        particle = genericAlgorithmSolutions.at(i);
        discovery[i] = particle->getPerformanceDiscovery();
        latency[i] = particle->getPerformanceLatency();
    }


    int countModified = modificatedAlgorithmSolutions.count();
    QVector<double> discoveryModified(countModified), latencyModified(countModified);
    //if (ui->checkBoxComparation->isChecked())
    //if (comparation)
    //{
        for (int i = 0; i < countModified; i++)
        {
            particle = modificatedAlgorithmSolutions.at(i);
            discoveryModified[i] = particle->getPerformanceDiscovery();
            latencyModified[i] = particle->getPerformanceLatency();
        }
    //}

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setData(discovery, latency);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    customPlot->graph(0)->setName("PSO generico");

    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
    customPlot->graph(1)->setData(discoveryModified, latencyModified);
    customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    customPlot->graph(1)->setName("PSO modificado");

    // give the axes some labels:
    customPlot->xAxis->setLabel("Descubierta");
    customPlot->yAxis->setLabel("Latencia");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, 75);
    customPlot->yAxis->setRange(0, 300);

    customPlot->yAxis->grid()->setSubGridVisible(true);



    ui->customPlot->replot();

    // show legend:
    //customPlot->legend->setVisible(true);
}

void MainWindow::activateGridSelection(int state)
{
    if (state == 0)
    {
        ui->label_9->setEnabled(false);
        ui->lineEditSubintervals->setEnabled(false);
    }
    else if(state == 2)
    {
        ui->label_9->setEnabled(true);
        ui->lineEditSubintervals->setEnabled(true);
    }
}

void MainWindow::activateComparationButton(int state)
{
    if (state == 0)
    {
        ui->pushButtonCompareAlgorithms->setEnabled(false);

        ui->pushButtonExecute->setEnabled(true);

        ui->checkBoxGrid->setEnabled(true);
        ui->checkBoxGrid->setChecked(false);

        //ui->label_9->setEnabled(true);
        //ui->lineEditSubintervals->setEnabled(true);

        ui->labelExternalFile->setVisible(true);
        ui->labelNonDominatedNUmber->setVisible(true);
        ui->listViewBestNonDominatedParticles->setVisible(true);

        ui->label_PSO_generico->setVisible(false);
        ui->psoGenericNumber->setVisible(false);
        ui->label_PSO_modificado->setVisible(false);
        ui->psoModifiedNumber->setVisible(false);

        ui->psoGenericTime->setVisible(false);
        ui->psoModifiedTime->setVisible(false);
        ui->label_tiempo_generico->setVisible(false);
        ui->label_tiempo_modificado->setVisible(false);

    }
    else if(state == 2)
    {
        ui->pushButtonCompareAlgorithms->setEnabled(true);

        ui->pushButtonExecute->setEnabled(false);

        ui->checkBoxGrid->setEnabled(false);
        ui->checkBoxGrid->setChecked(false);
        ui->label_9->setEnabled(false);
        ui->lineEditSubintervals->setEnabled(false);

        ui->labelExternalFile->setVisible(false);
        ui->labelNonDominatedNUmber->setVisible(false);
        ui->listViewBestNonDominatedParticles->setVisible(false);

        ui->label_PSO_generico->setVisible(true);
        ui->psoGenericNumber->setVisible(true);
        ui->label_PSO_modificado->setVisible(true);
        ui->psoModifiedNumber->setVisible(true);

        ui->psoGenericTime->setVisible(true);
        ui->psoModifiedTime->setVisible(true);

        ui->label_tiempo_generico->setVisible(true);
        ui->label_tiempo_modificado->setVisible(true);

    }
}

void MainWindow::compareAlgorithms()
{

    //ui->labelExternalFile->setVisible(false);
    //ui->labelNonDominatedNUmber->setVisible(false);
    //ui->listViewBestNonDominatedParticles->setVisible(false);


    ui->checkBoxGrid->setChecked(false);

    QTime timer;
    timer.start();

    executeAlgorithm();

    int runtimeGeneric = timer.elapsed(); //gets the runtime in ms
    ui->psoGenericTime->setText(QString::number(runtimeGeneric)+" ms");



    QMessageBox msg;
    msg.setText("Termino el algoritmo generico");
    //msg.exec();

    ui->checkBoxGrid->setChecked(true);


    timer.start();
    executeAlgorithm();
    int runtimeModified = timer.elapsed(); //gets the runtime in ms
    ui->psoModifiedTime->setText(QString::number(runtimeModified)+" ms");

    ui->checkBoxGrid->setChecked(false);

    msg.setText("Termino el algoritmo modificado");
    //msg.exec();


    setupCustomPlot2(ui->customPlot);


    ui->psoGenericNumber->setText(QString::number(genericAlgorithmSolutions.count()));
    ui->psoModifiedNumber->setText(QString::number(modificatedAlgorithmSolutions.count()));

}


void MainWindow::plotSolutions()
{
    if (!ui->checkBoxComparation->isChecked())
    {
        setupCustomPlot(ui->customPlot);
    }
}


QString MainWindow::createResultsDirectory()
{

    QString resultsDir = QDir::currentPath() + "/resultados";
    qDebug(qPrintable(resultsDir));

    QString outputDir = resultsDir + "/" + QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss");
    qDebug(qPrintable(outputDir));


    QDir dir(resultsDir);
    if (!dir.exists())
    {
        qDebug("no existe");
        if (dir.mkdir(resultsDir))
            qDebug("se acaba de crear");
        else
            qDebug("no se pudo crear");
    }
    else
    {
        qDebug("existe");
    }

    // crear el directorio con la fecha
    QDir tmpDir(outputDir);
    if (!tmpDir.exists())
    {
        qDebug("no existe el directorio de ejecucion");
        if (tmpDir.mkdir(outputDir))
            qDebug("creado el directorio de la ejecucion");
        else
            qDebug("fallo la creacion del directorio de ejecucion");

    }
    return outputDir;

}


void MainWindow::reportParticleAsFile(QList<Particle*> list, QString resultsSubdirectory, QString fileName)
{
    QFile file(resultsSubdirectory+"/"+fileName+".txt");
    if (file.exists())
    {
        file.remove();
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QString msg = "No se pudo crear el archivo /tmp/"+fileName+".txt";
        qDebug(qPrintable(msg));
        return;
    }
    QTextStream out(&file);
    //out << resultsSubdirectory +"/"+fileName+".txt - Individuos encontrados luego de ejecutar el algoritmo cultural: " << "\n";

    for(int i=0; i<list.count(); i++)
    {
        out << list.at(i)->getParticleAsQString() << "\n";
    }
}
