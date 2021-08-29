#include "serial_configuration.h"

#include "./system/definitions.h"
#include "./system/system_configuration.h"
#include "FFat.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./fs/json_memory.h"
#include "./fs/sys_logs_data.h"
#include "./configurator/configurator.h"
#include "./system/system_json.h"


/* Pointer to a filename, system or user */
const char *SelectedFile = NULL;
/* data change wizard active flag */
bool IsWizardActive = false;
/* Flag to command write new data to JSON */
bool WriteDataToNewJson = false;

/* Flag to command write data to certificate file */
static bool ExportCertificate = false;
/* Flag to command write data to JSON from syscfg */
static bool ExportJson = false;
static bool ExportJsonResetAfter = false;

/* Json objects */
DynamicJsonDocument *JsonDocOld = NULL;
DynamicJsonDocument *JsonDocNew = NULL;
JsonObject JsonRoot;
JsonObject::iterator KeyNum;


/**
 * @brief Select file for wizard
 */
void SerialJsonCfgSelectFile( const char *FilePath)
{
    SelectedFile = FilePath;
}


/**
 * @brief Prepeare new JSON object and burn it to new file. This runs in fs_module
 */
void HandleJsonCfgFile( void)
{
    if (SelectedFile == NULL || IsWizardActive)
    {
        if (ExportCertificate && SystemGetCAcertificate())
        {
            ExportCertificate = false;
            FFat.remove( (char *)CA_CRT_PATH);
            
            fs::File CertFile = FFat.open( CA_CRT_PATH, FILE_WRITE);

            if (!CertFile)
            {
                Serial.println("(data is not saved)");
                return;
            }
            CertFile.write( (uint8_t *)SystemGetCAcertificate(),
                                strlen(SystemGetCAcertificate()));
            CertFile.flush();
            CertFile.close();
        }

        if (ExportJson)
        {
            if (!JsonDocNew)
            {
                JsonDocNew = new DynamicJsonDocument(2048);
            }
            JsonDocNew->clear();
            for (int i=0; configuratorGetConf(i); i++)
            {
                struct configParameters *conf = configuratorGetConf(i);
                struct sJsonKeys *JsDt = conf->Data;

                for (int j=0; j<conf->Size; j++, JsDt++)
                {
                    switch (JsDt->ElementDataType)
                    {
                        case JsonDataTypeInt:
                        {
                            (*JsonDocNew)[JsDt->ElementKey] = *(int *)JsDt->ElementPointer;
                        }
                        break;

                        case JsonDataTypeString:
                        case JsonDataTypePass:
                        {
                            (*JsonDocNew)[JsDt->ElementKey] = (char *)*JsDt->ElDoublePointer;
                        }
                        break;
                        
                        case JsonDataTypeBool:
                        {
                            (*JsonDocNew)[JsDt->ElementKey] = *(bool *)JsDt->ElementPointer;
                        }
                        break;
                    }
                }
            }
            ExportJson = false;
            WriteDataToNewJson = true;
            SelectedFile = SYS_CFG_PATH;
        }

        /* Write data to new JSON */
        if (WriteDataToNewJson)
        {
            WriteDataToNewJson = false;
            IsWizardActive = false;
            
            Serial.println();
            Serial.print("Leaving IoTaaP Configurator ");

            if (SelectedFile)
            {
                char FileToRemove[100];
                strcpy( FileToRemove, SelectedFile);
                FFat.remove( FileToRemove);

                fs::File ConfigFile = FFat.open( SelectedFile, FILE_WRITE);
                SelectedFile = NULL;

                if (!ConfigFile)
                {
                    Serial.println("(data is not saved)");
                    return;
                }
                serializeJsonPretty( *JsonDocNew, ConfigFile);

                ConfigFile.flush();
                ConfigFile.close();

                delete JsonDocNew;
                delete JsonDocOld;
                JsonDocNew = NULL;
                JsonDocOld = NULL;
                
                Serial.println("(data is saved)");
                if (ExportJsonResetAfter)
                {
                    ConfiguratorDeactivate();
                    vTaskDelay(1000/portTICK_PERIOD_MS);
                    ESP.restart();
                }
            }
        }
        return;
    }

    /* Prepare new JSON */
    fs::File ConfigFile = FFat.open( SelectedFile, FILE_READ);
    if (!ConfigFile)
    {
        SelectedFile = NULL;
        return;
    }

    ConfigFile.seek(0);
    int JsonSize = CalculateDynamicMemorySizeForJson( ConfigFile);
    JsonDocNew = new DynamicJsonDocument(JsonSize);
    JsonDocOld = new DynamicJsonDocument(JsonSize*3/2);

    /* Clear JSONs */
    JsonDocNew->clear();
    JsonDocOld->clear();

    /* Deserialize and connect everything */
    ConfigFile.seek(0);
    deserializeJson(*JsonDocOld, ConfigFile);
    JsonRoot = JsonDocOld->as<JsonObject>();
    KeyNum = JsonRoot.begin();

    /* Close file and exit */
    ConfigFile.close();

    IsWizardActive = true;
    return;
}


/**
 * @brief   Set data to new JSON. If no new data, data will be copied from old JSON
 * @return  true - There are still some elements in JSON file
 *          false - No more elements in JSON file
 */
bool SerialJsonCfgPrintKey( void)
{
    while (!IsWizardActive)
    {
        if (SelectedFile==NULL && !IsWizardActive)
        {
            return false;
        }
        vTaskDelay( 1 / portTICK_PERIOD_MS);
    }

    if (KeyNum == JsonRoot.end())
    {
        KeyNum = JsonRoot.begin();
        WriteDataToNewJson = true;
        return false;
    }

    Serial.println();

    /* Print data type */
    if (KeyNum->value().is<char*>())
    {
        Serial.print( " String");
    }
    else if (KeyNum->value().is<int>())
    {
        Serial.print( "Integer");
    }
    else if (KeyNum->value().is<bool>())
    {
        Serial.print( "Boolean");
    }

    /* Print element key */
    Serial.print( " \"");
    Serial.print( KeyNum->key().c_str());
    Serial.print( "\":");

    /* Print element value */
    if (KeyNum->value().is<char*>())
    {
        Serial.println( KeyNum->value().as<char*>());
    }
    else if (KeyNum->value().is<int>())
    {
        Serial.println( KeyNum->value().as<int>());
    }
    else if (KeyNum->value().is<bool>())
    {
        Serial.println(KeyNum->value().as<bool>() ? "true": "false");
    }

    /* Print input for new value */
    Serial.print( "    New \"");
    Serial.print( KeyNum->key().c_str());
    Serial.print( "\":");

    return true;
}


/**
 * @brief   Set data to new JSON. If no new data, data will be copied from old JSON
 * @par     Data - new data
 */
void SerialJsonCfgSetValue( char *Data)
{
    if (Data[0]=='\n' || Data[0]=='\r' || Data[0] == '\0')
    {
        /* Do nothing */
        Serial.println( "--> Not changed!");
        (*JsonDocNew)[KeyNum->key().c_str()] = (*JsonDocOld)[KeyNum->key().c_str()];
    }
    else
    {   
        /* If any parameter is changed - reset after */
        ExportJsonResetAfter = true;

        /* New Data Detected */
        Serial.print( "--> New \"");
        Serial.print( KeyNum->key().c_str());
        Serial.print( "\":");

        if (KeyNum->value().is<char*>())
        {
            (*JsonDocNew)[KeyNum->key().c_str()] = Data;
            Serial.println( Data);
        }
        else if (KeyNum->value().is<int>())
        {
            int VarNum = atoi( Data);
            (*JsonDocNew)[KeyNum->key().c_str()] = VarNum;
            Serial.println( VarNum);
        }
        else if (KeyNum->value().is<bool>())
        {
            bool VarBool;
            if (strstr( Data, "true"))
            {
                VarBool = true;
                (*JsonDocNew)[KeyNum->key().c_str()] = VarBool;
                Serial.println("true");
            }
            else if (strstr( Data, "false"))
            {
                VarBool = false;
                (*JsonDocNew)[KeyNum->key().c_str()] = VarBool;
                Serial.println("false");
            }
            else
            {
                /* Do nothing */
                Serial.println( "--> Not changed!");
                (*JsonDocNew)[KeyNum->key().c_str()] = (*JsonDocOld)[KeyNum->key().c_str()];
            }
        }
    }
    
    ++KeyNum;
}


/**
 * @brief   Switch flags to save cfg data to JSON
 */
bool FromCfgSaveData( bool ResetAfter)
{
    if (IsWizardActive)
    {
        return false;
    }

    ExportCertificate = true;
    ExportJson = true;
    ExportJsonResetAfter = ResetAfter;

    SerialJsonCfgSelectFile( SYS_CFG_PATH);

    return true;
}