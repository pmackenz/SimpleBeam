#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVector>
#include "qcp/qcustomplot.h"
#include "helpwindow.h"
#include "dialogabout.h"
#include "solver.h"

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

    m_solver = new Solver();

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
    int height = this->height()<0.65*rec.height()?0.65*rec.height():this->height();
    int width  = this->width()<0.65*rec.width()?0.65*rec.width():this->width();
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
    this->updatePlots();
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
    m_solver->setParameters(m_E, m_I, m_length);

    double  kfu, kf0, kmu, km0;
    double  Pe, Me;

    int N = 6;

    double Pvector[N];
    double Kmatrix[N][N];
    for (int i=0; i<N; i++) {
        Pvector[i] = 0.0;
        for (int j=0; j<N; j++) {
            Kmatrix[i][j] = 0.0;
        }
    }

    // element 1

    double L = m_xP;
    if (L < m_length/100.) L = m_length/100.;

    kfu = 12. * m_E * m_I / pow(L, 3) ;
    kf0 =  6. * m_E * m_I / pow(L, 2) ;
    kmu = kf0 ;
    km0 =  4. * m_E * m_I / L ;

    Pe = m_w * L / 2.;
    Me = -Pe * L / 6.0;

    Kmatrix[0][0] += kfu;
    Kmatrix[1][0] += kmu;
    Kmatrix[1][1] += km0;
    Kmatrix[2][0] += -kfu;
    Kmatrix[2][1] += -kf0;
    Kmatrix[2][2] += kfu;
    Kmatrix[3][0] += kmu;
    Kmatrix[3][1] += km0/2.;
    Kmatrix[3][2] += -kmu;
    Kmatrix[3][3] += km0;

    Pvector[0] -= Pe;
    Pvector[1] += Me;
    Pvector[2] -= Pe;
    Pvector[3] -= Me;

    // element 2
    L = m_length - m_xP;
    if (L < m_length/100.) L = m_length/100.;

    kfu = 12. * m_E * m_I / pow(L, 3) ;
    kf0 =  6. * m_E * m_I / pow(L, 2) ;
    kmu = kf0 ;
    km0 =  4. * m_E * m_I / L ;

    Pe = m_w * L / 2.;
    Me = -Pe * L / 6.0;

    Kmatrix[2][2] += kfu;
    Kmatrix[3][2] += kmu;
    Kmatrix[3][3] += km0;
    Kmatrix[4][2] += -kfu;
    Kmatrix[4][3] += -kf0;
    Kmatrix[4][4] += kfu;
    Kmatrix[5][2] += kmu;
    Kmatrix[5][3] += km0/2.;
    Kmatrix[5][4] += -kmu;
    Kmatrix[5][5] += km0;

    Pvector[2] -= Pe;
    Pvector[3] += Me;
    Pvector[4] -= Pe;
    Pvector[5] -= Me;

    if (m_P != 0.0) Pvector[2] -= m_P;

    for (int i=0; i<N; i++)
        for (int j=i+1; j<N; j++)
            Kmatrix[i][j] = Kmatrix[j][i];

    // apply left BC
    if (leftBC == BC::fixed || leftBC == BC::pinned)
    {
        Kmatrix[0][0] = 1.0;
        Kmatrix[1][0] = 0.0;
        Kmatrix[2][0] = 0.0;
        Kmatrix[3][0] = 0.0;
        Kmatrix[4][0] = 0.0;
        Kmatrix[5][0] = 0.0;
        Kmatrix[0][1] = 0.0;
        Kmatrix[0][2] = 0.0;
        Kmatrix[0][3] = 0.0;
        Kmatrix[0][4] = 0.0;
        Kmatrix[0][5] = 0.0;

        Pvector[0] = 0.0;
    }

    if (leftBC == BC::fixed || leftBC == BC::sliding)
    {
        Kmatrix[0][1] = 0.0;
        Kmatrix[1][1] = 1.0;
        Kmatrix[2][1] = 0.0;
        Kmatrix[3][1] = 0.0;
        Kmatrix[4][1] = 0.0;
        Kmatrix[5][1] = 0.0;
        Kmatrix[1][0] = 0.0;
        Kmatrix[1][2] = 0.0;
        Kmatrix[1][3] = 0.0;
        Kmatrix[1][4] = 0.0;
        Kmatrix[1][5] = 0.0;

        Pvector[1] = 0.0;
    }

    // apply right BC

    if (rightBC == BC::fixed || rightBC == BC::pinned)
    {
        Kmatrix[0][4] = 0.0;
        Kmatrix[1][4] = 0.0;
        Kmatrix[2][4] = 0.0;
        Kmatrix[3][4] = 0.0;
        Kmatrix[4][4] = 1.0;
        Kmatrix[5][4] = 0.0;
        Kmatrix[4][0] = 0.0;
        Kmatrix[4][1] = 0.0;
        Kmatrix[4][2] = 0.0;
        Kmatrix[4][3] = 0.0;
        Kmatrix[4][5] = 0.0;

        Pvector[4] = 0.0;
    }

    if (rightBC == BC::fixed || rightBC == BC::sliding)
    {
        Kmatrix[0][5] = 0.0;
        Kmatrix[1][5] = 0.0;
        Kmatrix[2][5] = 0.0;
        Kmatrix[3][5] = 0.0;
        Kmatrix[4][5] = 0.0;
        Kmatrix[5][5] = 1.0;
        Kmatrix[5][0] = 0.0;
        Kmatrix[5][1] = 0.0;
        Kmatrix[5][2] = 0.0;
        Kmatrix[5][3] = 0.0;
        Kmatrix[5][4] = 0.0;

        Pvector[5] = 0.0;
    }

    // solve system

    // ... LDL^t matrix decomposition
    for (int i=0; i<N; i++)
    {
        for (int k=0; k<i; k++) Kmatrix[i][i] -= Kmatrix[i][k]*Kmatrix[i][k]*Kmatrix[k][k];
        for (int j=i+1; j<N; j++)
        {
            for (int k=0; k<i; k++) Kmatrix[j][i] -= Kmatrix[i][k]*Kmatrix[j][k]*Kmatrix[k][k];
            Kmatrix[j][i] /= Kmatrix[i][i];
        }
    }

    // .. back substitution
    for (int j=0; j<N; j++)
        for (int k=0; k<j; k++)
            Pvector[j] -= Kmatrix[j][k]*Pvector[k];
    for (int m=0; m<N; m++)
        Pvector[m] /= Kmatrix[m][m];
    for (int j=N-1; j>=0; j--)
        for (int m=j+1; m<N; m++)
            Pvector[j] -= Kmatrix[m][j] * Pvector[m];

    double vi, vj, thetai, thetaj;

    // set up results containers
    X.clear();
    M.clear();
    V.clear();
    th.clear();
    d.clear();

    double x0;

    // ... element 1
    x0 = 0.0;

    vi     = Pvector[0];
    thetai = Pvector[1];
    vj     = Pvector[2];
    thetaj = Pvector[3];

    L = m_xP;
    if (L < m_length/100.) L = m_length/100.;

    kfu = 12. * m_E * m_I / pow(L, 3) ;
    kf0 =  6. * m_E * m_I / pow(L, 2) ;
    kmu = kf0 ;
    km0 =  4. * m_E * m_I / L ;

    Pe = m_w * L / 2.;
    Me = -Pe * L / 6.0;

    for (int k=0; k<=20; k++)
    {
        // position
        double x = L*double(k)/20;

        // compute moment
        double moment = ( kmu - kfu * x ) * (vj - vi)
                + (-km0 + kf0 * x ) * thetai  + (-0.5*km0 + kf0 * x) * thetaj
                - m_w / 12. * (L*L - 6.*x*L + 6.*x*x);

        // compute shear
        double shear = kfu * (vi - vj) + kf0 * (thetai + thetaj) - m_w * (x - 0.5*L);

        // compute slope
        double slope = 6.*x/L*(1.0 - x/L) * (vj - vi) / L
                + (1. - 4.*x/L + 3.*x*x/(L*L)) * thetai + (-2.*x/L + 3.*x*x/(L*L)) * thetaj
                - m_w * x* (L*L - 3.*L*x + 2.*x*x)/(12.*m_E*m_I);

        // compute deflection
        double disp =  vj * (3. - 2.*x/L)*x*x/(L*L) + vi * (1. - 3.*x*x/(L*L) + 2.*x*x*x/(L*L*L))
                + x*(1. - 2.*x/L + x*x/(L*L)) * thetai + x*(-x/L + x*x/(L*L)) * thetaj
                - m_w * (L - x)*(L - x)*x*x/(24.*m_E*m_I) ;

        X.append((x0+x)/12.);
        M.append(moment/12.);
        V.append(shear);
        th.append(slope);
        d.append(disp);
    }

    // ... element 2
    x0 = m_xP;
    if (x0 < m_length/100.) x0 = m_length/100.;

    vi     = Pvector[2];
    thetai = Pvector[3];
    vj     = Pvector[4];
    thetaj = Pvector[5];

    L = m_length - m_xP;
    if (L < m_length/100.) L = m_length/100.;

    kfu = 12. * m_E * m_I / pow(L, 3) ;
    kf0 =  6. * m_E * m_I / pow(L, 2) ;
    kmu = kf0 ;
    km0 =  4. * m_E * m_I / L ;

    Pe = m_w * L / 2.;
    Me = -Pe * L / 6.0;

    for (int k=0; k<=20; k++)
    {
        // position
        double x = L*double(k)/20;

        // compute moment
        double moment = ( kmu - kfu * x ) * (vj - vi)
                + (-km0 + kf0 * x ) * thetai  + (-0.5*km0 + kf0 * x) * thetaj
                - m_w / 12. * (L*L - 6.*x*L + 6.*x*x);

        // compute shear
        double shear = kfu * (vi - vj) + kf0 * (thetai + thetaj) - m_w * (x - 0.5*L);

        // compute slope
        double slope = 6.*x/L*(1.0 - x/L) * (vj - vi) / L
                + (1. - 4.*x/L + 3.*x*x/(L*L)) * thetai + (-2.*x/L + 3.*x*x/(L*L)) * thetaj
                - m_w * x* (L*L - 3.*L*x + 2.*x*x)/(12.*m_E*m_I);

        // compute deflection
        double disp =  vj * (3. - 2.*x/L)*x*x/(L*L) + vi * (1. - 3.*x*x/(L*L) + 2.*x*x*x/(L*L*L))
                + x*(1. - 2.*x/L + x*x/(L*L)) * thetai + x*(-x/L + x*x/(L*L)) * thetaj
                - m_w * (L - x)*(L - x)*x*x/(24.*m_E*m_I) ;

        X.append((x0+x)/12.);
        M.append(moment/12.);
        V.append(shear);
        th.append(slope);
        d.append(disp);
    }
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
    this->doAnalysis();

    this->updateSystemPlot();
    this->updateResultPlot(ui->momentPlot, X, M);
    this->updateResultPlot(ui->shearPlot,  X, V);
    this->updateResultPlot(ui->slopePlot,  X, th);
    this->updateResultPlot(ui->dispPlot,   X, d);
}

void MainWindow::on_action_Load_triggered()
{

}

void MainWindow::on_action_Save_triggered()
{

}

void MainWindow::on_actionSave_As_triggered()
{

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
