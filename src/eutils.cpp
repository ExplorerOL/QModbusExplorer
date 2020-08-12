#include "eutils.h"

EUtils::EUtils()
{
}

QString EUtils::formatValue(int value,int frmt, bool is16Bit, bool isSigned=false)
{
    QString convertedValue;

    switch(frmt){

        case EUtils::Bin://Binary
        if (is16Bit)
            if (isSigned)
                convertedValue = QString("%1").arg((signed short)value,16,2,QLatin1Char('0'));
            else
                convertedValue = QString("%1").arg((unsigned short)value,16,2,QLatin1Char('0'));
        else

            convertedValue = QString("%1").arg(value,0,2);
        break;

        case EUtils::Dec://Decimal - Unsigned Integer
            if (isSigned)
                convertedValue = QString("%1").arg((signed short)value,0,10);
            else
                convertedValue = QString("%1").arg((unsigned short)value,0,10);
        break;

        case EUtils::Hex://Hex
            if (is16Bit)
                convertedValue = QString("%1").arg(value,4,16,QLatin1Char('0'));
            else
                convertedValue = QString("%1").arg(value,0,16);
        break;

        default://Default
        convertedValue = QString("%1").arg(value,0,10);

    }

    return convertedValue.toUpper();

}

QString EUtils::formatValue32(int valueHi, int valueLo, int endian = EUtils::Little, int precision = -1)
{//TODO : add float convertion
    union{
        struct{qint16 high, low;} reg;
        float value;
    } data;
    QString convertedValue;

    if (endian == EUtils::Little){
        data.reg.high = valueLo;
        data.reg.low = valueHi;
    }
    else if (endian == EUtils::Big){
        data.reg.high = valueHi;
        data.reg.low = valueLo;
    }

    convertedValue = QString("%1").arg(data.value, 0, 'G', precision);

    return convertedValue.toUpper();

}


QString EUtils::libmodbus_strerror(int errnum)
{
    switch (errnum) {

        case EINVAL:
            return "Protocol context is NULL";

        case ETIMEDOUT:
            return "Timeout";

        case ECONNRESET:
            return "Connection reset";

        case ECONNREFUSED:
            return "Connection refused";

        case EPIPE:
            return "Socket error";

        default://Default
            return modbus_strerror(errno);

    }

}

