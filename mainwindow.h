#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "./qcp/qcustomplot.h"

enum class BC { free, pinned, fixed, sliding };

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updatePlots();

    void updateSystemPlot(void);
    void doAnalysis(void);
    void updateResultPlot(QCustomPlot * plot, QVector<double> &x, QVector<double> &y);
    void computeI(void);

private slots:

    void on_loadWslider_valueChanged(int value);
    void on_loadPslider_valueChanged(int value);
    void on_loadXPslider_valueChanged(int value);
    void on_loadW_valueChanged(double arg1);
    void on_loadP_valueChanged(double arg1);
    void on_loadXP_valueChanged(double arg1);
    void on_materialSelection_currentIndexChanged(const QString &arg1);

    void on_rectangleB_valueChanged(double arg1);
    void on_rectangleH_valueChanged(double arg1);

    void on_IbeamB_valueChanged(double arg1);
    void on_IbeamH_valueChanged(double arg1);
    void on_IbeamTf_valueChanged(double arg1);
    void on_IbeamTw_valueChanged(double arg1);

    void on_channelB_valueChanged(double arg1);
    void on_channelH_valueChanged(double arg1);
    void on_channelTf_valueChanged(double arg1);
    void on_channelTtop_valueChanged(double arg1);
    void on_channelTbottom_valueChanged(double arg1);

    void on_dimensionL_valueChanged(double arg1);

    void on_leftBCfree_clicked();
    void on_leftBCpinned_clicked();
    void on_leftBCfixed_clicked();
    void on_leftBCslide_clicked();

    void on_rightBCfree_clicked();
    void on_rightBCpinned_clicked();
    void on_rightBCfixed_clicked();
    void on_rightBCslide_clicked();

    void on_shapeSelection_currentIndexChanged(int index);
    void on_loadW_editingFinished();
    void on_loadP_editingFinished();
    void on_loadXP_editingFinished();
    void on_action_Load_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_actionE_xport_to_PNG_triggered();
    void on_action_About_triggered();
    void on_actionBackground_triggered();

private:
    Ui::MainWindow *ui;

    double  m_length;
    double  m_E;
    double  m_I;

    double  m_rectB;
    double  m_rectH;

    double  m_IbeamB;
    double  m_IbeamH;
    double  m_IbeamTw;
    double  m_IbeamTf;

    double  m_channelB;
    double  m_channelH;
    double  m_channelTf;
    double  m_channelTtop;
    double  m_channelTbottom;

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

#endif // MAINWINDOW_H
