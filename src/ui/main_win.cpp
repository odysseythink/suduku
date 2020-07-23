#include "ui/main_win.hh"
#include "ui/game_board_win.hh"
#include "ui/puzzle_add_win.hh"
#include "ui_main_win.h"
#include "db/puzzle_db.hh"
#include <QMessageBox>

CMainWin::CMainWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMainWin)
{
	ui->setupUi(this);
}

CMainWin::~CMainWin()
{
	delete ui;
}

void CMainWin::on_startBtn_clicked()
{
    int gameLevel = 3;
    if(ui->comboBox->currentText() == tr("简单")){
        gameLevel = 1;
    } else if(ui->comboBox->currentText() == tr("中等")){
        gameLevel =2;
    } else if(ui->comboBox->currentText() == tr("困难")){
        gameLevel = 3;
    }
    QString errmsg;
    std::shared_ptr<ST_PuzzleInfo_Type> pPuzzleInfo = Get_OnePuzzle_Unsolved(gameLevel, errmsg);
    if(pPuzzleInfo == nullptr){
        QMessageBox::critical(nullptr, tr("获取谜题失败"), tr("获取谜题失败:") + errmsg);

        return;
    }
    CGameBoard* pWin = new CGameBoard(pPuzzleInfo, this);
    if(pWin->exec() == QDialog::Accepted){

    }
    delete pWin;
}

void CMainWin::On_AddBtn_Clicked()
{
    CPuzzleAddWin* pWin = new CPuzzleAddWin(ui->comboBox->currentText(), this);
    if(pWin->exec() == QDialog::Accepted){

    }
    delete pWin;
}
