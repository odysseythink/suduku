#include "grid_btn.hh"

#include <QDebug>
#include <QPushButton>
#include <QDebug>

CGridBtn::CGridBtn(QWidget* parent) : \
	QLabel(parent), m_chosen(false), m_correct(true), m_editMode(false), m_highlight(false), m_sameNumHighlight(false), m_haveSelfIcon(false)
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
    m_chosenIcon = QPixmap(":/icon/resources/icons/circle.png");
    m_wrongIcon = QPixmap(":/icon/resources/icons/cross.png");
}

void CGridBtn::paintEvent(QPaintEvent*)
{
	QPainter p(this);
//	this->setFrameShape(QFrame::Box);
	if(m_highlight) {
		if(m_sameNumHighlight) {
			setStyleSheet(m_sameNumColor);
		} else {
			setStyleSheet(m_chosenColor);
		}
	} else {
		setStyleSheet(m_normalColor);
	}
	if(m_chosen) {
		p.drawPixmap(0, 0, size().width(), size().height(), m_chosenIcon);
	}
	if(!m_correct) {
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

void CGridBtn::highlight(bool isHighlight)
{
	m_highlight = isHighlight;
	update();
}

void CGridBtn::setSameNumHighlight(bool isSameNumHighLight)
{
	m_highlight = true;
	m_sameNumHighlight = isSameNumHighLight;
	update();
}

void CGridBtn::setChosen(bool isChosen)
{
	m_chosen = isChosen;
	update();
}

void CGridBtn::setCorrect(bool iscorrect)
{
	m_correct = iscorrect;
	update();
}

void CGridBtn::add(int num, bool editMode)
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

void CGridBtn::remove(int num, bool allRemove)
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


void CGridBtn::clearState()
{
//    qDebug() << "CGridBtn::clearState text=" << text();
	//m_editMode = false;
	m_chosen = false;
	m_correct = true;
	m_highlight = false;
	m_sameNumHighlight = false;
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
//    m_chosen = !m_chosen;
//    update();
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
