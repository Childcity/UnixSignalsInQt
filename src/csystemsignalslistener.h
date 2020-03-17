/*
 * System Signal Listener
 * version 1.0.0
 * https://github.com/Childcity/UnixSignalsInQt
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2020 Childcity
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CSYSTEMSIGNALSLISTENER_H
#define CSYSTEMSIGNALSLISTENER_H

#include "utils.hpp"

#include <QSocketNotifier>
#include <exception>
#include <signal.h>

namespace childcity {


struct SetupSystemSignalException: public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    SetupSystemSignalException(const QString str)
        : std::runtime_error(std::move(str.toStdString()))
    {}

    SetupSystemSignalException(const SetupSystemSignalException &) = default;

    ~SetupSystemSignalException() override;
};


class CSystemSignalsListener: public QObject {
    Q_OBJECT

private:
    static int sigHupFd[2];
    static int sigTermFd[2];
    static int sigIntFd[2];
    static int sigSegvFd[2];

    qt_unique_ptr<QSocketNotifier> snHup_;
    qt_unique_ptr<QSocketNotifier> snTerm_;
    qt_unique_ptr<QSocketNotifier> snInt_;
    qt_unique_ptr<QSocketNotifier> snSegv_;

private:

    static int SetupUnixSignalHandlers();

    // Unix signal handlers.
    static void HupSignalHandler(int);
    static void TermSignalHandler(int);
    static void IntSignalHandler(int);
    static void SegvSignalHandler(int);

public:

    enum class SysSig {
        SigHup = SIGHUP,
        SigTerm = SIGTERM,
        SigInt = SIGINT,
        SigSegv = SIGSEGV
    };
    Q_ENUM(SysSig)

    static CSystemSignalsListener& GetInstance();

    CSystemSignalsListener(const CSystemSignalsListener&) = delete;
    CSystemSignalsListener(CSystemSignalsListener&&) = delete;
    CSystemSignalsListener& operator=(const CSystemSignalsListener&) = delete;
    CSystemSignalsListener& operator=(CSystemSignalsListener&&) = delete;

signals:
    void sigSystemSignal(SysSig sig);

private:
    explicit CSystemSignalsListener();

private slots:

    // Qt signal handlers.
    void handleSigHup();
    void handleSigTerm();
    void handleSigInt();
    void handleSigSegv();
};


}

#endif // CSYSTEMSIGNALSLISTENER_H
