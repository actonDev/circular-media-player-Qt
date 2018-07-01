#ifndef OPTIONS_H
#define OPTIONS_H
#include <QDebug>

class Options
{
private:
	Options();
	~Options(){qDebug() << "destructor";}
	Options(Options const&) {}
	Options& operator=(Options const&){}
	static Options *m_pOptionsInstance;

public:
	static Options *Instance()
	{
		if(m_pOptionsInstance == 0){
			m_pOptionsInstance = new Options();
		}
		return m_pOptionsInstance;
	}

	//##################
	//...my functions...
	//##################

	void testFunc();
	bool SET_PLAYING_AS_ROOT;
};
#endif // OPTIONS_H
