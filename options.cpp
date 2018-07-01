#include "options.h"
Options *Options::m_pOptionsInstance = 0;


//###################### END SINGLETON #########################

Options::Options()
{
	SET_PLAYING_AS_ROOT = false;
}

void Options::testFunc()
{
	qDebug() << "test func!";
}
