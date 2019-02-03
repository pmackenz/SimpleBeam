#include "solver.h"
#include "math.h"

Solver::Solver()
{
    setParameters(1.0, 1.0, 10.);
}

void Solver::setParameters(double E, double I, double L)
{
    m_E = E;
    m_I = I;
    m_length = L;
}

void Solver::doAnalysis()
{
    double  kfu, kf0, kmu, km0;
    double  Pe, Me;

    const int N = 6;

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
}
