#include "grid_btn.hh"

#include <QDebug>
#include <QPushButton>
#include <QDebug>

CGridBtn::CGridBtn(QWidget* parent) : \
    QLabel(parent), m_bIsCorrect(true), m_editMode(false), m_bIsHighlight(false), m_bIsSameNumHighlight(false), m_haveSelfIcon(false)
{
    qDebug() << "CGridBtn constructor";
	for(int i = 0; i < 9; ++i) {
		m_arr[i] = false;
	}
    setText("1");
	this->setFrameShape (QFrame::Box);
//	this->setStyleSheet("border: 1px solid  #ff0000");

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_chosenColor = "background-color: #7bbfea";
	m_normalColor = "background-color: #fff";
	m_sameNumColor = "background-color: #ffff00";
}

void CGridBtn::paintEvent(QPaintEvent*)
{
	QPainter p(this);
//	this->setFrameShape(QFrame::Box);
    if(m_bIsHighlight) {
        if(m_bIsSameNumHighlight) {
			setStyleSheet(m_sameNumColor);
		} else {
			setStyleSheet(m_chosenColor);
		}
	} else {
		setStyleSheet(m_normalColor);
	}
    if(!m_bIsCorrect) {
		p.drawPixmap(0, 0, size().width(), size().height(), m_wrongIcon);
	}
	if(m_haveSelfIcon) {
		p.drawPixmap(0, 0, size().width(), size().height(), m_selfIcon);
	}
	QFont f("Helvetica");
	if(m_editMode) {
		f.setPixelSize(this->size().height() / 3);
		p.setFont(f);
		for(int i = 0; i < 9; ++i) {
			if(m_arr[i]) p.drawText(getPosi(i), QString::number(i + 1));
		}
	} else {
		f.setPixelSize(this->size().height() * 3 / 4);
		p.setFont(f);
		for(int i = 0; i < 9; ++i) {
			if(m_arr[i]) {
				p.drawText(getPosi(i), QString::number(i + 1));
				break;
			}
		}
	}
}

void CGridBtn::Set_Highlight(bool isHighlight)
{
    m_bIsHighlight = isHighlight;
	update();
}

void CGridBtn::Set_SameNumHighlight(bool isSameNumHighLight)
{
    m_bIsHighlight = true;
    m_bIsSameNumHighlight = isSameNumHighLight;
	update();
}

void CGridBtn::Set_Correct(bool iscorrect)
{
    m_bIsCorrect = iscorrect;
	update();
}

void CGridBtn::Add_Num(int num, bool editMode)
{
	if(!editMode) {
		for(int i = 0; i < 9; ++i) {
			m_arr[i] = false;
		}
	}

	m_arr[num - 1] = true;
	m_editMode = editMode;
	update();
}

void CGridBtn::Remove_Num(int num, bool allRemove)
{
	if(allRemove) {
		for(int i = 0; i < 9; ++i) {
			m_arr[i] = false;
		}
	} else {
		m_arr[num - 1] = false;
	}
	update();
}


void CGridBtn::Clear_State()
{
//    qDebug() << "CGridBtn::Clear_State text=" << text();
	//m_editMode = false;
    m_bIsCorrect = true;
    m_bIsHighlight = false;
    m_bIsSameNumHighlight = false;
	update();
}

void CGridBtn::setSelfIcon(const QPixmap& myicon)
{
	m_haveSelfIcon = true;
	m_selfIcon = myicon;
}

void CGridBtn::mousePressEvent(QMouseEvent*)
{
//    setColor(m_chosenColor);
//    setStyleSheet(m_chosenColor);
}

void CGridBtn::mouseReleaseEvent(QMouseEvent* ev)
{
	if(ev->x() < 0 || ev->x() > size().width() || ev->y() < 0 || ev->y() > size().height()) {
		return;
	}
	emit click();
}

QRect CGridBtn::getPosi(int r)
{
	int w = 0, h = 0, row = 0, col = 0;
	if(m_editMode) {
		w = this->size().width() / 3;
		h = this->size().height() / 3;
		row = r / 3;
		col = r % 3;
		return QRect(col * w + w / 4, row * h, w, h);
	} else {
		w = this->size().width();
		h = this->size().height();
		return QRect(w / 3, h / 8, w, h);
	}
}
