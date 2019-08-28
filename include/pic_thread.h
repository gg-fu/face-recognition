#ifndef PICTHREAD_H
#define PICTHREAD_H
 
#include <QThread>
 
class PicThread : public QThread
{
	public:
		PicThread();
		void closeThread();
			 
	protected:
		virtual void run();	     
	private:

		volatile bool isStop;       //isStop是易失性变量，需要用volatile进行申明
		int MAX_SIZE = 65535;
};
 
#endif
