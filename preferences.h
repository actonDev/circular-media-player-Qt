#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
	Q_OBJECT
	
public:
	explicit Preferences(QWidget *parent = 0);
	~Preferences();
	
private slots:
	void reject();
	void accept();
	void apply();
	void authenticate();
	void onLastFmAuthorization(QString text);

private:
	Ui::Preferences *ui;
	void resetOptions();
	void setOptions();
};

#endif // PREFERENCES_H
