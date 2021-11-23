#ifndef __FS_MODULE_H__
#define __FS_MODULE_H__

void initializeFileSystem();
void FSmanagerTask(void *parameter);
void loadCertificate(const char *path);

#endif