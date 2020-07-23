#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include <QIcon>
#include <QVector>
#include <QSignalMapper>
#include "ui/grid_btn.hh"
#include "logic/logiccontroller.h"
#include "db/puzzle_db.hh"


typedef enum {
    EN_Operation_INPUTADD,
    EN_Operation_EDITADD, // set a number
    EN_Operation_REMOVE, // clear a grid
    EN_Operation_CHECK // check all answer
}EN_Operation_Type;


namespace Ui {
class CGameBoard;
}

class CGameBoard : public QDialog
{
	Q_OBJECT

public:
    explicit CGameBoard(std::shared_ptr<ST_PuzzleInfo_Type> pInfo, QWidget *parent = 0);
    ~CGameBoard();

signals:
	void setNumMsg(int rank, int num, bool isAdd, bool editMode);
	void check();
	void clearGridMsg(int rank);
	void requestHighlight(int rank);
	void getAnswer();
	void restartMsg();

public slots: // connect with logic
    void showProblem();
	void highlightGrids(int rank, QVector<int> grids);
	void showWrong(const QVector<int>& girds); // show which grids are misfilled
	void showAnswer(const QVector< QVector<int> >& ans); // show answer of the problem
	void setNum(int rank, int num, bool isAdd, bool editMode);
	void clearGrid(int rank);

private slots: // connect with GUI
	void setBtnChosen(int rank); // show the btn chosen
	void on_pushButton_clicked();
	void updateTime();
	void on_startpausebtn_clicked();
    void On_RestartBtn_clicked();
    void On_EditBtn_Click();
    void On_EraseBtn_Click();
    void On_CheckBtn_Click();
	void sendOperation(int num = 0);    
    void On_SudukuBtn_Clicked();
    void On_InputNumBtn_Clicked();

private:
    void on_receive_operation(EN_Operation_Type op, int num);
	void initTimer();

private:
    Ui::CGameBoard *ui;
	int m_nChosenSudukuGridBtn; // 数独区被选中的按键
    QTime m_StartTime;
    std::shared_ptr<QTimer> m_iComsumetimer;
	bool m_isStopped;
	QIcon m_startIcon;
	QIcon m_pauseIcon;
    std::shared_ptr<LogicController> m_iLogic;
    EN_Operation_Type m_enOperationMode; // 0 : input; 1 : edit;
    bool m_Exit;
};

#endif // GAMEBOARD_H
