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
    void setNumMsg(uint32_t rank, uint32_t num, bool isAdd, bool editMode);
	void check();
    void clearGridMsg(uint32_t rank);
	void requestHighlight(int rank);
	void getAnswer();
	void restartMsg();

public slots: // connect with logic
    void showAnswer(const QVector< QVector<int> >& ans); // show answer of the problem

private slots: // connect with GUI
    void __On_Show_Problem();
    void __On_Show_Wrong(const QVector<int>& girds); // show which grids are misfilled
    void __On_SolveBtn_Clicked();
    void __On_ConsumeTimer_Timeout();
    void __On_StartpauseBtn_Clicked();
    void __On_RestartBtn_clicked();
    void __On_EditBtn_Click();
    void __On_EraseBtn_Click();
    void __On_CheckBtn_Click();
    void __On_SudukuBtn_Clicked();
    void __On_InputNumBtn_Clicked();
    void __On_Highlight_Grids(int rank, QVector<int> grids);
    void __On_SetNum(uint32_t rank, uint32_t num, bool isAdd, bool editMode);
    void __On_ClearGrid(uint32_t rank);

private:
    void __Receive_Operation(EN_Operation_Type op, uint32_t num);
    void __Init_Timer();
    void __Send_Operation(uint32_t num = 0);

private:
    Ui::CGameBoard *ui;
    uint32_t m_nChosenSudukuGridBtn; // 数独区被选中的按键
    QTime m_StartTime;
    std::shared_ptr<QTimer> m_iConsumeTimer;
	bool m_isStopped;
	QIcon m_startIcon;
	QIcon m_pauseIcon;
    std::shared_ptr<LogicController> m_iLogic;
    EN_Operation_Type m_enOperationMode; // 0 : input; 1 : edit;
    bool m_Exit;
};

#endif // GAMEBOARD_H
