#ifndef __LOCAL_DATA_H__
#define __LOCAL_DATA_H__

int InitMqttBackup( void);
void handleLocalMqttMessages( void);
bool FromContCreateJson( struct sMqttContainer *cont, char *buffer);

#endif