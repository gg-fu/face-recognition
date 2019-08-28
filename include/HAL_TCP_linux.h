#ifndef _HAL_TCP_LINUX_H
#define _HAL_TCP_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <sys/types.h>


uintptr_t HAL_TCP_Establish(const char *host, uint16_t port);
int HAL_TCP_Destroy(uintptr_t fd);
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms);
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
