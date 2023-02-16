#ifndef NINEHOLES_H
#define NINEHOLES_H

#include <QMainWindow>
#include <Hole.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class NineHoles;
}
QT_END_NAMESPACE

class NineHoles : public QMainWindow {
    Q_OBJECT

public:
    enum player{
        p_blue,
        p_red
    };
    NineHoles(QWidget *parent = nullptr);
    virtual ~NineHoles();

private:
    Ui::NineHoles *ui;
    Hole* m_board[9];
    bool m_moving;
    player m_turn;
    int m_round, m_move;
    void selectables();
    void neighbors(int index);
    void winners();
    void win(Hole::State player);

private slots:
    void play(int index);
    void reset();
    void showAbout();
    void status();

};

#endif // NINEHOLES_H
