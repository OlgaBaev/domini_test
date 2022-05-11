#include "mainwindowqgl.h"


MainWindowQGL::MainWindowQGL()
{
    board = new BoardOGL();
    game = new Game();
    game->connectBoard(board->getBoard());

    setFocusPolicy(Qt::StrongFocus);

    layoutMain = new QGridLayout;

    lInfo = new QLabel("TEST");
    lInfo->setStyleSheet("QLabel {color : white; font: 32pt;}");
    //lInfo->setGeometry(0, 0, 200, 50);
    layoutMain->addWidget(lInfo, 0, 0);

    layoutMain->addWidget(board, 1, 0);


    lControlInfo = new QLabel("Press SPACE to reset");
    lControlInfo->setStyleSheet("QLabel {color : white; font: 20pt;}");

    layoutMain->addWidget(lControlInfo, 2, 0);



    painter = new QPainter;

    setLayout(layoutMain);

    connect(board, SIGNAL(signalFieldSelected(Coord)),
            this, SLOT(slotFieldSelected(Coord)));

    connect(game, SIGNAL(signalVictory(Player)), this, SLOT(slotVictory(Player)));
    connect(game, SIGNAL(signalNoMoreAIMoves()), this, SLOT(slotNoMoreAIMoves()));

    update();

}

MainWindowQGL::~MainWindowQGL()
{
    delete board;
    delete game;
}

void MainWindowQGL::paintEvent(QPaintEvent *event)
{
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    board->update();
    painter->end();
}

void MainWindowQGL::keyPressEvent(QKeyEvent *ke)
{
    //lInfo->setText("KEY");
    switch (ke->key())
        {
              case Qt::Key_Space:
                    game->reset();
                    update();
                    break;
    }
}

void MainWindowQGL::mousePressEvent(QMouseEvent *me)
{
    //lInfo->setText("MOUSE");

}


bool MainWindowQGL::pick(Coord pos)
{
    if(game->pick(pos))
    {
        board->getField(pos.x, pos.y)->select();
    }


}

bool MainWindowQGL::unpick()
{
    Coord selected = game->getSelected();
    if(selected.x != -1 && selected.y != -1)
        board->getField(selected.x, selected.y)->unselect();
    game->unpick();

}

void MainWindowQGL::slotFieldSelected(Coord pos)
{
    if(!game->getIsRun())
    {
        game->start();
    }

    if(!game->move(pos))
    {
        unpick();
        pick(pos);
    }
    else
    {
        unpick();
        game->makeAIMove();
    }

    /*QString inf = "\n";
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
            inf.append(QString("%1 ").arg(board->getFieldPtr(i,j)->occupiedStatus()));
        inf.append("\n");
    }*/
    /*lInfo->setText(QString("Pressed: (%1, %2) :: %3 Curr=%4\n(%5 %6) %7")
                   .arg(pos.x).arg(pos.y)
                   .arg(board->getFieldStatus(pos.x, pos.y))
                   .arg(game->getCurrentPlayer())
                   .arg(game->getSelected().x).arg(game->getSelected().y).arg((int)board->getFieldPtr(pos.x,pos.y), 1, 16));
*/

    update();

}

void MainWindowQGL::slotVictory(Player player)
{
    unpick();
    if(player == Player::white)
        lInfo->setText("White won!");
    if(player == Player::black)
        lInfo->setText("Black won!");


}

void MainWindowQGL::slotNoMoreAIMoves()
{
    lInfo->setText("AI failed. Reset the game.");
}
