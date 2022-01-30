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

#ifndef _QEXTSERIALPORT_P_H_
#define _QEXTSERIALPORT_P_H_

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QESP API.  It exists for the convenience
// of other QESP classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qextserialport.h"
#include <QtCore/QReadWriteLock>
#ifdef Q_OS_UNIX
#  include <termios.h>
#elif (defined Q_OS_WIN)
#  include <QtCore/qt_windows.h>
#endif
#include <stdlib.h>

// This is QextSerialPort's read buffer, needed by posix system.
// ref: QRingBuffer & QIODevicePrivateLinearBuffer
/**
 * @brief
 *
 * @class QextReadBuffer qextserialport_p.h "qextserialport/qextserialport_p.h"
 */
class QextReadBuffer
{
public:
    /**
     * @brief
     *
     * @fn QextReadBuffer
     * @param growth
     */
    inline QextReadBuffer(size_t growth=4096)
        : len(0), first(0), buf(0), capacity(0), basicBlockSize(growth) {
    }

    /**
     * @brief
     *
     * @fn ~QextReadBuffer
     */
    ~QextReadBuffer() {
        delete [] buf;
    }

    /**
     * @brief
     *
     * @fn clear
     */
    inline void clear() {
        first = buf;
        len = 0;
    }

    /**
     * @brief
     *
     * @fn size
     * @return int
     */
    inline int size() const {
        return len;
    }

    /**
     * @brief
     *
     * @fn isEmpty
     * @return bool
     */
    inline bool isEmpty() const {
        return len == 0;
    }

    /**
     * @brief
     *
     * @fn read
     * @param target
     * @param size
     * @return int
     */
    inline int read(char *target, int size) {
        int r = qMin(size, len);
        if (r == 1) {
            *target = *first;
            --len;
            ++first;
        } else {
            memcpy(target, first, r);
            len -= r;
            first += r;
        }
        return r;
    }

    /**
     * @brief
     *
     * @fn reserve
     * @param size
     * @return char
     */
    inline char *reserve(size_t size) {
        if ((first - buf) + len + size > capacity) {
            size_t newCapacity = qMax(capacity, basicBlockSize);
            while (newCapacity < len + size)
                newCapacity *= 2;
            if (newCapacity > capacity) {
                // allocate more space
                char *newBuf = new char[newCapacity];
                memmove(newBuf, first, len);
                delete [] buf;
                buf = newBuf;
                capacity = newCapacity;
            } else {
                // shift any existing data to make space
                memmove(buf, first, len);
            }
            first = buf;
        }
        char *writePtr = first + len;
        len += (int)size;
        return writePtr;
    }

    /**
     * @brief
     *
     * @fn chop
     * @param size
     */
    inline void chop(int size) {
        if (size >= len) {
            clear();
        } else {
            len -= size;
        }
    }

    /**
     * @brief
     *
     * @fn squeeze
     */
    inline void squeeze() {
        if (first != buf) {
            memmove(buf, first, len);
            first = buf;
        }
        size_t newCapacity = basicBlockSize;
        while (newCapacity < size_t(len))
            newCapacity *= 2;
        if (newCapacity < capacity) {
            char *tmp = static_cast<char *>(realloc(buf, newCapacity));
            if (tmp) {
                buf = tmp;
                capacity = newCapacity;
            }
        }
    }

    /**
     * @brief
     *
     * @fn readAll
     * @return QByteArray
     */
    inline QByteArray readAll() {
        char *f = first;
        int l = len;
        clear();
        return QByteArray(f, l);
    }

    /**
     * @brief
     *
     * @fn readLine
     * @param target
     * @param size
     * @return int
     */
    inline int readLine(char *target, int size) {
        int r = qMin(size, len);
        char *eol = static_cast<char *>(memchr(first, '\n', r));
        if (eol)
            r = 1+(eol-first);
        memcpy(target, first, r);
        len -= r;
        first += r;
        return int(r);
    }

    /**
     * @brief
     *
     * @fn canReadLine
     * @return bool
     */
    inline bool canReadLine() const {
        return memchr(first, '\n', len);
    }

private:
    int len; /**< TODO */
    char *first; /**< TODO */
    char *buf; /**< TODO */
    size_t capacity; /**< TODO */
    size_t basicBlockSize; /**< TODO */
};

class QWinEventNotifier;
class QReadWriteLock;
class QSocketNotifier;

/**
 * @brief
 *
 * @class QextSerialPortPrivate qextserialport_p.h "qextserialport/qextserialport_p.h"
 */
class QextSerialPortPrivate
{
    Q_DECLARE_PUBLIC(QextSerialPort)
public:
/**
 * @brief
 *
 * @fn QextSerialPortPrivate
 * @param q
 */
    QextSerialPortPrivate(QextSerialPort *q);
    /**
     * @brief
     *
     * @fn ~QextSerialPortPrivate
     */
    ~QextSerialPortPrivate();
    /**
     * @brief
     *
     * @enum DirtyFlagEnum
     */
    enum DirtyFlagEnum
    {
        DFE_BaudRate = 0x0001,
        DFE_Parity = 0x0002,
        DFE_StopBits = 0x0004,
        DFE_DataBits = 0x0008,
        DFE_Flow = 0x0010,
        DFE_TimeOut = 0x0100,
        DFE_ALL = 0x0fff,
        DFE_Settings_Mask = 0x00ff //without TimeOut
    };
    mutable QReadWriteLock lock; /**< TODO */
    QString port; /**< TODO */
    PortSettings settings; /**< TODO */
    QextReadBuffer readBuffer; /**< TODO */
    int settingsDirtyFlags; /**< TODO */
    ulong lastErr; /**< TODO */
    QextSerialPort::QueryMode queryMode; /**< TODO */

    // platform specific members
#ifdef Q_OS_UNIX
    int fd; /**< TODO */
    QSocketNotifier *readNotifier; /**< TODO */
    struct termios currentTermios; /**< TODO */
    struct termios oldTermios; /**< TODO */
#elif (defined Q_OS_WIN)
    HANDLE handle;
    OVERLAPPED overlap;
    COMMCONFIG commConfig;
    COMMTIMEOUTS commTimeouts;
    QWinEventNotifier *winEventNotifier;
    DWORD eventMask;
    QList<OVERLAPPED *> pendingWrites;
    QReadWriteLock *bytesToWriteLock;
#endif

    /*fill PortSettings*/
    /**
     * @brief
     *
     * @fn setBaudRate
     * @param baudRate
     * @param update
     */
    void setBaudRate(BaudRateType baudRate, bool update=true);
    /**
     * @brief
     *
     * @fn setDataBits
     * @param dataBits
     * @param update
     */
    void setDataBits(DataBitsType dataBits, bool update=true);
    /**
     * @brief
     *
     * @fn setParity
     * @param parity
     * @param update
     */
    void setParity(ParityType parity, bool update=true);
    /**
     * @brief
     *
     * @fn setStopBits
     * @param stopbits
     * @param update
     */
    void setStopBits(StopBitsType stopbits, bool update=true);
    /**
     * @brief
     *
     * @fn setFlowControl
     * @param flow
     * @param update
     */
    void setFlowControl(FlowType flow, bool update=true);
    /**
     * @brief
     *
     * @fn setTimeout
     * @param millisec
     * @param update
     */
    void setTimeout(long millisec, bool update=true);
    /**
     * @brief
     *
     * @fn setPortSettings
     * @param settings
     * @param update
     */
    void setPortSettings(const PortSettings &settings, bool update=true);

    /**
     * @brief
     *
     * @fn platformSpecificDestruct
     */
    void platformSpecificDestruct();
    /**
     * @brief
     *
     * @fn platformSpecificInit
     */
    void platformSpecificInit();
    /**
     * @brief
     *
     * @fn translateError
     * @param error
     */
    void translateError(ulong error);
    /**
     * @brief
     *
     * @fn updatePortSettings
     */
    void updatePortSettings();

    /**
     * @brief
     *
     * @fn readData_sys
     * @param data
     * @param maxSize
     * @return qint64
     */
    qint64 readData_sys(char *data, qint64 maxSize);
    /**
     * @brief
     *
     * @fn writeData_sys
     * @param data
     * @param maxSize
     * @return qint64
     */
    qint64 writeData_sys(const char *data, qint64 maxSize);
    /**
     * @brief
     *
     * @fn setDtr_sys
     * @param set
     */
    void setDtr_sys(bool set=true);
    /**
     * @brief
     *
     * @fn setRts_sys
     * @param set
     */
    void setRts_sys(bool set=true);
    /**
     * @brief
     *
     * @fn open_sys
     * @param mode
     * @return bool
     */
    bool open_sys(QIODevice::OpenMode mode);
    /**
     * @brief
     *
     * @fn close_sys
     * @return bool
     */
    bool close_sys();
    /**
     * @brief
     *
     * @fn flush_sys
     * @return bool
     */
    bool flush_sys();
    /**
     * @brief
     *
     * @fn lineStatus_sys
     * @return ulong
     */
    ulong lineStatus_sys();
    /**
     * @brief
     *
     * @fn bytesAvailable_sys
     * @return qint64
     */
    qint64 bytesAvailable_sys() const;

#ifdef Q_OS_WIN
    void _q_onWinEvent(HANDLE h);
#endif
    /**
     * @brief
     *
     * @fn _q_canRead
     */
    void _q_canRead();

    QextSerialPort *q_ptr; /**< TODO */
};

#endif //_QEXTSERIALPORT_P_H_
