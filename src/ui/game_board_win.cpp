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
    m_iConsumeTimer = std::make_shared<QTimer>(this);
    __Init_Timer();
    connect(m_iConsumeTimer.get(), SIGNAL(timeout()), this, SLOT(__On_ConsumeTimer_Timeout()));

    for(int i = 0; i < 9; i++) { // 大九宫格
        for(int j = 0; j < 9; j++) { // 子九宫格
            CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(i*9 + j + 1));
            if(pBtn != nullptr){
                connect(pBtn, SIGNAL(click()), this, SLOT(__On_SudukuBtn_Clicked()));
            }
        }
    }

	ui->m_iEditBtn->setSelfIcon(QPixmap(":/icon/resources/icons/edit.png"));
	ui->m_iEraseBtn->setSelfIcon(QPixmap(":/icon/resources/icons/eraser.png"));
	ui->m_iCheckBtn->setSelfIcon(QPixmap(":/icon/resources/icons/check.png"));

    m_iLogic = std::make_shared<LogicController>();
    connect(m_iLogic.get(), SIGNAL(showProblem()), this, SLOT(__On_Show_Problem()));
    connect(m_iLogic.get(), SIGNAL(setNumMsg(uint32_t,uint32_t,bool,bool)), this, SLOT(__On_SetNum(uint32_t,uint32_t,bool,bool)));
    connect(m_iLogic.get(), SIGNAL(showWrong(QVector<int>)), this, SLOT(__On_Show_Wrong(QVector<int>)));
    connect(m_iLogic.get(), SIGNAL(clearGridMsg(uint32_t)), this, SLOT(__On_ClearGrid(uint32_t)));
    connect(m_iLogic.get(), SIGNAL(highlightGrids(int,QVector<int>)), this, SLOT(__On_Highlight_Grids(int,QVector<int>)));
    connect(m_iLogic.get(), SIGNAL(showAnswer(QVector<QVector<int> >)), this, SLOT(showAnswer(QVector<QVector<int> >)));

    if(!m_iLogic->Start_Game(pInfo)){
        qDebug() << "start game failed";
        m_Exit = true;
    }
}

CGameBoard::~CGameBoard()
{
	delete ui;
}

void CGameBoard::__On_Show_Problem()
{
    for(int iLoop = 0; iLoop < 9; iLoop++){
        for(int jLoop = 0; jLoop < 9; jLoop++){
            CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(iLoop*9 + jLoop + 1));
            if(pBtn != nullptr){
                pBtn->Clear_State();
                int value = m_iLogic->Get_MatData(iLoop, jLoop);
                if(value != -1){
                    if(value) {
                        pBtn->Add_Num(value, false);
                    } else {
                        pBtn->Remove_Num(0, true);
                    }
                }
            }
        }
    }
}

void CGameBoard::__On_Highlight_Grids(int rank, QVector<int> grids)
{
    int row = (rank-1)/9;
    int col = (rank-1)%9;
    qDebug() << "__On_Highlight_Grids slot called";
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(i*9 + j+1));
            if(pBtn != nullptr){
                pBtn->Clear_State();
            }
        }
    }
    for(int i = 0; i < 9; i++) {
        CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(i*9 + col + 1));
        if(pBtn != nullptr){
            pBtn->Set_Highlight(true);
        }
        pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(row*9 + i + 1));
        if(pBtn != nullptr){
            pBtn->Set_Highlight(true);
        }
    }
    for(int r : grids) {
        CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(r));
        if(pBtn != nullptr){
            pBtn->Set_SameNumHighlight(true);
        }

    }
    CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
    if(pBtn != nullptr){
        pBtn->Set_SameNumHighlight(true);
    }
}

void CGameBoard::__On_Show_Wrong(const QVector<int>& grids)
{     
    qDebug() << "SukoduGrid showWrongMsg slot called";
    for(int rank : grids) {
        CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
        if(pBtn != nullptr){
            pBtn->Set_Correct(false);
        }
    }
    qDebug() << "SukoduGrid showWrongMsg slot finished";    
}

void CGameBoard::showAnswer(const QVector<QVector<int> >& ans)
{
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(ans[i][j]) {
                CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(i*9 + j + 1));
                if(pBtn != nullptr){
                    pBtn->Add_Num(ans[i][j], false);
                }
            }
        }
    }    
}

void CGameBoard::__On_SetNum(uint32_t rank, uint32_t num, bool isAdd, bool editMode)
{
    if(rank == 0 || rank > 81 || num == 0 || num > 9){
        return;
    }
	m_nChosenSudukuGridBtn = rank;     
    qDebug() << "SukoduGrid __On_SetNum slot called";
    CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
    if(pBtn != nullptr){
        if(isAdd) {
            pBtn->Add_Num(num, editMode);
        } else {
            pBtn->Remove_Num(num, false);
        }
    }

    qDebug() << "SukoduGrid __On_SetNum slot finished";
}

void CGameBoard::__On_ClearGrid(uint32_t rank)
{
    if(rank == 0 || rank > 81 ){
        return;
    }
    CGridBtn* pBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(rank));
    if(pBtn != nullptr){
        pBtn->Remove_Num(0, true);
    }
}

void CGameBoard::__Receive_Operation(EN_Operation_Type op, uint32_t num)
{
    if(num == 0 || num > 9) return;
	if(m_isStopped) return;
	qDebug() << "received operation" << "op = " << op << "number = " << num;
	if (op == EN_Operation_CHECK) {
        m_iLogic->Check();
		return;
	}
	else if (op == EN_Operation_REMOVE) {
        m_iLogic->Clear_Grid(m_nChosenSudukuGridBtn);
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
    m_iLogic->Set_Num(m_nChosenSudukuGridBtn, num, isAdd, editMode);
}

void CGameBoard::__On_SolveBtn_Clicked()
{
	if(m_isStopped) return;
	qDebug() << "requesting answer";
    m_iLogic->Calc_Answer();
}

void CGameBoard::__On_ConsumeTimer_Timeout()
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

void CGameBoard::__On_StartpauseBtn_Clicked()
{
	if(m_isStopped) {
        m_iConsumeTimer->start(1000);
		m_isStopped = false;
        ui->m_iStartpauseBtn->setIcon(m_pauseIcon);
	} else {
        m_iConsumeTimer->stop();
		m_isStopped = true;
        ui->m_iStartpauseBtn->setIcon(m_startIcon);
	}
}

void CGameBoard::__On_RestartBtn_clicked()
{
    __Init_Timer();
    m_iLogic->Restart_Game();
}

void CGameBoard::__Init_Timer()
{
    m_StartTime = QDateTime::currentDateTime().time();
    ui->timeLCD->display(QTime(0,0,0).toString("hh:mm:ss"));
    if(!m_isStopped) m_iConsumeTimer->start(50);
}

void CGameBoard::__On_EditBtn_Click()
{
	if(m_enOperationMode != EN_Operation_EDITADD) {
		m_enOperationMode = EN_Operation_EDITADD;
        ui->m_iEditBtn->Set_Highlight(true);
	} else {
		m_enOperationMode = EN_Operation_INPUTADD;
        ui->m_iEditBtn->Set_Highlight(false);
	}
}

void CGameBoard::__On_EraseBtn_Click()
{
    __Receive_Operation(EN_Operation_REMOVE, 0);
}

void CGameBoard::__On_CheckBtn_Click()
{
    __Receive_Operation(EN_Operation_CHECK, 0);
}

void CGameBoard::__Send_Operation(uint32_t num)
{
    if(num == 0 || num > 9) return;
	//qDebug() << mode << num;
    __Receive_Operation(m_enOperationMode, num);
}

void CGameBoard::__On_SudukuBtn_Clicked()
{
    CGridBtn* pBtn = qobject_cast<CGridBtn*>(sender());
    if(pBtn != nullptr){
        for(int i = 0; i < 9; i++) { // 大九宫格
            for(int j = 0; j < 9; j++) { // 子九宫格
                CGridBtn* pUiBtn = findChild<CGridBtn*>("m_iSudukuBtn" + QString::number(i*9 + j + 1));
                if(pUiBtn != nullptr){
                    if(pUiBtn == pBtn){
                        int rank = i*9 + j + 1;
                        qDebug() << "chose btn" << rank;
                        if(m_isStopped) return;
                        m_nChosenSudukuGridBtn = rank;
                        m_iLogic->Calc_Highlights(rank);
                        return;
                    }
                }
            }
        }
    }
    qDebug() << "can't find sender";
}

void CGameBoard::__On_InputNumBtn_Clicked()
{
    QPushButton* pBtn = qobject_cast<QPushButton*>(sender());
    if(pBtn == nullptr){
        qDebug() << "invalid sender";
    }
    for(uint32_t i = 1; i <= 9; i++) {
        QPushButton* pNumBtn = findChild<QPushButton*>(QString::asprintf("m_iInputNum%dBtn", i));
        if(pNumBtn != nullptr && pNumBtn == pBtn){
            __Send_Operation(i);
            return;
        }
    }
}


