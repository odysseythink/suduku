#include "puzzle_add_win.hh"
#include "ui_puzzle_add_win.h"
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include "db/puzzle_db.hh"

CPuzzleAddWin::CPuzzleAddWin(const QString &level, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPuzzleAddWin)
{
    ui->setupUi(this);
    m_strLevel = level;

    for(int i = 0; i < 9; i++) { // 大九宫格
        for(int j = 0; j < 9; j++) { // 子九宫格
            m_anPuzzleContent[i][j] = 0;
            QComboBox* pEdit = findChild<QComboBox*>("m_iNumEdit" + QString::number(i*9 + j + 1));
            if(pEdit != nullptr){
                qDebug() << "insert pBtn into pos " << (i-1)*9 + j;
                connect(pEdit, SIGNAL(currentTextChanged(QString)), this, SLOT(On_NumEdit_Changed(QString)));
            }
        }
    }
    setWindowTitle(tr("添加") +level + tr("数独"));
}

CPuzzleAddWin::~CPuzzleAddWin()
{
    delete ui;
}

void CPuzzleAddWin::On_Accepted()
{
    QJsonObject obj;
    for(int i = 0; i < 9; i++) { 
        QJsonArray array;
        for(int j = 0; j < 9; j++) { 
            array.append(QJsonValue(m_anPuzzleContent[i][j]));
        }
        obj.insert(QString::number(i+1), array);
    }
    QJsonDocument json_doc;
    json_doc.setObject(obj);
    qDebug() << json_doc.toJson(QJsonDocument::Compact);
    int level = 3;
    if(m_strLevel == tr("简单")){
        level = 1;
    } else if(m_strLevel == tr("中等")){
        level = 2;
    } else if(m_strLevel == tr("困难")){
        level = 3;
    }
    QString errmsg;
    if(!Add_Puzzle(json_doc.toJson(QJsonDocument::Compact), level, errmsg)){
        QMessageBox::critical(nullptr, tr("添加数独失败"),
                tr("添加数独失败:")+errmsg);
    }
}

void CPuzzleAddWin::On_NumEdit_Changed(const QString& txt)
{
    QComboBox* pEdit = qobject_cast<QComboBox*>(sender());
    if(pEdit == nullptr){
        qWarning() << "invalid sender";
        return;
    }
    for(int i = 0; i < 9; i++) { 
        for(int j = 0; j < 9; j++) { 
            QComboBox* pNumEdit = findChild<QComboBox*>("m_iNumEdit" + QString::number(i*9 + j + 1));
            if(pNumEdit != nullptr && pNumEdit == pEdit){
                if(txt == ""){
                    m_anPuzzleContent[i][j] = 0;
                } else {
                    bool isError = false;
                    int row = i / 3 * 3;
                    int col = j / 3 * 3;
                    for(int k = 0; k < 9; k++){
                        if(m_anPuzzleContent[i][k] == txt.toInt()){
                            isError = true;
                            break;
                        }
                        if(m_anPuzzleContent[k][j] == txt.toInt()){
                            isError = true;
                            break;
                        }
                    }
                    for(int k = 0; k < 3; k++){
                        for(int h = 0; h < 3; h++){
                            if(m_anPuzzleContent[k+row][h+col] == txt.toInt()){
                                isError = true;
                                break;
                            }
                        }
                    }
                    if(isError){
                        QMessageBox::critical(nullptr, "值错误",
                                txt + tr("已经存在"));
                        if(m_anPuzzleContent[i][j] == 0){
                            pEdit->setCurrentText("");
                        } else {
                            pEdit->setCurrentText(QString::number(m_anPuzzleContent[i][j]));
                        }
                        return;
                    }
                    m_anPuzzleContent[i][j] = txt.toInt();
                }
                return;
            }
        }
    }
}
