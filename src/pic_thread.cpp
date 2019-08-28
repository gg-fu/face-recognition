#include "include/pic_thread.h"
#include "include/rtp_client.h"
#include "include/queue.h"
#include <opencv2/highgui/highgui.hpp>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <vector>
#include <QDebug>
#include <sys/select.h>

PicThread::PicThread()
{
	isStop = false;
}
 
void PicThread::closeThread()
{
	isStop = true;
}

int UDP_recvfrom(intptr_t sockfd,   
                 unsigned char *p_data, 
                 unsigned int datalen,  
                 unsigned int timeout_ms) 
{
    int ret;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    fd_set read_fds;

    struct timeval timeout = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    ret = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret == 0) {
        return 0;    /* receive timeout */
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return -3;    /* want read */
        }
        return -4; /* receive failed */
    }

    ret = recvfrom(sockfd, p_data, datalen, 0, (struct sockaddr *)&addr, &addr_len);
    if (ret > 0) {
        return ret;
    }

    return -1;

}

void PicThread::run()
{
	PIC_QUE Q;
	int m_sockClient;
	
	unsigned char recv_data[64] = {0};
	if ((m_sockClient = socket(AF_INET, SOCK_DGRAM, 0)) < 0)    //创建socket句柄，采用UDP协议
	{
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return ;
	}
	sockaddr_in m_servaddr;
	memset(&m_servaddr, 0, sizeof(m_servaddr));  //初始化结构体
	m_servaddr.sin_family = AF_INET;           //设置通信方式
	m_servaddr.sin_port = htons(8000);         //设置端口
	m_servaddr.sin_addr.s_addr = inet_addr("192.168.100.128");
	bind(m_sockClient, (sockaddr*)&m_servaddr, sizeof(m_servaddr));//绑定端口号
	//RTP_Init(6000,"192.168.100.140");
	cv::Mat image;
	while (1)
	{
		if(isStop)
			return;
		
		node new_node = {0,NULL};
		Q.DE_Queue(new_node);
		if(new_node.size > 0){
			//RTP_Send(new_node.buf,new_node.size);
			/*std::vector<uchar> decode;
			for(int i=0;i<new_node.size;i++)
				decode.push_back(new_node.buf[i]);
			image = cv::imdecode(decode, CV_LOAD_IMAGE_COLOR);
			cv::imwrite("1.jpg",image);*/
			memset(recv_data,0,sizeof(recv_data));
			sendto(m_sockClient, new_node.buf, new_node.size, 0, (const sockaddr*)& m_servaddr, sizeof(m_servaddr));
			if (UDP_recvfrom(m_sockClient,recv_data, sizeof(recv_data),  800) > 0){
				string data = (char *)recv_data;
				Q.enqueue(data);
			}
			delete[] new_node.buf;
		}
	}
	delete recv_data;
}
