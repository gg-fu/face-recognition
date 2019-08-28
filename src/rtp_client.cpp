#include "include/rtp_client.h"
#include <iostream>
#include <string>
using namespace jrtplib;

static RTPSession sess;
// 错误处理函数
static int checkerror(int err){
	if (err < 0) {
		std::string errstr = RTPGetErrorString(err);
		std::cout<<"Error:"<<errstr<<std::endl;
		return -1;
        }
	return 0;
}

int RTP_Init(int destport,char *destip)
{
	int ret;
	if(destip == NULL || destport <= 0)
	{
		printf("port or ip error\n");
		return -1;
	}
	int status;
	uint16_t portbase = 6000;
	uint32_t IP = inet_addr(destip);

	// 创建RTP会话
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(1.0/10.0);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);
	ret = checkerror(status);
	if(ret != -1)
	{
		// 指定RTP数据接收端
		RTPIPv4Address addr(ntohl(IP),destport);
		status = sess.AddDestination(addr);
		ret = checkerror(status);
	}
	return ret;
}

int RTP_Send(const char *buffer,int length)
{
	if(buffer == NULL || length <= 0 )
		return -1;
	sess.SendPacket(buffer,length,0,false,10);
	return 0;
}
