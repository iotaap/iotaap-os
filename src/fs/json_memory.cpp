#include "json_memory.h"

/**
 * @brief Calculate memory needed for JSON deserializer
 * 
 */
int CalculateDynamicMemorySizeForJson( File JsonOpenedfile)
{
    /* Max number of characters between '\"', ':' and/or ',' */
    const int MaxJsonValueLength = 100;

    enum eJsonStage
    {
        StartJson,
        StartQuote,
        EndQuote,
        TwoDots,
        StartData,

        StringData,
        IntData,
        BoolData,
        NullData,

        EndData
    };

    char ThisChar, LastChar = 0;
    enum eJsonStage LastStage = StartJson, JsonStage = StartJson;
    int OutputDataStringsLength = 0;
    int OutputDataElementsLength = 0;
    int DataTimeout = 0;

    if (!JsonOpenedfile)
    {
        return 0;
    }

    while (1)
    {
        ThisChar = JsonOpenedfile.read();

        
        /* Wrong data or algorithm detection */
        DataTimeout++;
        if (LastStage != JsonStage)
        {
            LastStage = JsonStage;
            DataTimeout = 0;
        }
        else if (DataTimeout > MaxJsonValueLength)
        {
            return -1;
        }

        switch (JsonStage)
        {
            /* Search for '\"' (tag start) */
            case StartJson:
            case StartQuote:
            {
                if (ThisChar == '\"')
                {
                    OutputDataElementsLength++;
                    JsonStage = EndQuote;
                }
                /* End of file */
                else if (ThisChar == '\0' || ThisChar == 0xFF)
                {
                    return OutputDataElementsLength*16 + OutputDataStringsLength+(OutputDataStringsLength>>3);
                }
            }
            break;


            /* Search for '\"' (tag end) */
            case EndQuote:
            {
                OutputDataStringsLength++;
                if (ThisChar == '\"' && LastChar != '\\')
                {
                    JsonStage = TwoDots;
                }

                /* If two '\\' in row characters - they are escaping each other.
                    Change ThisChar (in next iteration this will be LastChar) to avoid undetected '\"' */
                if (ThisChar == '\\' && LastChar != '\\')
                {
                    ThisChar = '.';
                }
            }
            break;


            /* Searh for ':' */
            case TwoDots:
            {
                if (ThisChar == ':')
                {
                    JsonStage = StartData;
                }
            }
            break;


            /* Search for start */
            case StartData:
            {
                /* String data */
                if (ThisChar == '\"')
                {
                    JsonStage = StringData;
                }

                /* Numeric data */
                else if (ThisChar >= '0' && ThisChar <= '9')
                {
                    JsonStage = IntData;
                }

                /* Bool data */
                else if (ThisChar == 't' ||
                        ThisChar == 'f')
                {
                    JsonStage = BoolData;
                }

                /* NULL data */
                else if (ThisChar == 'n')
                {
                    JsonStage = NullData;
                }
            }
            break;

            /* Search for string end */
            case StringData:
            {
                OutputDataStringsLength++;
                if (ThisChar == '\"' && LastChar != '\\')
                {
                    JsonStage = EndData;
                }

                /* If two '\\' in row characters - they are escaping each other.
                    Change ThisChar (in next iteration this will be LastChar) to avoid undetected '\"' */
                if (ThisChar == '\\' && LastChar != '\\')
                {
                    ThisChar = '.';
                }
            }
            break;


            case IntData:
            case BoolData:
            case NullData:
            /* Search for element end ',' */
            case EndData:
            {
                /* End data or end file */
                if (ThisChar == ',')
                {
                    JsonStage = StartQuote;
                }
                else if (ThisChar == '\0' || ThisChar == 0xFF)
                {
                    return OutputDataElementsLength*16 + OutputDataStringsLength + (OutputDataStringsLength>>3);
                }
            }
            break;
        }

        LastChar = ThisChar;
    }
}
