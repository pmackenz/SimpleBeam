#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVector>
#include "qcp/qcustomplot.h"
#include "helpwindow.h"
#include "dialogabout.h"
#include "solver.h"
#include "definitions.h"

#include <QGuiApplication>

#include <QDebug>


#define MAX_FORCE 100.
#define MAX_W     10.

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Single Span Beam Tool");

    m_solver   = new Solver();
    m_status   = false;
    m_filename = "";

    m_length = ui->dimensionL->value() * 12.;

    m_rectB = ui->rectangleB->value();
    m_rectH = ui->rectangleH->value();

    m_IbeamB  = ui->IbeamB->value();
    m_IbeamH  = ui->IbeamH->value();
    m_IbeamTw = ui->IbeamTw->value();
    m_IbeamTf = ui->IbeamTf->value();

    m_channelB = ui->channelB->value();
    m_channelH = ui->channelH->value();
    m_channelTf = ui->channelTf->value();
    m_channelTtop = ui->channelTtop->value();
    m_channelTbottom = ui->channelTbottom->value();

    ui->loadPslider->setValue(10);
    ui->loadWslider->setValue(0);
    ui->loadXPslider->setValue(50);

    leftBC  = BC::pinned;
    rightBC = BC::pinned;

    this->on_materialSelection_currentIndexChanged(ui->materialSelection->currentText());

    ui->systemPlot->setBackground(QBrush(QColor("#ececec")));

    //
    // adjust size of application window to the available display
    //
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int height = this->height()<0.65*rec.height()?int(0.65*rec.height()):this->height();
    int width  = this->width()<0.65*rec.width()?int(0.65*rec.width()):this->width();
    this->resize(width, height);
}

MainWindow::~MainWindow()
{
    delete m_solver;
    delete ui;
}

void MainWindow::on_loadWslider_valueChanged(int value)
{
    m_w = MAX_W * double(value) / 100. / 12.;
    ui->loadW->setValue(m_w*12.);
    this->updatePlots();
}

void MainWindow::on_loadW_valueChanged(double arg1)
{
    ui->loadWslider->setValue(int(100.*arg1/MAX_W));
}

void MainWindow::on_loadW_editingFinished()
{
    double arg1 = ui->loadW->value();
    ui->loadWslider->setValue(int(100.*arg1/MAX_W));
}

void MainWindow::on_loadP_valueChanged(double arg1)
{
    ui->loadPslider->setValue(int(100.*arg1/MAX_FORCE));
}

void MainWindow::on_loadP_editingFinished()
{
    double arg1 = ui->loadP->value();
    ui->loadPslider->setValue(int(100.*arg1/MAX_FORCE));
}

void MainWindow::on_loadPslider_valueChanged(int value)
{
    m_P = MAX_FORCE * double(value) / 100.;
    ui->loadP->setValue(m_P);
    this->updatePlots();
}

void MainWindow::on_loadXP_valueChanged(double arg1)
{
    ui->loadXPslider->setValue(int(100.*arg1*12./m_length));
    //this->updatePlots();
}

void MainWindow::on_loadXP_editingFinished()
{
    double arg1 = ui->loadXP->value();
    ui->loadXPslider->setValue(int(100.*arg1*12./m_length));
}

void MainWindow::on_loadXPslider_valueChanged(int value)
{
    m_xP = m_length * double(value)/100.;
    ui->loadXP->setValue(m_xP/12.);
    this->updatePlots();
}

void MainWindow::on_leftBCfree_clicked()
{
    leftBC = BC::free;
    this->updatePlots();
}

void MainWindow::on_leftBCpinned_clicked()
{
    leftBC = BC::pinned;
    this->updatePlots();
}

void MainWindow::on_leftBCfixed_clicked()
{
    leftBC = BC::fixed;
    this->updatePlots();
}

void MainWindow::on_leftBCslide_clicked()
{
    leftBC = BC::sliding;
    this->updatePlots();
}

void MainWindow::on_rightBCfree_clicked()
{
    rightBC = BC::free;
    this->updatePlots();
}

void MainWindow::on_rightBCpinned_clicked()
{
    rightBC = BC::pinned;
    this->updatePlots();
}

void MainWindow::on_rightBCfixed_clicked()
{
    rightBC = BC::fixed;
    this->updatePlots();
}

void MainWindow::on_rightBCslide_clicked()
{
    rightBC = BC::sliding;
    this->updatePlots();
}

void MainWindow::on_shapeSelection_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    this->updatePlots();
}

void MainWindow::on_materialSelection_currentIndexChanged(const QString &arg1)
{
    QString name;
    if (arg1 == "Softwood") {
        m_E = 1100.0;
    }
    else if (arg1 == "Hardwood") {
        m_E = 1900.0;
    }
    else if (arg1 == "Steel") {
        m_E = 29000.0;
    }
    else if (arg1 == "Concrete 4 ksi") {
        m_E = 9900.0;
    }
    else if (arg1 == "Concrete 6 ksi") {
        m_E = 12000.0;
    }
    else {
        m_E = 1.0;
    }

    name = "E = %1 ksi";
    ui->materialInfo->setText(name.arg(m_E));

    this->updatePlots();
}

void MainWindow::on_rectangleB_valueChanged(double arg1)
{
    m_rectB = arg1;
    this->updatePlots();
}

void MainWindow::on_rectangleH_valueChanged(double arg1)
{
    m_rectH = arg1;
    this->updatePlots();
}

void MainWindow::on_IbeamB_valueChanged(double arg1)
{
    m_IbeamB = arg1;
    this->updatePlots();
}

void MainWindow::on_IbeamH_valueChanged(double arg1)
{
    m_IbeamH = arg1;
    this->updatePlots();
}

void MainWindow::on_IbeamTf_valueChanged(double arg1)
{
    m_IbeamTf = arg1;
    this->updatePlots();
}

void MainWindow::on_IbeamTw_valueChanged(double arg1)
{
    m_IbeamTw = arg1;
    this->updatePlots();
}

void MainWindow::on_channelB_valueChanged(double arg1)
{
    m_channelB = arg1;
    this->updatePlots();
}

void MainWindow::on_channelH_valueChanged(double arg1)
{
    m_channelH = arg1;
    this->updatePlots();
}

void MainWindow::on_channelTf_valueChanged(double arg1)
{
    m_channelTf = arg1;
    this->updatePlots();
}

void MainWindow::on_channelTtop_valueChanged(double arg1)
{
    m_channelTtop = arg1;
    this->updatePlots();
}

void MainWindow::on_channelTbottom_valueChanged(double arg1)
{
    m_channelTbottom = arg1;
    this->updatePlots();
}

void MainWindow::on_dimensionL_valueChanged(double arg1)
{
    m_length = arg1 * 12.;
    m_xP = m_length * ui->loadXPslider->value() / 100.;
    ui->loadXP->setValue(m_xP/12.);

    this->updateSystemPlot();
}

void MainWindow::updateSystemPlot(void)
{
    QCPCurve *LBC    = nullptr;
    QCPCurve *LBC2   = nullptr;
    QCPCurve *RBC    = nullptr;
    QCPCurve *RBC2   = nullptr;
    QCPCurve *forceP = nullptr;
    QCPCurve *forceW = nullptr;

    QVector<double> x;
    QVector<double> y;
    double  s=0.0;

    QCustomPlot *plot = ui->systemPlot;

    plot->clearPlottables();

    plot->addGraph(); plot->graph(0)->setPen(QPen(Qt::black,3));      // line color black for first graph

    // plot the beam

    double L = m_length/12.; // length in ft

    if (L > 0.0)
    {
        s = L/20.;
        x.append(0.0);
        y.append(0.0);
        x.append(L);
        y.append(0.0);
    }
    plot->graph(0)->addData(x,y);

    // plot left boundary condition

    switch (leftBC)
    {
    case BC::free:
        break;
    case BC::pinned:

        LBC = new QCPCurve(plot->xAxis, plot->yAxis);
        LBC->setPen(QPen(Qt::darkGreen, 2)); // line color green

        x.clear(); y.clear();
        x.append(0.0); y.append(0.0);
        x.append(s); y.append(-s);
        x.append(-s); y.append(-s);
        x.append(0.0); y.append(0.0);

        LBC->setData(x,y);
        LBC->rescaleAxes(true);
        break;

    case BC::fixed:

        LBC = new QCPCurve(plot->xAxis, plot->yAxis);
        LBC->setPen(QPen(Qt::darkGreen, 2)); // line color green

        x.clear(); y.clear();
        x.append(0.0); y.append(s);
        x.append(0.0); y.append(-s);

        LBC->setData(x,y);
        LBC->rescaleAxes(true);
        break;

    case BC::sliding:

        LBC = new QCPCurve(plot->xAxis, plot->yAxis);
        LBC->setPen(QPen(Qt::darkGreen, 2)); // line color green
        LBC2 = new QCPCurve(plot->xAxis, plot->yAxis);
        LBC2->setPen(QPen(Qt::darkGreen, 2)); // line color green

        x.clear(); y.clear();
        x.append(0.0); y.append(s);
        x.append(0.0); y.append(-s);

        LBC->setData(x,y);
        LBC->rescaleAxes(true);

        x.append(-s/2.); y.append(s);
        x.append(-s/2.); y.append(-s);

        LBC2->setData(x,y);
        LBC2->rescaleAxes(true);
        break;
        }

    // plot the right boundary condition

    switch (rightBC)
    {
    case BC::free:
        break;
    case BC::pinned:

        RBC = new QCPCurve(plot->xAxis, plot->yAxis);
        RBC->setPen(QPen(Qt::darkGreen, 2)); // line color green

        x.clear(); y.clear();
        x.append(L); y.append(0.0);
        x.append(L+s); y.append(-s);
        x.append(L-s); y.append(-s);
        x.append(L); y.append(0.0);

        RBC->setData(x,y);
        RBC->rescaleAxes(true);
        break;

    case BC::fixed:

        RBC = new QCPCurve(plot->xAxis, plot->yAxis);
        RBC->setPen(QPen(Qt::darkGreen, 2)); // line color green

        x.clear(); y.clear();
        x.append(L); y.append(s);
        x.append(L); y.append(-s);

        RBC->setData(x,y);
        RBC->rescaleAxes(true);
        break;

    case BC::sliding:

        RBC = new QCPCurve(plot->xAxis, plot->yAxis);
        RBC->setPen(QPen(Qt::darkGreen, 2)); // line color green
        RBC2 = new QCPCurve(plot->xAxis, plot->yAxis);
        RBC2->setPen(QPen(Qt::darkGreen, 2)); // line color green

        x.clear(); y.clear();
        x.append(L); y.append(s);
        x.append(L); y.append(-s);

        RBC->setData(x,y);
        RBC->rescaleAxes(true);

        x.clear(); y.clear();
        x.append(L+s/2.); y.append(s);
        x.append(L+s/2.); y.append(-s);

        RBC2->setData(x,y);
        RBC2->rescaleAxes(true);
        break;
        }

    // plot the loads

    if (m_P != 0.0)
    {
        forceP = new QCPCurve(plot->xAxis, plot->yAxis);
        forceP->setPen(QPen(Qt::darkRed, 2)); // line color red for second graph
        forceP->rescaleAxes(true);

        double xp = m_xP/12.; // x_P in ft

        x.clear(); y.clear();
        x.append(xp);   y.append(6.*s*m_P/MAX_FORCE);
        x.append(xp);   y.append(0.0);
        x.append(xp+0.5*s*m_P/MAX_FORCE); y.append(2.*s*m_P/MAX_FORCE);
        x.append(xp-0.5*s*m_P/MAX_FORCE); y.append(2.*s*m_P/MAX_FORCE);
        x.append(xp);   y.append(0.0);

        forceP->setData(x,y);
    }

    if (m_w != 0.0)
    {
        forceW = new QCPCurve(plot->xAxis, plot->yAxis);
        forceW->setPen(QPen(Qt::red, 1)); // line color red for second graph
        forceW->rescaleAxes(true);

        double w = 3.*s*12.*m_w/MAX_W;
        double dL = L/5.;

        x.clear(); y.clear();

        for (int k=0; k<5; k++)
        {
            double x0 = k*dL;
            x.append(x0);         y.append(w/2.);
            x.append(x0+w/6.);    y.append(w/2.);
            x.append(x0);         y.append(0.0);
            x.append(x0);         y.append(w);
            x.append(x0+dL);      y.append(w);
            x.append(x0+dL);      y.append(0.0);
            x.append(x0+dL-w/6.); y.append(w/2.);
            x.append(x0+dL);      y.append(w/2.);
        }

        forceW->setData(x,y);
    }


    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    plot->yAxis->setScaleRatio(plot->xAxis, 1.0);
    plot->graph(0)->rescaleAxes();

    if (forceP) forceP->rescaleAxes(true);
    if (forceW) forceW->rescaleAxes(true);

    if (LBC)  LBC->rescaleAxes(true);
    if (LBC2) LBC2->rescaleAxes(true);
    if (RBC)  RBC->rescaleAxes(true);
    if (RBC2) RBC2->rescaleAxes(true);

    //plot->xAxis->setVisible(false);
    plot->yAxis->setVisible(false);
    plot->replot();
};

void MainWindow::computeI(void)
{
    int type = ui->shapeSelection->currentIndex();

    m_I = 1.0;

    switch (type)
    {
    case 0:
        m_I = m_rectB*pow(m_rectH, 3.);
        m_I /= 12.;
        break;
    case 1:
        m_I = m_IbeamB*pow(m_IbeamH, 3.);
        m_I -= (m_IbeamB-m_IbeamTw)*pow((m_IbeamH-2.*m_IbeamTf), 3.);
        m_I /= 12.;
        break;
    case 2:
        double A = m_channelB*m_channelH;
        A -= (m_channelB-2.*m_channelTf)*(m_channelH-m_channelTtop-m_channelTbottom);
        double S = 2.*m_channelTf*pow(m_channelH, 2.)/2.;
        S += (m_channelB-2.*m_channelTf)*pow(m_channelTbottom, 2.)/2.;
        S += (m_channelB-2.*m_channelTf)*m_channelTtop*(m_channelH-0.5*m_channelTtop);
        double yc = S/A;
        m_I = 2.*m_channelTf*pow(m_channelH, 3.)/3.;
        m_I += (m_channelB-2.*m_channelTf)*pow(m_channelTbottom, 3.)/3.;
        m_I += (m_channelB-2.*m_channelTf)*pow(m_channelTtop, 3.)/12.;
        m_I += (m_channelB-2.*m_channelTf)*m_channelTtop*pow((m_channelH-0.5*m_channelTtop), 2.);
        m_I -= S*yc;
        break;
    }

    ui->crossSectionInfo->setText(QString("I = %1 in^4").arg(m_I, 0, 'g', 3));
}

void MainWindow::doAnalysis(void)
{
    this->computeI();
    m_solver->setParameters(m_E, m_I, m_length, leftBC, rightBC);
    m_status = m_solver->doAnalysis(m_w, m_P, m_xP);
};

void MainWindow::updateResultPlot(QCustomPlot * plot, QVector<double> &x, QVector<double> &y)
{
    plot->clearPlottables();

    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::red,3));
    plot->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 20)));
    plot->graph(0)->setData(x, y);

    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::black,3));
    plot->graph(1)->setData(x, QVector<double>(y.length(), 0.0));

    plot->graph(0)->rescaleAxes();
    plot->graph(1)->rescaleAxes(true);
    plot->replot();
};

void MainWindow::updatePlots()
{
    QVector<double> AXIS({0., m_length/12.});
    QVector<double> ZEROS({0.0, 0.0});

    QVector<double> *X;
    QVector<double> *M;
    QVector<double> *V;
    QVector<double> *th;
    QVector<double> *d;

    this->doAnalysis();

    if (m_status) {
        // set up results containers
        X  = m_solver->getX();
        M  = m_solver->getMoment();
        V  = m_solver->getShear();
        th = m_solver->getRotation();
        d  = m_solver->getDeflection();
    }
    else {
        X  = &AXIS;
        M  = &ZEROS;
        V  = &ZEROS;
        th = &ZEROS;
        d  = &ZEROS;
    }

    this->updateSystemPlot();
    this->updateResultPlot(ui->momentPlot, *X, *M);
    this->updateResultPlot(ui->shearPlot,  *X, *V);
    this->updateResultPlot(ui->slopePlot,  *X, *th);
    this->updateResultPlot(ui->dispPlot,   *X, *d);
}

void MainWindow::on_action_Load_triggered()
{
    // get a filename and location from the user
    QFileDialog *dlg = new QFileDialog(this,
                                       "open file",
                                       ".",
                                       "*.json");
    m_filename = dlg->getOpenFileName();
    delete dlg;

    if (m_filename != "") {
        // open the file
        QFile loadfile(m_filename);
        loadfile.open(QIODevice::ReadOnly);
        QJsonDocument doc;
        QJsonObject obj = doc.fromJson(loadfile.readAll()).object();
        loadfile.close();

        // parse the information
        if (obj.contains("product"))
        {
            if (obj.value("product") == "SimpleBeam")
            {
                // version check in future releases goes here ...

                // parse system information
                QJsonObject system = obj.value("system").toObject();

                m_length = system.value("span").toDouble() * 12.0;  // in

                QString left  = system.value("leftBC").toString();
                if (left == "pinned") { leftBC = BC::pinned; }
                else if (left == "fixed") { leftBC = BC::fixed; }
                else if (left == "sliding") { leftBC = BC::sliding; }
                else  { leftBC = BC::free; }

                QString right = system.value("rightBC").toString();
                if (right == "pinned") { rightBC = BC::pinned; }
                else if (right == "fixed") { rightBC = BC::fixed; }
                else if (right == "sliding") { rightBC = BC::sliding; }
                else  { rightBC = BC::free; }

                // *** update the UI ***

                // system properties
                ui->dimensionL->setValue(m_length/12.);
                switch (leftBC) {
                case BC::free :
                    ui->leftBCfree->setChecked(true);
                    break;
                case BC::fixed :
                    ui->leftBCfixed->setChecked(true);
                    break;
                case BC::pinned :
                    ui->leftBCpinned->setChecked(true);
                    break;
                case BC::sliding :
                    ui->leftBCslide->setChecked(true);
                    break;
                }
                switch (rightBC) {
                case BC::free :
                    ui->rightBCfree->setChecked(true);
                    break;
                case BC::fixed :
                    ui->rightBCfixed->setChecked(true);
                    break;
                case BC::pinned :
                    ui->rightBCpinned->setChecked(true);
                    break;
                case BC::sliding :
                    ui->rightBCslide->setChecked(true);
                    break;
                }

                // parse loads
                QJsonObject loads = obj.value("loads").toObject();

                m_w  = loads.value("w").toDouble() / 12.0;    // k/in
                m_P  = loads.value("P").toDouble();           // kips
                m_xP = loads.value("xP").toDouble() * 12.0;   // in

                // load properties
                ui->loadP->setValue(m_P);
                ui->loadW->setValue(m_w*12.);
                ui->loadXP->setValue(m_xP/12.);

                // parse cross section data
                QJsonObject sectionproperties = obj.value("sectionproperties").toObject();

                m_rectB = sectionproperties.value("rectB").toDouble();
                m_rectH = sectionproperties.value("rectH").toDouble();
                m_IbeamB = sectionproperties.value("IbeamB").toDouble();
                m_IbeamH = sectionproperties.value("IbeamH").toDouble();
                m_IbeamTw = sectionproperties.value("IbeamTw").toDouble();
                m_IbeamTf = sectionproperties.value("IbeamTf").toDouble();
                m_channelB = sectionproperties.value("channelB").toDouble();
                m_channelH = sectionproperties.value("channelH").toDouble();
                m_channelTf = sectionproperties.value("channelTf").toDouble();
                m_channelTtop = sectionproperties.value("channelTtop").toDouble();
                m_channelTbottom = sectionproperties.value("channelTbottom").toDouble();

                // cross section type and properties
                ui->rectangleB->setValue(m_rectB);
                ui->rectangleH->setValue(m_rectH);

                ui->IbeamB->setValue(m_IbeamB);
                ui->IbeamH->setValue(m_IbeamH);
                ui->IbeamTf->setValue(m_IbeamTf);
                ui->IbeamTw->setValue(m_IbeamTw);

                ui->channelB->setValue(m_channelB);
                ui->channelH->setValue(m_channelH);
                ui->channelTf->setValue(m_channelTf);
                ui->channelTtop->setValue(m_channelTtop);
                ui->channelTbottom->setValue(m_channelTbottom);

                int shapeIdx = obj.value("sectiontype").toInt();
                ui->shapeSelection->setCurrentIndex(shapeIdx);
                this->on_shapeSelection_currentIndexChanged(shapeIdx);

                // material type
                ui->materialSelection->setCurrentIndex(obj.value("materialtype").toInt());
            }
            else
            {
                qDebug() << "file is bad";
            }
        }
    }
}

void MainWindow::on_action_Save_triggered()
{
    if (m_filename != "") {

        // create the JSON representation
        QJsonObject data;
        data.insert("product", QJsonValue("SimpleBeam"));
        data.insert("author", QJsonValue("Peter Mackenzie-Helnwein"));
        data.insert("contact", QJsonValue("pmackenz@uw.edu"));
        data.insert("version", QJsonValue("1.0"));

        data.insert("sectiontype",  QJsonValue(ui->shapeSelection->currentIndex()));
        data.insert("materialtype", QJsonValue(ui->materialSelection->currentIndex()));

        QJsonObject loads;
        loads.insert("w", QJsonValue(m_w*12.));     // k/ft
        loads.insert("P", QJsonValue(m_P));         // kips
        loads.insert("xP", QJsonValue(m_xP/12.));   // ft

        QJsonObject sectionproperties;
        sectionproperties.insert("rectB", QJsonValue(m_rectB));
        sectionproperties.insert("rectH", QJsonValue(m_rectH));
        sectionproperties.insert("IbeamB", QJsonValue(m_IbeamB));
        sectionproperties.insert("IbeamH", QJsonValue(m_IbeamH));
        sectionproperties.insert("IbeamTw", QJsonValue(m_IbeamTw));
        sectionproperties.insert("IbeamTf", QJsonValue(m_IbeamTf));
        sectionproperties.insert("channelB", QJsonValue(m_channelB));
        sectionproperties.insert("channelH", QJsonValue(m_channelH));
        sectionproperties.insert("channelTf", QJsonValue(m_channelTf));
        sectionproperties.insert("channelTtop", QJsonValue(m_channelTtop));
        sectionproperties.insert("channelTbottom", QJsonValue(m_channelTbottom));

        QJsonObject system;
        system.insert("span", QJsonValue(m_length/12.));    // ft
        switch (leftBC) {
        case BC::free :
            system.insert("leftBC", QJsonValue("free"));
            break;
        case BC::fixed :
            system.insert("leftBC", QJsonValue("fixed"));
            break;
        case BC::pinned :
            system.insert("leftBC", QJsonValue("pinned"));
            break;
        case BC::sliding :
            system.insert("leftBC", QJsonValue("sliding"));
            break;
        }
        switch (rightBC) {
        case BC::free :
            system.insert("rightBC", QJsonValue("free"));
            break;
        case BC::fixed :
            system.insert("rightBC", QJsonValue("fixed"));
            break;
        case BC::pinned :
            system.insert("rightBC", QJsonValue("pinned"));
            break;
        case BC::sliding :
            system.insert("rightBC", QJsonValue("sliding"));
            break;
        }

        data.insert("loads", loads);
        data.insert("sectionproperties", sectionproperties);
        data.insert("system", system);

        // save to file
        QFile savefile(m_filename);
        savefile.open(QIODevice::WriteOnly);
        savefile.write(QJsonDocument(data).toJson());
        savefile.close();
    }
    else {
        this->on_actionSave_As_triggered();
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    // get a filename and location from the user
    QFileDialog *dlg = new QFileDialog(this,
                                       "save file as",
                                       ".",
                                       "*.json");
    m_filename = dlg->getSaveFileName();
    delete dlg;

    if (m_filename != "") {
        this->on_action_Save_triggered();
    }
}

void MainWindow::on_actionE_xport_to_PNG_triggered()
{

}

void MainWindow::on_action_About_triggered()
{
    DialogAbout *dlg = new DialogAbout();
    dlg->exec();
}

void MainWindow::on_actionBackground_triggered()
{
    HelpWindow *help = new HelpWindow();
    help->show();
}
