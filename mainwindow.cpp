#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "wiresim.h"

#include <QVBoxLayout>
#include <QSlider>
#include <QGroupBox>
#include <QPushButton>
#include <QWheelEvent>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_wireSim( NULL ),
    m_timer( NULL ),
    m_updateMs( 1000 ), // Default 100 seconds
    m_startSimulation( false ),
    m_wireNodeBrush( cWireNodeType_None ),
    m_zoomFactor( 1.0f ),
    m_renderArea( NULL )
{
    ui->setupUi(this);

    // Get a wire-sim, scramble it up
    m_wireSim = new WireSim( 32, 32 );
    for(int i = 0; i < 32; i++)
    for(int j = 0; j < 32; j++)
    {
        m_wireSim->GetNode( i, j ).m_type = rand() % WireNode::cTypeCount;
        m_wireSim->GetNode( i, j ).m_power = (rand() + 1) % WireNode::cMaxPower;
        m_wireSim->GetNode( i, j ).m_channels = 1;
    }

    m_renderArea = findChild< RenderArea* >();
    m_renderArea->SetWireSim( m_wireSim );

    // Register our update timer...
    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL(timeout()), this, SLOT(update()) );

    // Start timer with initial update call
    update();
}

MainWindow::~MainWindow()
{
    delete m_timer;
    delete m_wireSim;
    delete ui;
}

void MainWindow::SetBrush( WireNodeType wireNodeBrush )
{
    m_renderArea->SetBrush( wireNodeBrush );

    // For each button we have
    for( int i = 0; i < WireNode::cTypeCount; i++ )
    {
        // Grab this index's button
        QString buttonName;
        buttonName.sprintf( "pushButton%d", i );
        QPushButton* button = this->findChild<QPushButton*>( buttonName );

        if( button != NULL )
        {
            // Match (&& never highline no-wire)
            if( i == int(wireNodeBrush) && i != 0 )
            {
                QFont oldFont = button->font();
                oldFont.setBold( true );
                button->setFont( oldFont );
            }
            // Turn off highlight
            else
            {
                QFont oldFont = button->font();
                oldFont.setBold( false );
                button->setFont( oldFont );
            }
        }
    }

}

void MainWindow::update()
{
    // Grab input values
    QSlider* updateFrequencySlider = this->findChild<QSlider*>( "updateFrequencySlider" );
    m_updateMs = 1000.0f / float(updateFrequencySlider->value());

    // Update UI
    QLabel* updateFrequencyLabel = this->findChild<QLabel*>( "updateFrequencyLabel" );
    QString newText;
    newText.sprintf( "%d Hz%s", int( 1000.0f / float(m_updateMs) ), m_startSimulation ? "" : " (Inactive)" );
    updateFrequencyLabel->setText( newText );

    // Do nothing if we aren't playing
    if( !m_startSimulation )
    {
        return;
    }

    // Simulate a step, then update drawing surface based on color map
    m_wireSim->Simulate();

    // Update our table view
    m_renderArea->update();

    // Reset timer
    m_timer->start( m_updateMs );
}


void MainWindow::on_updateFrequencySlider_sliderMoved(int position)
{
    // Update speed changed, so re-start timer
    m_timer->stop();
    update();
}

void MainWindow::on_pushButton_11_clicked()
{
    // Starting sim, disable brushes
    for( int i = 0; i < WireNode::cTypeCount; i++ )
    {
        // Grab this index's button
        QString buttonName;
        buttonName.sprintf( "pushButton%d", i );
        QPushButton* button = this->findChild<QPushButton*>( buttonName );

        if( button != NULL )
        {
            button->setEnabled( false );
        }
    }

    // Force-start the sim cycle
    m_startSimulation = true;
    update();
}

void MainWindow::on_pushButton_2_clicked()
{
    // Starting sim, disable brushes
    for( int i = 0; i < WireNode::cTypeCount; i++ )
    {
        // Grab this index's button
        QString buttonName;
        buttonName.sprintf( "pushButton%d", i );
        QPushButton* button = this->findChild<QPushButton*>( buttonName );

        if( button != NULL )
        {
            button->setEnabled( true );
        }
    }

    // Force-start the sim cycle
    m_startSimulation = false;
    update();
}

void MainWindow::on_pushButton6_clicked()
{
    SetBrush( cWireNodeType_Not );
}

void MainWindow::on_pushButton1_clicked()
{
    SetBrush( cWireNodeType_Wire );
}

void MainWindow::on_pushButton0_clicked()
{

    SetBrush( cWireNodeType_None );
}

void MainWindow::on_pushButton2_clicked()
{
    SetBrush( cWireNodeType_Joint );
}

void MainWindow::on_pushButton3_clicked()
{
    SetBrush( cWireNodeType_Switch );
}

void MainWindow::on_pushButton4_clicked()
{
    SetBrush( cWireNodeType_And );
}

void MainWindow::on_pushButton5_clicked()
{
    SetBrush( cWireNodeType_Or );
}

void MainWindow::on_pushButton7_clicked()
{
    SetBrush( cWireNodeType_Xor );
}


