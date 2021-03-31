## IoTaaP OS - SD card File system structure
FS structure will be automatically created on first boot

```
├── home
│   └── iotaap
│       └── data.log    
└── var
    └── log
        └── SYS00000.LOG
```

- home/iotaap/data.log is temporary file that will contain dropped data (e.g. if connection was lost) that will be removed after being successfully published
- var/log/SYS*****.log will contain system logs in multiple files limited to 50MB per file, oldest file will be dropped when total log size is greather than 2GB
