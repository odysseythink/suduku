#ifndef LOGICCONTROLLER_H
#define LOGICCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QStack>
#include "db/puzzle_db.hh"

const int GAMESIZE = 9;


class LogicController : public QObject
{
	Q_OBJECT
public:
	explicit LogicController(QObject *parent = 0);
    int Get_MatData(uint32_t row, uint32_t col){
        if(row >= 9 && col >= 9){
            return -1;
        }
        return m_mat[row][col];
    }

signals:
    void showProblem();
	void setNumMsg(int rank, int num, bool isAdd, bool editMode);
	void showWrong(const QVector<int>& grids);
	void clearGridMsg(int rank);
	void highlightGrids(int rank, const QVector<int>& grids);
	void showAnswer(const QVector<QVector<int> >& ans);
//	void congratulation();

public slots:
    bool startGame(std::shared_ptr<ST_PuzzleInfo_Type> pInfo); // init the gameboard in logic controller
	void setNum(int rank, int num, bool isAdd, bool editMode); // support undo and redo
	void check(); // check if is correct
	void clearGrid(int rank); // clear all number in the posi; support undo and redo
	void calHighlights(int rank); // calculate the grids that should be highlighted
	void calAnswer(); // will empty undo redo stack
	void restartGame(); // will empty undo redo stack

private:
	void emptyUndoStack();
	void emptyRedoStack();

private:
	QString m_game;
    int m_mat[9][9];
//    QMap<int, QMap<int,int> > m_ans;
    /*
        m_user是一个三维的vector。每个格子对应一个QVector<bool>,且大小为m_size。
        startGame函数中：
            1.如果当前位置有值，就把对应位置的值置为true，其它地方置为false;
            2.如果当前位置没有值，就把整个QVector<bool>置为false.
    */
    bool m_user[9][9][9];
    bool m_preMode[9][9]; // true is edit mode; false is input mode

    QStack< QMap<int, bool> > m_undoStack_user; // rank r grid status
    QStack< QMap<int, QMap<int,bool> > > m_undoStack_preMode;
	QStack<int> m_undoStack_chosen;
    QStack< QMap<int, bool> > m_redoStack_user; // rank r grid status
    QStack< QMap<int, QMap<int,bool> > > m_redoStack_preMode;
	QStack<int> m_redoStack_chosen;
	int m_emptyPlace;
	int m_size;
    int m_nPuzzleId;
    std::shared_ptr<ST_PuzzleInfo_Type> m_iPuzzleInfo;
};

#endif // LOGICCONTROLLER_H

