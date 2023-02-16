#include "NineHoles.h"
#include "ui_NineHoles.h"

#include <QMessageBox>
#include <QSignalMapper>

#include <QDebug>

NineHoles::NineHoles(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::NineHoles) {

    m_moving=false;
    m_turn = p_red;
    m_move = 0;
    m_round = 0;

    ui->setupUi(this);
    ui->statusbar->setFont(QFont("Times", 12));
    status();
    QObject::connect(ui->statusbar, SIGNAL(messageChanged(const QString)), this, SLOT(status()));

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    // Load the pieces.
    QSignalMapper* map = new QSignalMapper(this);
    for (int idx = 0; idx < 9; ++idx) {
        int r = idx / 3;
        int c = idx % 3;

        Hole* hole = this->findChild<Hole*>(QString("hole%1%2").arg(r).arg(c));
        Q_ASSERT(hole != nullptr);
        hole->setRow(r);
        hole->setCol(c);

        m_board[idx] = hole;
        map->setMapping(hole, idx);
        QObject::connect(hole, SIGNAL(clicked()), map, SLOT(map()));
    }
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));

    // Compact the layout of the widgets.
    this->adjustSize();

    // Set a fixed window size.
    this->setFixedSize(this->size());
}

NineHoles::~NineHoles() {
    delete ui;
}

void NineHoles::play(int index) {
    Hole* hole = m_board[index];
    Q_ASSERT(hole != 0);
//    qDebug() << QString("hole: (%1, %2)").arg(hole->row()).arg(hole->col());

    if(m_moving){
        if(m_turn == p_red){
            if(hole->state() == Hole::RedState){
                selectables();
                neighbors(index);
                m_move = index;
            }
            if(hole->state() == Hole::SelectableState){
                hole->setState(Hole::RedState);
                m_board[m_move]->setState(Hole::EmptyState);
                selectables();
                m_turn = p_blue;
            }
        }
        else{
            if(hole->state() == Hole::BlueState){
                selectables();
                neighbors(index);
                m_move = index;
            }
            if(hole->state() == Hole::SelectableState){
                hole->setState(Hole::BlueState);
                m_board[m_move]->setState(Hole::EmptyState);
                selectables();
                m_turn = p_red;
            }
        }
    }
    else{
        if(hole->state() == Hole::EmptyState){
            if(m_turn == p_red){
                hole->setState(Hole::RedState);
                m_turn = p_blue;
            }
            else{
                hole->setState(Hole::BlueState);
                m_turn = p_red;
                if(++m_round == 3)
                    m_moving = true;
            }
        }
    }
    winners();
    status();
}

void NineHoles::selectables(){
    for (int idx = 0; idx < 9; ++idx)
        if(m_board[idx]->state() == Hole::SelectableState)
            m_board[idx]->reset();
}

void NineHoles::neighbors(int index){
    int holes[4]={-1,-1,-1,-1};
    switch(index){
        case 0:
            holes[0] = 1;
            holes[1] = 3;
            break;
        case 1:
            holes[0] = 0;
            holes[1] = 2;
            holes[2] = 4;
            break;
        case 2:
            holes[0] = 1;
            holes[1] = 5;
            break;
        case 3:
            holes[0] = 0;
            holes[1] = 4;
            holes[2] = 6;
            break;
        case 4:
            holes[0] = 1;
            holes[1] = 3;
            holes[2] = 5;
            holes[3] = 7;
            break;
        case 5:
            holes[0] = 2;
            holes[1] = 4;
            holes[2] = 8;
            break;
        case 6:
            holes[0] = 3;
            holes[1] = 7;
            break;
        case 7:
            holes[0] = 6;
            holes[1] = 4;
            holes[2] = 8;
            break;
        case 8:
            holes[0] = 7;
            holes[1] = 5;
            break;
        default:
            return;
    }
    for(int i=0;i<4;i++){
        if(holes[i]>=0 && holes[i]<=8)
            if(m_board[holes[i]]->state() == Hole::EmptyState)
                m_board[holes[i]]->setState(Hole::SelectableState);
    }
}

void NineHoles::winners(){
    if(m_board[0]->state() != Hole::EmptyState && m_board[0]->state() == m_board[1]->state() && m_board[0]->state() == m_board[2]->state())
        win(m_board[0]->state());
    if(m_board[3]->state() != Hole::EmptyState && m_board[3]->state() == m_board[4]->state() && m_board[3]->state() == m_board[5]->state())
        win(m_board[3]->state());
    if(m_board[6]->state() != Hole::EmptyState && m_board[6]->state() == m_board[7]->state() && m_board[6]->state() == m_board[8]->state())
        win(m_board[6]->state());
    if(m_board[0]->state() != Hole::EmptyState && m_board[0]->state() == m_board[3]->state() && m_board[0]->state() == m_board[6]->state())
        win(m_board[0]->state());
    if(m_board[1]->state() != Hole::EmptyState && m_board[1]->state() == m_board[4]->state() && m_board[1]->state() == m_board[7]->state())
        win(m_board[1]->state());
    if(m_board[2]->state() != Hole::EmptyState && m_board[2]->state() == m_board[5]->state() && m_board[2]->state() == m_board[8]->state())
        win(m_board[2]->state());
}

void NineHoles::win(Hole::State player){
    QString winner;
    if(player == Hole::RedState)
        winner = "Parabéns, o jogador vermelho venceu.";
    else
        winner = "Parabéns, o jogador azul venceu.";
    QMessageBox::information(this, tr("Vencedor"), tr(winner.toStdString().c_str()));
    reset();
}

void NineHoles::reset() {
    m_moving=false;
    m_turn = p_red;
    m_round = 0;
    for (int idx = 0; idx < 9; ++idx)
        m_board[idx]->reset();
}

void NineHoles::showAbout() {
    const char* devs = "Camilo Santana Melgaço: camilocsm@gmail.com\nThiago Rodrigues da Silva: thiagorcefetmg@gmail.com";
    QMessageBox::information(this, tr("Sobre"), tr(devs), tr("Fechar"));
}

void NineHoles::status(){
    QString  msg;
    if(m_moving)
        if(m_turn == p_red)
            msg = "Fase de mover: vez do jogador vermelho";
        else
            msg = "Fase de mover: vez do jogador azul";
    else
        if(m_turn == p_red)
            msg = "Fase de colocar: vez do jogador vermelho";
        else
            msg = "Fase de colocar: vez do jogador azul";
    ui->statusbar->showMessage(msg);
}
