#ifndef _QUEUE_H
#define _QUEUE_H

#include <vector>
#include <queue>
#include <string>
using namespace std;
struct node
{
	int size;
	char *buf;
};
class PIC_QUE
{
	public:
		PIC_QUE();
		~PIC_QUE();
		int size();
		void enqueue(string &data);
		int  dequeue(string &data);
		void EN_Queue(vector<unsigned char>&buff);
		void DE_Queue(node &pic_node);		
	private:
		int MAX_SIZE = 20;
};



#endif
