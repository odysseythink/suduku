#ifndef GRIDBTN_H
#define GRIDBTN_H

#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QMouseEvent>

class CGridBtn : public QLabel
{
	Q_OBJECT
public:
    CGridBtn(QWidget* parent = 0);
	void paintEvent(QPaintEvent*);
    void Set_Highlight(bool isHighlight); // finish
    void Set_SameNumHighlight(bool isSameNumHighLight);
    void Set_Correct(bool isCorrect); // finish
    void Add_Num(int num, bool editMode); // finish
    void Remove_Num(int num, bool allRemove); // finish
    void Clear_State(); // finish
	void setSelfIcon(const QPixmap& myicon);

signals:
	void click();

private:
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent* ev);
	QRect getPosi(int r); // finish

private:
	QPixmap m_wrongIcon;
	QPixmap m_selfIcon;
	QString m_chosenColor;
	QString m_normalColor;
	QString m_sameNumColor;
	bool m_arr[9];
	bool m_editMode;
    bool m_bIsHighlight;
    bool m_bIsSameNumHighlight;
    bool m_bIsCorrect;
	bool m_haveSelfIcon;
};

#endif // GRIDBTN_H
