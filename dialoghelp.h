#ifndef DIALOGHELP_H
#define DIALOGHELP_H

#include <QDialog>
#include <QtGui>
#include "videowidget.h"
namespace Ui {
class DialogHelp;
}

class DialogHelp : public QDialog
{
	Q_OBJECT
private:
	int m_index;
	int m_count;
	void updateHelp(int where);
	VideoWidget m_videoWidget;
	void fixTextSizes();
public:
	explicit DialogHelp(QWidget *parent = 0);
	~DialogHelp();
	void resetHelp();
	void hide();
private slots:

	void on_buttonNext_clicked();

	void on_DialogHelp_rejected();

	void on_buttonPrev_clicked();

	void on_opt_dontShowHelp_toggled(bool checked);

	void onRejected();

private:
	Ui::DialogHelp *ui;
};

#endif // DIALOGHELP_H
