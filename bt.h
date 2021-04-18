#ifndef __bt_h__
#define __bt_h__
#include "VescUart.h"


void bt_scan(void);
bool bt_connect(void);

extern VescUart VescConnection;
#endif
