#ifndef SOLVER_H
#define SOLVER_H

#include "definitions.h"
#include <QVector>

class Solver
{
public:
    Solver();
    void setParameters(double E, double I, double L);
    void doAnalysis();

private:
    double  m_length;
    double  m_E;
    double  m_I;

    double  m_w;
    double  m_P;
    double  m_xP;

    BC  leftBC;
    BC  rightBC;

    // set up results containers
    QVector<double> X;
    QVector<double> M;
    QVector<double> V;
    QVector<double> th;
    QVector<double> d;

};

#endif // SOLVER_H
