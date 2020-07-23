#include "logiccontroller.h"
#include <QDebug>
#include <QMessageBox>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "db/puzzle_db.hh"

LogicController::LogicController(QObject *parent) : \
	QObject(parent)
{
}

bool LogicController::startGame(std::shared_ptr<ST_PuzzleInfo_Type> pInfo)
{
    m_iPuzzleInfo = pInfo;
    m_nPuzzleId = pInfo->id;

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(pInfo->content.toLocal8Bit(), &json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!";
        return false;
    }

    QJsonObject rootObj = jsonDoc.object();
    QStringList keys = rootObj.keys();
    for(int iLoop = 1; iLoop <= 9; iLoop++)
    {
        if(rootObj.contains(QString::number(iLoop))){
            QJsonArray subObj = rootObj.value(QString::number(iLoop)).toArray();
            for(int jLoop = 0; jLoop < 9; jLoop++){
                m_mat[iLoop-1][jLoop] = subObj.at(jLoop).toInt();
            }
        }
    }
    qDebug() << __FUNCTION__ << ":" << __LINE__;
    m_size = 9;
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            for(int kLoop = 1; kLoop <= 9; kLoop++) {
                if(m_mat[iLoop][jLoop] == kLoop)  {
                    m_user[iLoop][jLoop][kLoop-1] = true;
                }else{
                    m_user[iLoop][jLoop][kLoop-1] = false;
                }
			}
		}
	}
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            m_preMode[iLoop][jLoop] = false;
		}
	}
    emit showProblem();
//	Solver sol;
//	m_ans = sol(m_mat)[0];
    return true;
}

void LogicController::setNum(int rank, int num, bool isAdd, bool editMode)
{
    int row = (rank-1) / 9;
    int col = (rank-1)%9;

	bool isRemove = !isAdd;
	bool f = false;
	if(m_mat[row][col]) return;
	if(isRemove) { // 删除模式
		if(m_mat[row][col]) { // 题面不能被删除
			return;
		} else {
            m_user[row][col][num-1] = false;
			f = true;
		}
	} else {
        if(editMode) { // 编辑模式
        	if(m_preMode[row][col] && \
                m_user[row][col][num-1]) // 如果先前也是编辑模式而且有这个数，其实是移除
        	{
                m_user[row][col][num-1] = false;
        		f = true;
        	} else { // 先前是输入模式或没有这个数，都是编辑标记
                m_user[row][col][num-1] = true;
        		f = false;
        	}
        	m_preMode[row][col] = true;
        } else { // 输入模式
        	if(m_preMode[row][col])
        	{// 如果先前是编辑模式那么就是直接输入
                for(int i = 1; i <= 9; i++) m_user[row][col][i-1] = false;
                m_user[row][col][num-1] = true;
        		f = false;
        	} else {
                if(!m_user[row][col][num-1]) {// 先前是输入模式且没有这个数
                    for(int i = 1; i <= 9; i++) m_user[row][col][i-1] = false;
                    m_user[row][col][num-1] = true;
            		f = false;
            	} else { // 先前是输入模式且填过这个数了
                    for(int i = 1; i <= 9; i++) m_user[row][col][i-1] = false;
            		f = true;
            	}
        	}
        	m_preMode[row][col] = false;
        }
	}
	emit setNumMsg(rank, num, !f, editMode);
	calHighlights(rank);
	emptyRedoStack();
}

void LogicController::check()
{  
//	QVector<int> wrongGrids;
//	for(int i = 1; i <= m_size; i++) {
//		for(int j = 1; j <= m_size; j++) {
//			if(m_preMode[i][j]) continue;
//			bool f = false;
//			for(int k = 1; k <= m_size; k++) {
//				if(m_user[i][j][k]) f = true;
//			}
//			if(f && !m_user[i][j][m_ans[i][j] - 1]) wrongGrids.push_back(i*m_size + j);
//		}
//	}
//	emit showWrong(wrongGrids);
}

void LogicController::clearGrid(int rank)
{
    int row = (rank-1) / 9;
    int col = (rank-1)%9;
	qDebug() << "LogicController::clearGrid called";
	bool haveElem = false;
    for(int i = 1; i <= 9; i++) {
        if(m_user[row][col][i-1]) {
			haveElem = true; break;
		}
	}
	if(!haveElem) return;
	if(m_mat[row][col]) return;
    for(int i = 1; i <= 9; i++) {
        m_user[row][col][i-1] = false;
	}
	emit clearGridMsg(rank);
	emptyRedoStack();
	qDebug() << "LogicController::clearGrid finished";
}

void LogicController::calHighlights(int rank)
{
    qDebug() << "LogicController::calHighlights called rank=" << rank;
    int row = (rank-1)/9;
    int col = (rank-1)%9;
	int cnt = 0;
    int pos = -1;
	QVector<int> ans;
    for(int iLoop = 1; iLoop <= 9; iLoop++) {
        if(m_user[row][col][iLoop-1]) {
			cnt++;
            pos = iLoop-1;
		}
	}
    qDebug() << "LogicController::calHighlight, cnt=" << cnt << ",pos=" << pos;
	if(cnt != 1 || m_preMode[row][col]) { // 未填或多填或标记模式
		emit highlightGrids(rank, ans);
		return;
	}
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            if(!m_preMode[iLoop][jLoop] && pos >= 0 && m_user[iLoop][jLoop][pos]) {
                ans.push_back(iLoop * 9 + jLoop +1);
			}
		}
	}
	emit highlightGrids(rank, ans);
}

void LogicController::calAnswer()
{
//	emptyUndoStack();
//	emptyRedoStack();
//	for(int i = 1; i <= m_size; i++) {
//		for(int j = 1; j <= m_size; j++) {
//			for(int k = 1; k <= m_size; k++) {
//				m_user[i][j][k] = false;
//			}
//			m_user[i][j][m_ans[i][j]] = true;
//		}
//	}
//	emit showAnswer(m_ans);
}

void LogicController::restartGame()
{
	emptyUndoStack();
	emptyRedoStack();
    startGame(m_iPuzzleInfo);
}

void LogicController::emptyUndoStack()
{
	while(!m_undoStack_chosen.empty()) {
		m_undoStack_chosen.pop();
	}
	while(!m_undoStack_preMode.empty()) {
		m_undoStack_preMode.pop();
	}
	while(!m_undoStack_user.empty()) {
		m_undoStack_user.pop();
	}
}

void LogicController::emptyRedoStack()
{
	while(!m_redoStack_chosen.empty()) {
		m_redoStack_chosen.pop();
	}
	while(!m_redoStack_preMode.empty()) {
		m_redoStack_preMode.pop();
	}
	while(!m_redoStack_user.empty()) {
		m_redoStack_user.pop();
	}
}

