#ifndef __USER_CFG_H__
#define __USER_CFG_H__

int InitUserParameters( void);
bool uGetUserParameter( const char *Element, char *Output);
bool uGetUserParameter( const char *Element, int *Output);
bool uGetUserParameter( const char *Element, bool *Output);

#endif