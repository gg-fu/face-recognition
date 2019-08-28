#include "include/queue.h"
#include <iostream>

static queue<node> Q;
static queue<string> que;
PIC_QUE::PIC_QUE()
{

}

PIC_QUE::~PIC_QUE()
{

}

int PIC_QUE::size()
{
	return Q.size();
}

void PIC_QUE::EN_Queue(vector<unsigned char>&buff)
{
	if (Q.size() >= MAX_SIZE || buff.empty()){
		return;
	}
	node new_node;
	int size = buff.size();
	int pixel = size/3;
	new_node.buf = new char[size];
	for (int i = 0; i < size; i++)
	{
		
		new_node.buf[i] = buff[i];
	}
	new_node.size = size;
	Q.push(new_node);
}

void PIC_QUE::DE_Queue(node &pic_node)
{
	if (Q.empty()){
		return;
	}
	pic_node = Q.front();
	Q.pop();
}

void PIC_QUE::enqueue(string &data)
{
	if (data.empty()){
		return;
	}
	que.push(data);
}

int PIC_QUE::dequeue(string &data)
{
	if (que.empty()){
		return -1;
	}
	data = que.front();
	que.pop();
	return 0;
}
