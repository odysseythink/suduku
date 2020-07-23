#include "game_board_win.hh"
#include "ui_game_board_win.h"
#include <QDebug>
#include <QDateTime>

CGameBoard::CGameBoard(std::shared_ptr<ST_PuzzleInfo_Type> pInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGameBoard),
	m_nChosenSudukuGridBtn(-1),
	m_isStopped(false),
    m_enOperationMode(EN_Operation_INPUTADD),m_Exit(false)
{
	ui->setupUi(this);

    ui->m_iStartpauseBtn->setIconSize(QSize(40, 40));

	m_startIcon = QIcon(":/icon/resources/icons/start.png");
	m_pauseIcon = QIcon(":/icon/resources/icons/pause.png");
    ui->m_iStartpauseBtn->setIcon(m_pauseIcon);
    m_iComsumetimer = std::make_shared<QTimer>(this);
	initTimer();
    connect(m_iComsumetimer.get(), SIGNAL(timeout()), this, SLOT(updateTime()));
	//connect(ui->sudokugrid, SIGNAL(btnChosen(int)), this, SLOT(setBtnChosen(int)));

    for(int i = 1; i <= 9; i++) { // 大九宫格
        for(int j = 1; j <= 9; j++) { // 子九宫格
            CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number((i-1)*9 + j));
            if(pBtn != nullptr){
                connect(pBtn, SIGNAL(click()), this, SLOT(On_SudukuBtn_Clicked()));
            }
        }
    }

	ui->m_iEditBtn->setSelfIcon(QPixmap(":/icon/resources/icons/edit.png"));
	ui->m_iEraseBtn->setSelfIcon(QPixmap(":/icon/resources/icons/eraser.png"));
	ui->m_iCheckBtn->setSelfIcon(QPixmap(":/icon/resources/icons/check.png"));

    m_iLogic = std::make_shared<LogicController>();
    QObject::connect(m_iLogic.get(), SIGNAL(showProblem()), this, SLOT(showProblem()));
    QObject::connect(this, SIGNAL(setNumMsg(int,int,bool,bool)), m_iLogic.get(), SLOT(setNum(int,int,bool,bool)));
    QObject::connect(this, SIGNAL(check()), m_iLogic.get(), SLOT(check()));
    QObject::connect(this, SIGNAL(clearGridMsg(int)), m_iLogic.get(), SLOT(clearGrid(int)));
    QObject::connect(this, SIGNAL(requestHighlight(int)), m_iLogic.get(), SLOT(calHighlights(int)));
    QObject::connect(this, SIGNAL(getAnswer()), m_iLogic.get(), SLOT(calAnswer()));
    QObject::connect(this, SIGNAL(restartMsg()), m_iLogic.get(), SLOT(restartGame()));

    QObject::connect(m_iLogic.get(), SIGNAL(setNumMsg(int,int,bool,bool)), this, SLOT(setNum(int,int,bool,bool)));
    QObject::connect(m_iLogic.get(), SIGNAL(showWrong(QVector<int>)), this, SLOT(showWrong(QVector<int>)));
    QObject::connect(m_iLogic.get(), SIGNAL(clearGridMsg(int)), this, SLOT(clearGrid(int)));
    QObject::connect(m_iLogic.get(), SIGNAL(highlightGrids(int,QVector<int>)), this, SLOT(highlightGrids(int,QVector<int>)));
    QObject::connect(m_iLogic.get(), SIGNAL(showAnswer(QVector<QVector<int> >)), this, SLOT(showAnswer(QVector<QVector<int> >)));

    if(!m_iLogic->startGame(pInfo)){
        qDebug() << "start game failed";
        m_Exit = true;
    }
}

CGameBoard::~CGameBoard()
{
	delete ui;
}

void CGameBoard::showProblem()
{
    for(int iLoop = 0; iLoop < 9; iLoop++){
        for(int jLoop = 0; jLoop < 9; jLoop++){
            CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(iLoop*9 + jLoop + 1));
            if(pBtn != nullptr){
                pBtn->clearState();
                int value = m_iLogic->Get_MatData(iLoop, jLoop);
                if(value != -1){
                    if(value) {
                        pBtn->add(value, false);
                    } else {
                        pBtn->remove(0, true);
                    }
                }
            }
        }
    }
}

void CGameBoard::highlightGrids(int rank, QVector<int> grids)
{
	int row = (rank-1)/9 + 1;
	int col = (rank-1)%9 + 1;     
	qDebug() << "gameboard::highlightGrids called";
    qDebug() << "SukoduGrid highlightGrid slot called";
    for(int i = 1; i <= 9; i++) {
        for(int j = 1; j <= 9; j++) {
            CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number((i-1)*9 + j));
            if(pBtn != nullptr){
                pBtn->clearState();
            }
        }
    }
    for(int i = 1; i <= 9; i++) {
        CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number((i-1)*9 + col));
        if(pBtn != nullptr){
            pBtn->highlight(true);
        }
        pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number((row-1)*9 + i));
        if(pBtn != nullptr){
            pBtn->highlight(true);
        }
    }
    for(int r : grids) {
        CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(r));
        if(pBtn != nullptr){
            pBtn->setSameNumHighlight(true);
        }

    }
    CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
    if(pBtn != nullptr){
        pBtn->setSameNumHighlight(true);
        pBtn->setChosen(true);
    }

    qDebug() << "SukoduGrid highlightGrid slot finished";    
	qDebug() << "gameboard::highlightGrids finished";
}

void CGameBoard::showWrong(const QVector<int>& grids)
{     
    qDebug() << "SukoduGrid showWrongMsg slot called";
    for(int rank : grids) {
        CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
        if(pBtn != nullptr){
            pBtn->setCorrect(false);
        }
    }
    qDebug() << "SukoduGrid showWrongMsg slot finished";    
}

void CGameBoard::showAnswer(const QVector<QVector<int> >& ans)
{
    for(int i = 1; i <= 9; i++) {
        for(int j = 1; j <= 9; j++) {
            if(ans[i][j]) {
                CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number((i-1)*9 + j));
                if(pBtn != nullptr){
                    pBtn->add(ans[i][j], false);
                }
            }
        }
    }    
}

void CGameBoard::setNum(int rank, int num, bool isAdd, bool editMode)
{
	m_nChosenSudukuGridBtn = rank;     
    qDebug() << "SukoduGrid setNum slot called";
    CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
    if(pBtn != nullptr){
        if(isAdd) {
            pBtn->add(num, editMode);
        } else {
            pBtn->remove(num, false);
        }
    }

    qDebug() << "SukoduGrid setNum slot finished";    
}

void CGameBoard::clearGrid(int rank)
{
    CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
    if(pBtn != nullptr){
        pBtn->remove(0, true);
    }
}

void CGameBoard::on_receive_operation(EN_Operation_Type op, int num)
{
	if(m_isStopped) return;
	qDebug() << "received operation" << "op = " << op << "number = " << num;
	if(m_nChosenSudukuGridBtn == -1) return;
	if (op == EN_Operation_CHECK) {
		emit check();
		return;
	}
	else if (op == EN_Operation_REMOVE) {
		emit clearGridMsg(m_nChosenSudukuGridBtn);
		return;
	}
	bool isAdd = false;
	bool editMode = false;
	if(op == EN_Operation_EDITADD) {
		isAdd = true;
		editMode = true;
	} else if (op == EN_Operation_INPUTADD) {
		isAdd = true;
		editMode = false;
	} else {
		qDebug() << "something wrong in GameBoard::on_receive_operation";
	}
	emit setNumMsg(m_nChosenSudukuGridBtn, num, isAdd, editMode);
}

void CGameBoard::setBtnChosen(int rank) // 数独区被选中的区域
{
    qDebug() << "chose btn" << rank;
	if(m_isStopped) return;
	m_nChosenSudukuGridBtn = rank;
    m_iLogic->calHighlights(rank);
}

void CGameBoard::on_pushButton_clicked()
{
	if(m_isStopped) return;
	qDebug() << "requesting answer";
	emit getAnswer();
}

void CGameBoard::updateTime()
{
    if(m_Exit){
        this->close();
        return;
    }
	// qDebug() << "GameBoard::updateTime() called";
    int secs = m_StartTime.secsTo(QDateTime::currentDateTime().time());
    ui->timeLCD->display(QTime(0,0,0).addSecs(secs).toString("hh:mm:ss"));
	// qDebug() << time.toString("hh:mm:ss");
}

void CGameBoard::on_startpausebtn_clicked()
{
	if(m_isStopped) {
        m_iComsumetimer->start(1000);
		m_isStopped = false;
        ui->m_iStartpauseBtn->setIcon(m_pauseIcon);
	} else {
        m_iComsumetimer->stop();
		m_isStopped = true;
        ui->m_iStartpauseBtn->setIcon(m_startIcon);
	}
}

void CGameBoard::On_RestartBtn_clicked()
{
	initTimer();
	emit restartMsg();
}

void CGameBoard::initTimer()
{
    m_StartTime = QDateTime::currentDateTime().time();
    ui->timeLCD->display(QTime(0,0,0).toString("hh:mm:ss"));
    if(!m_isStopped) m_iComsumetimer->start(50);
}

void CGameBoard::On_EditBtn_Click()
{
	if(m_enOperationMode != EN_Operation_EDITADD) {
		m_enOperationMode = EN_Operation_EDITADD;
		ui->m_iEditBtn->highlight(true);
	} else {
		m_enOperationMode = EN_Operation_INPUTADD;
		ui->m_iEditBtn->highlight(false);
	}
}

void CGameBoard::On_EraseBtn_Click()
{
	on_receive_operation(EN_Operation_REMOVE, 0);
}

void CGameBoard::On_CheckBtn_Click()
{
	on_receive_operation(EN_Operation_CHECK, 0);
}

void CGameBoard::sendOperation(int num)
{
	//qDebug() << mode << num;
    on_receive_operation(m_enOperationMode, num);
}

void CGameBoard::On_SudukuBtn_Clicked()
{
    CGridBtn* pBtn = qobject_cast<CGridBtn*>(sender());
    if(pBtn != nullptr){
        for(int i = 1; i <= 9; i++) { // 大九宫格
            for(int j = 1; j <= 9; j++) { // 子九宫格
                CGridBtn* pUiBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number((i-1)*9 + j));
                if(pUiBtn != nullptr){
                    if(pUiBtn == pBtn){
//                        setBtnChosen((i-1)*9 + j);
                        int rank = (i-1)*9 + j;
                        qDebug() << "chose btn" << rank;
                        if(m_isStopped) return;
                        m_nChosenSudukuGridBtn = rank;
                        m_iLogic->calHighlights(rank);
                        return;
                    }
                }
            }
        }
    }
    qDebug() << "can't find sender";
}

void CGameBoard::On_InputNumBtn_Clicked()
{
    QPushButton* pBtn = qobject_cast<QPushButton*>(sender());
    if(pBtn == nullptr){
        qDebug() << "invalid sender";
    }
    for(int i = 1; i <= 9; i++) {
        QPushButton* pNumBtn = findChild<QPushButton*>(QString("m_iInputNum%1Btn").arg(QString::number(i)));
        if(pNumBtn != nullptr && pNumBtn == pBtn){
            sendOperation(i);
            return;
        }
    }
}


