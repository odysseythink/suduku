#ifndef PUZZLE_ADD_WIN_HH
#define PUZZLE_ADD_WIN_HH

#include <QDialog>

namespace Ui {
class CPuzzleAddWin;
}

class CPuzzleAddWin : public QDialog
{
    Q_OBJECT

public:
    explicit CPuzzleAddWin(const QString &level, QWidget *parent = nullptr);
    ~CPuzzleAddWin();

private slots:
    void On_Accepted();
    void On_NumEdit_Changed(const QString& txt);

private:
    Ui::CPuzzleAddWin *ui;
    int m_anPuzzleContent[9][9];
    QString m_strLevel;
};

#endif // PUZZLE_ADD_WIN_HH
