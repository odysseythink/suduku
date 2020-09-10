#include "logiccontroller.h"
#include <QDebug>
#include <QMessageBox>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "db/puzzle_db.hh"

static bool __Find_Empty_Location(QGenericMatrix<9,9,uint8_t>& mat, uint32_t& row, uint32_t& col){
    for(uint32_t iLoop = 0; iLoop < 9; iLoop++) {
        for(uint32_t jLoop = 0; jLoop < 9; jLoop++){
            if(mat(iLoop, jLoop) == 0){
                row = iLoop;
                col = jLoop;
                return true;
            }
        }
    }
    return false;
}

static bool __Used_In_Row(QGenericMatrix<9,9,uint8_t>& mat, uint32_t row, uint32_t num){
    for(uint32_t iLoop = 0; iLoop < 9; iLoop++) {
        if(mat(row, iLoop) == num){
            return true;
        }
    }
    return false;
}

static bool __Used_In_Col(QGenericMatrix<9,9,uint8_t>& mat, uint32_t col, uint32_t num){
    for(uint32_t iLoop = 0; iLoop < 9; iLoop++) {
        if(mat(iLoop, col) == num){
            return true;
        }
    }
    return false;
}

static bool __Used_In_Box(QGenericMatrix<9,9,uint8_t>& mat, uint32_t row, uint32_t col, uint32_t num){
    for(uint32_t iLoop = 0; iLoop < 3; iLoop++) {
        for(uint32_t jLoop = 0; jLoop < 3; jLoop++) {
            if(mat(iLoop+row, jLoop+col) == num){
                return true;
            }
        }
    }
    return false;
}

static bool __Check_Location_Is_Safe(QGenericMatrix<9,9,uint8_t>& mat, uint32_t row, uint32_t col, uint32_t num){
    return !__Used_In_Row(mat, row, num) && !__Used_In_Col(mat, col, num) &&
           !__Used_In_Box(mat, row - row%3, col - col%3, num);
}

static bool __Solve_Sudoku(QGenericMatrix<9,9,uint8_t>& mat){
    uint32_t row = 0, col = 0;
    if(!__Find_Empty_Location(mat, row, col)){
        return true;
    }
    for( uint32_t num = 1; num <= 9; num++){
        if(__Check_Location_Is_Safe(mat, row, col, num)){
            mat(row, col) = num;
            if(__Solve_Sudoku(mat)){
                return true;
            }
            mat(row, col) = 0;
        }
    }
    return false;
}

LogicController::LogicController(QObject *parent)
    : QObject(parent)
{
    for(int iLoop = 0; iLoop < 9; iLoop++)
    {
        m_user[iLoop] = QMap<int, QSet<int>>();
        for(int jLoop = 0; jLoop < 9; jLoop++){
            m_QuestionMat(iLoop, jLoop) = 0;
            m_AnswerMat(iLoop,jLoop) = 0;
            m_user[iLoop][jLoop] = QSet<int>();
        }
    }
}

bool LogicController::Check_AllRight()
{
    return true;
}

bool LogicController::Start_Game(std::shared_ptr<ST_PuzzleInfo_Type> pInfo)
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
    for(int iLoop = 0; iLoop < 9; iLoop++)
    {
        if(rootObj.contains(QString::number(iLoop))){
            QJsonArray subObj = rootObj.value(QString::number(iLoop)).toArray();
            for(int jLoop = 0; jLoop < 9; jLoop++){
                m_QuestionMat(iLoop, jLoop) = subObj.at(jLoop).toInt();
            }
        }
    }
    m_size = 9;
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            if(m_QuestionMat(iLoop, jLoop) != 0)  {
                m_user[iLoop][jLoop] << m_QuestionMat(iLoop, jLoop);
            }
		}
	}
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            m_preMode[iLoop][jLoop] = false;
		}
	}
    emit showProblem();
    return true;
}

void LogicController::Set_Num(uint32_t rank, uint32_t num, bool isAdd, bool editMode)
{
    int row = (rank-1) / 9;
    int col = (rank-1)%9;
    if(row >= 9 || col >= 9){
        return;
    }

	bool isRemove = !isAdd;
	bool f = false;
    if(m_QuestionMat(row, col)) return;
	if(isRemove) { // 删除模式
        if(m_QuestionMat(row, col)) { // 题面不能被删除
			return;
		} else {
            m_user[row][col].remove(num);
			f = true;
		}
	} else {
        if(editMode) { // 编辑模式
            if(m_preMode[row][col] && m_user[row][col].contains(num)) // 如果先前也是编辑模式而且有这个数，其实是移除
        	{
                m_user[row][col].remove(num);
        		f = true;
        	} else { // 先前是输入模式或没有这个数，都是编辑标记
                m_user[row][col].insert(num);
        		f = false;
        	}
        	m_preMode[row][col] = true;
        } else { // 输入模式
        	if(m_preMode[row][col])
        	{// 如果先前是编辑模式那么就是直接输入
                for(int i = 1; i <= 9; i++) m_user[row][col].remove(i);
                m_user[row][col].insert(num);
        		f = false;
        	} else {
                if(!m_user[row][col].contains(num)) {// 先前是输入模式且没有这个数
                    for(int i = 1; i <= 9; i++) m_user[row][col].remove(i);
                    m_user[row][col].insert(num);
            		f = false;
            	} else { // 先前是输入模式且填过这个数了
                    for(int i = 1; i <= 9; i++) m_user[row][col].remove(i);
            		f = true;
            	}
        	}
        	m_preMode[row][col] = false;
        }
	}
	emit setNumMsg(rank, num, !f, editMode);
    Calc_Highlights(rank);
	emptyRedoStack();
}

void LogicController::Check()
{  
//    QVector<int> wrongGrids;
//    for(int i = 0; i < 9; i++) {
//        for(int j = 0; j < 9; j++) {
//            if(m_preMode[i][j]) continue;
//            bool f = false;
//            for(int k = 1; k <= 9; k++) {
//                if(m_user[i][j][k-1]) f = true;
//            }
//            if(f && !m_user[i][j][m_ans[i][j] - 1]) wrongGrids.push_back(i*m_size + j);
//        }
//    }
//    emit showWrong(wrongGrids);
}

void LogicController::Clear_Grid(uint32_t rank)
{
    uint32_t row = (rank-1) / 9;
    uint32_t col = (rank-1)%9;
    if(row >= 9 || col >= 9){
        return;
    }
	qDebug() << "LogicController::clearGrid called";
	bool haveElem = false;
    for(int i = 0; i < 9; i++) {
        if(m_user[row][col].contains(i)) {
			haveElem = true; break;
		}
	}
	if(!haveElem) return;
    if(m_QuestionMat(row,col)) return;
    for(int i = 0; i < 9; i++) {
        m_user[row][col].remove(i);
	}
	emit clearGridMsg(rank);
	emptyRedoStack();
	qDebug() << "LogicController::clearGrid finished";
}

void LogicController::Calc_Highlights(uint32_t rank)
{
    qDebug() << "LogicController::On_Calc_Highlights called rank=" << rank;
    int row = (rank-1) / 9;
    int col = (rank-1)%9;
	int cnt = 0;
    int pos = -1;
	QVector<int> ans;
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        if(m_user[row][col].contains(iLoop)) {
			cnt++;
            pos = iLoop;
		}
	}
    qDebug() << "LogicController::calHighlight, cnt=" << cnt << ",pos=" << pos;
	if(cnt != 1 || m_preMode[row][col]) { // 未填或多填或标记模式
		emit highlightGrids(rank, ans);
		return;
	}
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            if(!m_preMode[iLoop][jLoop] && pos > 0 && m_user[iLoop][jLoop].contains(pos)) {
                ans.push_back(iLoop * 9 + jLoop +1);
			}
		}
	}
	emit highlightGrids(rank, ans);
}

void LogicController::Calc_Answer()
{
    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            if(m_QuestionMat(iLoop, jLoop) != 0)  {
                m_AnswerMat(iLoop, jLoop) = m_QuestionMat(iLoop, jLoop);
            }
        }
    }
    __Solve_Sudoku(m_AnswerMat);

    for(int iLoop = 0; iLoop < 9; iLoop++) {
        for(int jLoop = 0; jLoop < 9; jLoop++) {
            if(m_AnswerMat(iLoop, jLoop) != 0)  {
                Set_Num(iLoop*9+jLoop+1, m_AnswerMat(iLoop, jLoop), true, false);
            }
        }
    }
}

void LogicController::Restart_Game()
{
	emptyUndoStack();
	emptyRedoStack();
    Start_Game(m_iPuzzleInfo);
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

