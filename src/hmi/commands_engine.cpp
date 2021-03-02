#include "commands_engine.h"
#include "./system/definitions.h"
#include "./system/utils.h"
#include "./fs/fs_module.h"
#include "./fs/sys_logs_data.h"
#include "./hmi/commands_mqtt.h"
#include "./hmi/commands_serial.h"
#include "./fs/serial_configuration.h"

bool IsDebugPrintAllowed = false;

const unsigned int MaxCommandLen = 30;
const unsigned int MaxLinesNumToSend = 100;
const unsigned int MaxTimestampLen = 40;
const unsigned int NumOfLinesPrintedInOneCall = 10;
const unsigned int MaxLogFileLineLength = 200;
const unsigned int MinSearchStringLen = 5;  // "[2020"
static unsigned int MaxSearchStringLen; // "[2020-01-01 12:33:42"

// Commands
const char DebugOn[]   = "debug_on";
const char DebugOff[]  = "debug_off";
const char LinesNum[]  = "lines:";
const char TimeComm[]  = "time:";
const char NextPage[]  = "np";
const char PrevPage[]  = "pp";
const char ClearLogs[] = "clear_sys_logs";
const char StopSearch[]= "stop";
const char SetParSys[] = "set-system";
const char SetParUsr[] = "set-user";

/* Number of lines to print via COM port */
int LinesNumToSend = 20;

/* Starting time for output */
char Timestamp[MaxTimestampLen];

/* Last file location - used for pp and np */
unsigned int FileLocation = 0;

/* Output location in file - this vars sets while searching */
unsigned int FileLocationPageStart = 0;
unsigned int FileIdPageStart = 0;

/* Next page output in file - this vars sets while printing */
unsigned int FileIdNextPageStart = 0;
unsigned int FileLocationNextPageStart = 0;

/* Serial communication engine commands */
bool NewTimeStampSearch = false;
bool SearchPreviousPage = false;
bool PrintDataOutput = false;
bool ClearSystemLogs = false;

/* Source of command */
enum eCommandSource CommandSource;

/* Output functions */
static void CommandSetResponse( char *Response);
static void CommandAddData( char *Data);
static void CommandSendResponse( void);
static void CommandSendStatus( char *Status);

/* This file pointer is used only and only for seek() function when "stop"
    command is called. When searching in file, engine sets this pointer to
    file so "stop" command can find seek file and finish find() function */
File *SearchFilePointer = NULL;

/**
 * @brief   Process command (from serial or mqtt)
 */
void ProcessCommand( const char *Command, enum eCommandSource Source)
{
    static int FillConfigurationActive = 0;
    CommandSource = Source;

    /* Set configuration parameters */
    if (Source==CommandSourceSerial && FillConfigurationActive)
    {
        SerialJsonCfgSetValue( (char *)Command);
        if (!SerialJsonCfgPrintKey())
        {
            FillConfigurationActive = 0;
        }
    }
    else if (Source==CommandSourceSerial &&
            (strstr( Command, SetParSys) || strstr( Command, SetParUsr)))
    {
        char StatusParWiz[] = "Parameter wizard (leave empty for unchanged):";
        CommandSendStatus( StatusParWiz);

        /* Open file and JSON */
        if (strstr( Command, SetParSys))
        {
            SerialJsonCfgSelectFile( SYS_CFG_PATH);
        }
        else //if (strstr( Command, SetParUsr))
        {
            SerialJsonCfgSelectFile( USER_CFG_PATH);
        }
        
        if (SerialJsonCfgPrintKey())
        {
            FillConfigurationActive = 1;
        }
    }
    /* Turn UART debug ON */
    else if (Source==CommandSourceSerial && strstr( Command, DebugOn))
    {
        IsDebugPrintAllowed = true;

        char StatusDebugOn[] = "Debug print on";
        CommandSendStatus( StatusDebugOn);
    }
    /* Turn UART debug OFF */
    else if (Source==CommandSourceSerial && strstr( Command, DebugOff))
    {
        IsDebugPrintAllowed = false;
        
        char StatusDebugOff[] = "Debug print off";
        CommandSendStatus( StatusDebugOff);
    }
    /* Set number of lines in output */
    else if (strstr( Command, LinesNum))
    {
        int TempLinesNum = atoi( strstr( Command, LinesNum)+strlen(LinesNum));
        if (TempLinesNum>0 && TempLinesNum<=MaxLinesNumToSend)
        {
            LinesNumToSend = TempLinesNum;

            char RespString[50];
            sprintf( RespString, "Print %u lines.", LinesNumToSend);
            CommandSetResponse( RespString);
            CommandSendResponse();
        }
        else
        {
            char RespString[50];
            sprintf( RespString, "Choose number from 1 to %u.", MaxLinesNumToSend);
            CommandSetResponse( RespString);
            CommandSendResponse();
        }
    }
    /* Set first page of output time */
    else if (strstr( Command, TimeComm))
    {
        strcpy( Timestamp, strstr( Command, TimeComm)+strlen(TimeComm));
        if (ValidateTime(Timestamp))
        {
            /* Start searching from first file */
            NewTimeStampSearch = true;
            SearchSysLogFileId = FirstSysLogFileId;
        }
        else
        {
            char ResponseTimeFormatInvalid[] = "Time format not valid. (yyyy-mm-dd hh:mm:ss)";
            CommandSetResponse( ResponseTimeFormatInvalid);
            CommandSendResponse();
        }
    }
    /* Clear system logs command */
    else if (strstr( Command, ClearLogs))
    {
        ClearSystemLogs = true;
    }
    /* Next page command */
    else if (strstr( Command, NextPage))
    {
        PrintDataOutput = true;
        FileLocationPageStart = FileLocationNextPageStart;
        FileIdPageStart = FileIdNextPageStart;
    }
    /* Previous page command */
    else if (strstr( Command, PrevPage))
    {
        SearchPreviousPage = true;
    }
    /* Stop command for searching */
    else if (strstr( Command, StopSearch))
    {
        NewTimeStampSearch = false;
        
        /* This file pointer is used only and only for seek() function when
            "stop" command is called. When searching in file, engine sets this
            pointer to file so "stop" command can find seek file and finish
            find() function - see declaration */
        if (SearchFilePointer)
        {
            SearchFilePointer->seek( SearchFilePointer->size());
        }
    }
    /* Print unknown command message */
    else
    {
        char ResponseCommandUnknown[] = "Unknown command";
        CommandSetResponse( ResponseCommandUnknown);
        CommandSendResponse();
    }
}

/**
 * @brief Print debug info
 * 
 * @param DebugString [in] String to print to serial port
 */
void PrintDebugInfo( const char* DebugString)
{
    if (IsDebugPrintAllowed)
    {
        Serial.println( DebugString);
    }
}

/**
 * @brief Searching in file, printing from file
 * 
 * @param OpenedLogFile [in] Pointer to file
 */
void CheckAndPrintLogFileData( File *OpenedLogFile)
{
    /* File does not exist */
    if (*OpenedLogFile == 0)
    {
        /* Open next or previous file */
        if (NewTimeStampSearch || PrintDataOutput)
        {
            SearchSysLogFileId++;
        }
        else// if (SearchPreviousPage)
        {
            SearchSysLogFileId--;
        }

        if (SearchSysLogFileId < FirstSysLogFileId ||
            SearchSysLogFileId > LastSysLogFileId)
        {
            char CorruptedFs[100];
            sprintf( CorruptedFs, "File system corrupted. Try again or use %s command to delete system log files.", ClearLogs);

            CommandSetResponse( CorruptedFs);
            CommandSendResponse();

            NewTimeStampSearch = false;
            PrintDataOutput = false;
            SearchPreviousPage = false;
        }
        return;
    }
    /* Search time string in file */
    /* In first iteration search for year only, in
        next iteration search for year and month, in
        next iteration search for year, month and day, etc.. */
    if (NewTimeStampSearch)
    {
        static int SearchLen = MinSearchStringLen;

        /* Using for next search iteration (but same string) */
        static unsigned int LastPositionFound = 0;
        static unsigned int LastFileIdWithFoundPosition = 0;
        static unsigned int NextSearchPosition = 0;

        /* Flags for string search */
        bool IsStringFound;
        bool IsStringFullyFoundAndSearchOver = false;
        static bool IsStringAtLeastPartiallyFound = false;

        char SearchString[MaxTimestampLen];

        /* Set string for search */
        strncpy( SearchString, Timestamp, SearchLen);
        SearchString[SearchLen] = '\0';

        /* Print to user - only in first run */
        if (SearchSysLogFileId == FirstSysLogFileId)
        {
            char StatusSearch[50];
            sprintf( StatusSearch, "Searching \"%s\"", SearchString+1);
            CommandSetResponse( StatusSearch);
            CommandSendResponse();
        }

        /* Position to last found point in file */
        OpenedLogFile->seek(NextSearchPosition);
        
        /* Search for string in file */
        /* This file pointer is used only and only for seek() function when
            "stop" command is called. When searching in file, engine sets this
            pointer to file so "stop" command can find seek file and finish
            find() function - see declaration */
        SearchFilePointer = OpenedLogFile;
        IsStringFound = OpenedLogFile->find(Timestamp, SearchLen);
        SearchFilePointer = NULL;

        /* If search is cancelled */
        if (!NewTimeStampSearch)
        {
            char ResponseSearchCanceled[] = "Search canceled!";
            CommandSetResponse( ResponseSearchCanceled);
            CommandSendResponse();

            /* File to open for printout */
            SearchSysLogFileId = FileIdPageStart;

            /* Reset found position for next iteration */
            LastPositionFound = 0;
            /* Reset string length */
            SearchLen = MinSearchStringLen;

            /* Search is over */
            PrintDataOutput = false;
            IsStringAtLeastPartiallyFound = false;
            return;
        }
        /* Locate string, expand search string if possible */
        else if (IsStringFound)
        {
            int Position = OpenedLogFile->position();
            Position = SearchLen>Position ? 0 : Position-SearchLen;
            OpenedLogFile->seek(Position);

            /* When found - expand search string length */
            SearchLen += 3;
            if (SearchLen > MaxSearchStringLen)
            {
                /* Set flag search is done */
                IsStringFullyFoundAndSearchOver = true;
            }
            /* Set flag something is found */
            IsStringAtLeastPartiallyFound = true;

            /* Remember this position - we found have data hete */
            LastPositionFound = Position;
            LastFileIdWithFoundPosition = SearchSysLogFileId;
            NextSearchPosition = Position;
        }
        /* String is not found - search in other files */
        else if (SearchSysLogFileId < LastSysLogFileId)
        {
            SearchSysLogFileId++;
            NextSearchPosition = 0;
            return;
        }

        /* If search is done */
        if (!IsStringFound || IsStringFullyFoundAndSearchOver)
        {
            /* If string is fully found */
            if (IsStringFullyFoundAndSearchOver)
            {
                char ResponseFound[] = "Found!";
                CommandSetResponse( ResponseFound);
                PrintDataOutput = true;
            }
            /* If at least something is found */
            else if (IsStringAtLeastPartiallyFound)
            {
                char ResponseFoundPartially[] = "Found partially!";
                CommandSetResponse( ResponseFoundPartially);
                PrintDataOutput = true;
            }
            /* If nothing is found (not even year) */
            else
            {
                char ResponseNotFound[] = "Not found!";
                CommandSetResponse( ResponseNotFound);
                CommandSendResponse();
                PrintDataOutput = false;
            }
            
            /* Remember start of page for printing */
            FileLocationPageStart = LastPositionFound;
            FileIdPageStart = LastFileIdWithFoundPosition;
            /* File to open for printout */
            SearchSysLogFileId = FileIdPageStart;

            /* Reset found position for next iteration */
            LastPositionFound = 0;
            /* Reset string length */
            SearchLen = MinSearchStringLen;

            /* Search is over */
            NewTimeStampSearch = false;
            IsStringAtLeastPartiallyFound = false;
        }
    }
    else if (SearchPreviousPage)
    {
        static bool NewPpSearchActive = true;
        static bool FirstIterationInThisFile = true;

        /* Count new lines */
        static int NewLinesCounter = 0;

        /* Position in file */
        int Position;
        char FileChar;

        /* If wrong file is opened */
        if (NewPpSearchActive && SearchSysLogFileId!=FileIdPageStart)
        {
            SearchSysLogFileId = FileIdPageStart;
            NewPpSearchActive = false;
            return;
        }
        NewPpSearchActive = false;

        /* Set position to begining of "next page" */
        if (FirstIterationInThisFile)
        {
            Position = FileLocationPageStart+1;
            FirstIterationInThisFile = false;
        }
        else
        {
            Position = OpenedLogFile->size();
        }

        /* Seek backward and read char by char until X new lines char are found */
        while (Position)
        {
            /* Seek to selected position in file */
            OpenedLogFile->seek(Position);

            /* Read char and check if it is new line */
            OpenedLogFile->readBytes(&FileChar, 1);

            /* New line detected? */
            if (FileChar == '\n')
            {
                NewLinesCounter++;
            }
            
            /* Go out if we have enough line for output */
            if (NewLinesCounter > LinesNumToSend)
            {
                /* Position to next line */
                Position++;
                break;
            }

            /* Go backward */
            Position --;
        }


        /* We found everything OR */
        /* We cannot move backward and no files backward, print beginning of first file */
        if (SearchSysLogFileId <= FirstSysLogFileId ||
            NewLinesCounter > LinesNumToSend)
        {
            SearchPreviousPage = false;
            PrintDataOutput = true;
            NewPpSearchActive = true;
            FirstIterationInThisFile = true;
            NewLinesCounter = 0;
            FileLocationPageStart = Position;
            FileIdPageStart = SearchSysLogFileId;
            FileLocation = Position;
        }
        /* At this point we are at beginning of the file */
        /* Move backward to new file */
        else
        {
            SearchSysLogFileId--;
        }
    }


    /* Print data to serial port */
    else if (PrintDataOutput)
    {
        static unsigned int NumOfPrintedLines = 0;
        static unsigned int OutputFileLocation;
        static bool FirstIterationThisRun = true;
        unsigned int StringLen;
        char Polje[MaxLogFileLineLength];

        /* Reset file location when start printing */
        if (FirstIterationThisRun)
        {
            FirstIterationThisRun = false;
            OutputFileLocation = FileLocationPageStart;
        }

        /* Print log file data */
        OpenedLogFile->seek( OutputFileLocation);

        /* Print line by line */
        for (int i=0; i<NumOfLinesPrintedInOneCall; i++)
        {
            StringLen = OpenedLogFile->readBytesUntil( '\n', Polje, MaxLogFileLineLength);

            /* If not EOF */
            if (StringLen)
            {
                Polje[StringLen] = '\0';
                if (Polje[StringLen-1] == '\r')
                {
                    StringLen--;
                    Polje[StringLen] = '\0';
                }

                /* Remember position for next call */
                OutputFileLocation = OpenedLogFile->position();

                /* Remember data for future ("np" command) */
                FileLocationNextPageStart = OpenedLogFile->position();
                FileIdNextPageStart = SearchSysLogFileId;

                /* Print line */
                CommandAddData( Polje);
                NumOfPrintedLines++;
            }
            /* EOF - check for other files */
            else if (SearchSysLogFileId < LastSysLogFileId)
            {
                SearchSysLogFileId++;
                OutputFileLocation = 0;
                break;
            }
            /* EOF - no more files */
            else
            {
                char StatusEndOfData[] = "End of data";
                CommandSendStatus( StatusEndOfData);
            }

            /* If entire page is printed or End Of Data */
            if (NumOfPrintedLines >= LinesNumToSend ||
                (SearchSysLogFileId == LastSysLogFileId && !StringLen))
            {
                NumOfPrintedLines = 0;
                FirstIterationThisRun = true;
                PrintDataOutput = false;
                CommandSendResponse();
                break;
            }
        }
    }
}


/**
 * @brief Is any command for serial output active?
 * 
 * @return true - active
 *         false- not active
 */
bool IsCommandActive( void)
{
    return (NewTimeStampSearch  ||
            PrintDataOutput     ||
            SearchPreviousPage);
}

/**
 * @brief Is command for clearing logs active?
 */
bool IsClearingLogsCommandActive( void)
{
    return ClearSystemLogs;
}

/**
 * @brief Reset command for clearing system logs
 */
void ResetClearingLogsCommand( void)
{
    ClearSystemLogs = false;
}
/**
 * @brief Validate input time, and save time string
 * 
 * @param TimeString [inout] Time string - could be changed
 * 
 * @return true - time string is ok
 *         false- time string not ok
 */
bool ValidateTime( char *TimeString)
{
    struct tm TimeStruct;
    int NumOfTimeinputs = sscanf(TimeString,"%d-%d-%d %d:%d:%d",
                                        &TimeStruct.tm_year,
                                        &TimeStruct.tm_mon,
                                        &TimeStruct.tm_mday,
                                        &TimeStruct.tm_hour,
                                        &TimeStruct.tm_min,
                                        &TimeStruct.tm_sec);

    /* We are checking length */
    if (NumOfTimeinputs)
    {
        TimeStruct.tm_year-= 1900;

        /* Set correct inputs into structure */
        TimeStruct.tm_mon  = (NumOfTimeinputs > 1) ? (TimeStruct.tm_mon-1) : 0;
        TimeStruct.tm_mday = (NumOfTimeinputs > 2) ? (TimeStruct.tm_mday) : 1;
        TimeStruct.tm_hour = (NumOfTimeinputs > 3) ? (TimeStruct.tm_hour) : 0;
        TimeStruct.tm_min  = (NumOfTimeinputs > 4) ? (TimeStruct.tm_min) : 0;
        TimeStruct.tm_sec  = (NumOfTimeinputs > 5) ? (TimeStruct.tm_sec) : 0;

        if (!mktime(&TimeStruct))
        {
            return false;
        }

        /* NumOfTimeinputs can be max 6 */
        NumOfTimeinputs = NumOfTimeinputs >= 6 ? 6 : NumOfTimeinputs;
    }
    else
    {
        return false;
    }

    /* Create string for file search */
    strftime(TimeString, MaxTimestampLen, "[%Y-%m-%d %H:%M:%S", &TimeStruct);
    MaxSearchStringLen = MinSearchStringLen + (NumOfTimeinputs-1)*3;
    
    return true;
}


/**
 * @brief Set response element in JSON
 */
static void CommandSetResponse( char *Response)
{
//Serial.println("SetResponse");
    if (CommandSource == CommandSourceMqtt)
    {
        MqttCommandSetResponse( Response);
    }
    else if (CommandSource == CommandSourceSerial)
    {
        SerialCommandSetResponse( Response);
    }
}


/**
 * @brief Add data to array. If data exceed given memory, send data and create new json
 */
static void CommandAddData( char *Data)
{
//Serial.println("AddData");
    if (CommandSource == CommandSourceMqtt)
    {
        MqttCommandAddData( Data);
    }
    else if (CommandSource == CommandSourceSerial)
    {
        SerialCommandAddData( Data);
    }
}


/**
 * @brief Send created JSON
 */
static void CommandSendResponse( void)
{
//Serial.println("SendResponse");
    if (CommandSource == CommandSourceMqtt)
    {
        MqttCommandSendResponse();
    }
    else if (CommandSource == CommandSourceSerial)
    {
        SerialCommandSendResponse();
    }
}


/**
 * @brief Send status
 */
static void CommandSendStatus( char *Status)
{
//Serial.println("SendStatus");
    if (CommandSource == CommandSourceMqtt)
    {
        MqttCommandSendStatus( Status);
    }
    else if (CommandSource == CommandSourceSerial)
    {
        SerialCommandSendStatus( Status);
    }
}