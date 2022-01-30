/****************************************************************************
** Copyright (c) 2000-2003 Wayne Roth
** Copyright (c) 2004-2007 Stefan Sander
** Copyright (c) 2007 Michal Policht
** Copyright (c) 2008 Brandon Fosdick
** Copyright (c) 2009-2010 Liam Staskawicz
** Copyright (c) 2011 Debao Zhang
** All right reserved.
** Web: http://code.google.com/p/qextserialport/
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#ifndef _QEXTSERIALPORT_H_
#define _QEXTSERIALPORT_H_

#include <QtCore/QIODevice>
#include "qextserialport_global.h"
#ifdef Q_OS_UNIX
#include <termios.h>
#endif
/*line status constants*/
// ### QESP2.0 move to enum
#define LS_CTS  0x01
#define LS_DSR  0x02
#define LS_DCD  0x04
#define LS_RI   0x08
#define LS_RTS  0x10
#define LS_DTR  0x20
#define LS_ST   0x40
#define LS_SR   0x80

/*error constants*/
// ### QESP2.0 move to enum
#define E_NO_ERROR                   0
#define E_INVALID_FD                 1
#define E_NO_MEMORY                  2
#define E_CAUGHT_NON_BLOCKED_SIGNAL  3
#define E_PORT_TIMEOUT               4
#define E_INVALID_DEVICE             5
#define E_BREAK_CONDITION            6
#define E_FRAMING_ERROR              7
#define E_IO_ERROR                   8
#define E_BUFFER_OVERRUN             9
#define E_RECEIVE_OVERFLOW          10
#define E_RECEIVE_PARITY_ERROR      11
#define E_TRANSMIT_OVERFLOW         12
#define E_READ_FAILED               13
#define E_WRITE_FAILED              14
#define E_FILE_NOT_FOUND            15
#define E_PERMISSION_DENIED         16
#define E_AGAIN                     17

/**
 * @brief
 *
 * @enum BaudRateType
 */
enum BaudRateType
{
#if defined(Q_OS_UNIX) || defined(qdoc)
    BAUD50 = 50,                //POSIX ONLY
    BAUD75 = 75,                //POSIX ONLY
    BAUD134 = 134,              //POSIX ONLY
    BAUD150 = 150,              //POSIX ONLY
    BAUD200 = 200,              //POSIX ONLY
    BAUD1800 = 1800,            //POSIX ONLY
#  if defined(B76800) || defined(qdoc)
    BAUD76800 = 76800,          //POSIX ONLY
#  endif
#  if (defined(B230400) && defined(B4000000)) || defined(qdoc)
    BAUD230400 = 230400,        //POSIX ONLY
    BAUD460800 = 460800,        //POSIX ONLY
    BAUD500000 = 500000,        //POSIX ONLY
    BAUD576000 = 576000,        //POSIX ONLY
    BAUD921600 = 921600,        //POSIX ONLY
    BAUD1000000 = 1000000,      //POSIX ONLY
    BAUD1152000 = 1152000,      //POSIX ONLY
    BAUD1500000 = 1500000,      //POSIX ONLY
    BAUD2000000 = 2000000,      //POSIX ONLY
    BAUD2500000 = 2500000,      //POSIX ONLY
    BAUD3000000 = 3000000,      //POSIX ONLY
    BAUD3500000 = 3500000,      //POSIX ONLY
    BAUD4000000 = 4000000,      //POSIX ONLY
#  endif
#endif //Q_OS_UNIX
#if defined(Q_OS_WIN) || defined(qdoc)
    BAUD14400 = 14400,          //WINDOWS ONLY
    BAUD56000 = 56000,          //WINDOWS ONLY
    BAUD128000 = 128000,        //WINDOWS ONLY
    BAUD256000 = 256000,        //WINDOWS ONLY
#endif  //Q_OS_WIN
    BAUD110 = 110,
    BAUD300 = 300,
    BAUD600 = 600,
    BAUD1200 = 1200,
    BAUD2400 = 2400,
    BAUD4800 = 4800,
    BAUD9600 = 9600,
    BAUD19200 = 19200,
    BAUD38400 = 38400,
    BAUD57600 = 57600,
    BAUD115200 = 115200
};

/**
 * @brief
 *
 * @enum DataBitsType
 */
enum DataBitsType
{
    DATA_5 = 5,
    DATA_6 = 6,
    DATA_7 = 7,
    DATA_8 = 8
};

/**
 * @brief
 *
 * @enum ParityType
 */
enum ParityType
{
    PAR_NONE,
    PAR_ODD,
    PAR_EVEN,
#if defined(Q_OS_WIN) || defined(qdoc)
    PAR_MARK,               //WINDOWS ONLY
#endif
    PAR_SPACE
};

/**
 * @brief
 *
 * @enum StopBitsType
 */
enum StopBitsType
{
    STOP_1,
#if defined(Q_OS_WIN) || defined(qdoc)
    STOP_1_5,               //WINDOWS ONLY
#endif
    STOP_2
};

/**
 * @brief
 *
 * @enum FlowType
 */
enum FlowType
{
    FLOW_OFF,
    FLOW_HARDWARE,
    FLOW_XONXOFF
};

/**
 * structure to contain port settings
 */
/**
 * @brief
 *
 * @class PortSettings qextserialport.h "qextserialport/qextserialport.h"
 */
struct PortSettings
{
    BaudRateType BaudRate; /**< TODO */
    DataBitsType DataBits; /**< TODO */
    ParityType Parity; /**< TODO */
    StopBitsType StopBits; /**< TODO */
    FlowType FlowControl; /**< TODO */
    long Timeout_Millisec; /**< TODO */
};

class QextSerialPortPrivate;
/**
 * @brief
 *
 * @class QextSerialPort qextserialport.h "qextserialport/qextserialport.h"
 */
class QEXTSERIALPORT_EXPORT QextSerialPort: public QIODevice
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QextSerialPort)
    Q_ENUMS(QueryMode)
    Q_PROPERTY(QString portName READ portName WRITE setPortName)
    Q_PROPERTY(QueryMode queryMode READ queryMode WRITE setQueryMode)
public:
    /**
     * @brief
     *
     * @enum QueryMode
     */
    enum QueryMode {
        Polling,
        EventDriven
    };

    /**
     * @brief
     *
     * @fn QextSerialPort
     * @param mode
     * @param parent
     */
    explicit QextSerialPort(QueryMode mode = EventDriven, QObject *parent = 0);
    /**
     * @brief
     *
     * @fn QextSerialPort
     * @param name
     * @param mode
     * @param parent
     */
    explicit QextSerialPort(const QString &name, QueryMode mode = EventDriven, QObject *parent = 0);
    /**
     * @brief
     *
     * @fn QextSerialPort
     * @param s
     * @param mode
     * @param parent
     */
    explicit QextSerialPort(const PortSettings &s, QueryMode mode = EventDriven, QObject *parent = 0);
/**
 * @brief
 *
 * @fn QextSerialPort
 * @param name
 * @param s
 * @param mode
 * @param parent
 */
    QextSerialPort(const QString &name, const PortSettings &s, QueryMode mode = EventDriven, QObject *parent=0);

    /**
     * @brief
     *
     * @fn ~QextSerialPort
     */
    ~QextSerialPort();

    /**
     * @brief
     *
     * @fn portName
     * @return QString
     */
    QString portName() const;
    /**
     * @brief
     *
     * @fn queryMode
     * @return QueryMode
     */
    QueryMode queryMode() const;
    /**
     * @brief
     *
     * @fn baudRate
     * @return BaudRateType
     */
    BaudRateType baudRate() const;
    /**
     * @brief
     *
     * @fn dataBits
     * @return DataBitsType
     */
    DataBitsType dataBits() const;
    /**
     * @brief
     *
     * @fn parity
     * @return ParityType
     */
    ParityType parity() const;
    /**
     * @brief
     *
     * @fn stopBits
     * @return StopBitsType
     */
    StopBitsType stopBits() const;
    /**
     * @brief
     *
     * @fn flowControl
     * @return FlowType
     */
    FlowType flowControl() const;

    /**
     * @brief
     *
     * @fn open
     * @param mode
     * @return bool
     */
    bool open(OpenMode mode);
    /**
     * @brief
     *
     * @fn isSequential
     * @return bool
     */
    bool isSequential() const;
    /**
     * @brief
     *
     * @fn close
     */
    void close();
    /**
     * @brief
     *
     * @fn flush
     */
    void flush();
    /**
     * @brief
     *
     * @fn bytesAvailable
     * @return qint64
     */
    qint64 bytesAvailable() const;
    /**
     * @brief
     *
     * @fn canReadLine
     * @return bool
     */
    bool canReadLine() const;
    /**
     * @brief
     *
     * @fn readAll
     * @return QByteArray
     */
    QByteArray readAll();

    /**
     * @brief
     *
     * @fn lastError
     * @return ulong
     */
    ulong lastError() const;

    /**
     * @brief
     *
     * @fn lineStatus
     * @return ulong
     */
    ulong lineStatus();
    /**
     * @brief
     *
     * @fn errorString
     * @return QString
     */
    QString errorString();

public Q_SLOTS:
    /**
     * @brief
     *
     * @fn setPortName
     * @param name
     */
    void setPortName(const QString &name);
    /**
     * @brief
     *
     * @fn setQueryMode
     * @param mode
     */
    void setQueryMode(QueryMode mode);
    /**
     * @brief
     *
     * @fn setBaudRate
     * @param BaudRateType
     */
    void setBaudRate(BaudRateType);
    /**
     * @brief
     *
     * @fn setDataBits
     * @param DataBitsType
     */
    void setDataBits(DataBitsType);
    /**
     * @brief
     *
     * @fn setParity
     * @param ParityType
     */
    void setParity(ParityType);
    /**
     * @brief
     *
     * @fn setStopBits
     * @param StopBitsType
     */
    void setStopBits(StopBitsType);
    /**
     * @brief
     *
     * @fn setFlowControl
     * @param FlowType
     */
    void setFlowControl(FlowType);
    /**
     * @brief
     *
     * @fn setTimeout
     * @param long
     */
    void setTimeout(long);

    /**
     * @brief
     *
     * @fn setDtr
     * @param set
     */
    void setDtr(bool set=true);
    /**
     * @brief
     *
     * @fn setRts
     * @param set
     */
    void setRts(bool set=true);

Q_SIGNALS:
    /**
     * @brief
     *
     * @fn dsrChanged
     * @param status
     */
    void dsrChanged(bool status);

protected:
    /**
     * @brief
     *
     * @fn readData
     * @param data
     * @param maxSize
     * @return qint64
     */
    qint64 readData(char *data, qint64 maxSize);
    /**
     * @brief
     *
     * @fn writeData
     * @param data
     * @param maxSize
     * @return qint64
     */
    qint64 writeData(const char *data, qint64 maxSize);

private:
    Q_DISABLE_COPY(QextSerialPort)

#ifdef Q_OS_WIN
    Q_PRIVATE_SLOT(d_func(), void _q_onWinEvent(HANDLE))
#endif
    Q_PRIVATE_SLOT(d_func(), void _q_canRead())

    QextSerialPortPrivate *const d_ptr; /**< TODO */
};

#endif
