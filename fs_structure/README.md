## IoTaaP OS File system structure
Must be burned to the SD card before usage. 

├── etc
│   ├── device
│   │   ├── custom.cfg 
│   │   └── default.cfg
│   └── ssl
│       └── certs       
│           └── ca.crt
├── home
│   └── iotaap
│       └── data.log    
└── var
    └── log
        └── SYS00000.LOG


- etc/device/default.cfg should be edited to match parameters given on IoTaaP console
- etc/device/custom.cfg can optionally contain custom jSON formatted data to be used from main program
- etc/ssl/certs/ca.crt Should be provided by CA
- home/iotaap/data.log is temporary file that will contain dropped data (e.g. if connection was lost) that will be removed after being successfully published
- var/log/SYS*****.log will contain system logs in multiple files limited to 50MB per file, oldest file will be dropped when total log size is greather than 2GB