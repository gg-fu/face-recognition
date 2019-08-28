#ifndef _RTP_CLIENT_H
#define _RTP_CLIENT_H

#include <rtpsession.h>
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

#include <stdio.h>
int RTP_Init(int destport,char *destip);
int RTP_Send(const char *buffer,int length);


#endif
