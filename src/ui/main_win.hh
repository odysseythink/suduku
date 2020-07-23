#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "ui/game_board_win.hh"

QT_BEGIN_NAMESPACE
namespace Ui { class CMainWin; }
QT_END_NAMESPACE

class CMainWin : public QWidget
{
	Q_OBJECT

public:
    explicit CMainWin(QWidget *parent = 0);
    ~CMainWin();

private slots:
	void on_startBtn_clicked();
    void On_AddBtn_Clicked();

private:
    Ui::CMainWin *ui;
};

#endif // MAINWINDOW_H
