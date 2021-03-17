#ifndef __IOTAAP_OS_CONFIGURATOR_H__
#define __IOTAAP_OS_CONFIGURATOR_H__


struct configParameters
{
    struct sJsonKeys *Data;
    int Size;
};

void ConfiguratorInit( void);
void startConfigurator( void);
void configuratorRegister( struct sJsonKeys *JsonSystemData, int JsonStructSize);
struct configParameters *configuratorGetConf( unsigned int id);

void HandleConfiguratorActivity( void *par);
void ConfiguratorActivate( void);
void ConfiguratorDeactivate( void);
bool IsConfiguratorActive( void);

#endif