#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QLabel>
#include <QAbstractTableModel>
#include <QTableWidget>
#include <QLabel>

#include "renderarea.h"
#include "custommodel.h"
#include "wiresim.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void SetBrush( WireNodeType wireNodeBrush );
    void updateTable();

protected slots:

    // Timer interaction
    void update();

private slots:

    void on_updateFrequencySlider_sliderMoved(int position);

    void on_pushButton_11_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton6_clicked();

    void on_pushButton1_clicked();

    void on_pushButton0_clicked();

    void on_pushButton2_clicked();

    void on_pushButton3_clicked();

    void on_pushButton4_clicked();

    void on_pushButton5_clicked();

    void on_pushButton7_clicked();

private:
    Ui::MainWindow *ui;

    WireSim* m_wireSim;

    bool m_startSimulation;

    QTimer* m_timer;

    int m_updateMs;

    float m_zoomFactor;

    QLabel* m_labels[ 32 ][ 32 ];

    // Active brush
    WireNodeType m_wireNodeBrush;

    RenderArea* m_renderArea;

};

#endif // MAINWINDOW_H
