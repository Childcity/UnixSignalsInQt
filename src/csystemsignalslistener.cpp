#include "csystemsignalslistener.h"

#include <unistd.h>
#include <sys/socket.h>

namespace childcity {


SetupSystemSignalException::~SetupSystemSignalException()
{}


int CSystemSignalsListener::sigHupFd[] = {0};
int CSystemSignalsListener::sigTermFd[] = {0};
int CSystemSignalsListener::sigIntFd[] = {0};
int CSystemSignalsListener::sigSegvFd[] = {0};

int CSystemSignalsListener::SetupUnixSignalHandlers()
{
    struct sigaction hup, term, intr, segv;

    {
        hup.sa_handler = CSystemSignalsListener::HupSignalHandler;
        sigemptyset(&hup.sa_mask);
        hup.sa_flags = 0;
        hup.sa_flags |= SA_RESTART;

        if (sigaction(SIGHUP, &hup, nullptr))
            return 1;
    }

    {
        term.sa_handler = CSystemSignalsListener::TermSignalHandler;
        sigemptyset(&term.sa_mask);
        term.sa_flags |= SA_RESTART;

        if (sigaction(SIGTERM, &term, nullptr))
            return 2;
    }

    {
        intr.sa_handler = CSystemSignalsListener::IntSignalHandler;
        sigemptyset(&intr.sa_mask);
        intr.sa_flags |= SA_RESTART;

        if (sigaction(SIGINT, &intr, nullptr))
            return 3;
    }

    {
        segv.sa_handler = CSystemSignalsListener::SegvSignalHandler;
        sigemptyset(&segv.sa_mask);
        segv.sa_flags |= SA_RESTART;

        if (sigaction(SIGSEGV, &segv, nullptr))
            return 4;
    }

    return 0;
}

void CSystemSignalsListener::HupSignalHandler(int)
{
    char a = 1;
    UNUSED_RET(::write(sigHupFd[0], &a, sizeof(a)));
}

void CSystemSignalsListener::TermSignalHandler(int)
{
    char a = 1;
    UNUSED_RET(::write(sigTermFd[0], &a, sizeof(a)));
}

void CSystemSignalsListener::IntSignalHandler(int)
{
    char a = 1;
    UNUSED_RET(::write(sigIntFd[0], &a, sizeof(a)));
}

void CSystemSignalsListener::SegvSignalHandler(int)
{
    char a = 1;
    UNUSED_RET(::write(sigSegvFd[0], &a, sizeof(a)));
}

CSystemSignalsListener &CSystemSignalsListener::GetInstance()
{
    static CSystemSignalsListener INSTANCE;
    return INSTANCE;
}

CSystemSignalsListener::CSystemSignalsListener()
{
    if(int ret = SetupUnixSignalHandlers()){
        throw SetupSystemSignalException(QString("SetupUnixSignalHandlers returned with: %1").arg(ret));
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigHupFd))
        throw SetupSystemSignalException("Couldn't create HUP socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigTermFd))
        throw SetupSystemSignalException("Couldn't create TERM socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigIntFd))
        throw SetupSystemSignalException("Couldn't create INT socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigSegvFd))
        throw SetupSystemSignalException("Couldn't create SEGV socketpair");

    snHup_ = qt_make_unique<QSocketNotifier>(sigHupFd[1], QSocketNotifier::Read);
    connect(&*snHup_, &QSocketNotifier::activated, this, &CSystemSignalsListener::handleSigHup);

    snTerm_ = qt_make_unique<QSocketNotifier>(sigTermFd[1], QSocketNotifier::Read);
    connect(&*snTerm_, &QSocketNotifier::activated, this, &CSystemSignalsListener::handleSigTerm);

    snInt_ = qt_make_unique<QSocketNotifier>(sigIntFd[1], QSocketNotifier::Read);
    connect(&*snInt_, &QSocketNotifier::activated, this, &CSystemSignalsListener::handleSigInt);

    snSegv_ = qt_make_unique<QSocketNotifier>(sigSegvFd[1], QSocketNotifier::Read);
    connect(&*snSegv_, &QSocketNotifier::activated, this, &CSystemSignalsListener::handleSigSegv);
}

void CSystemSignalsListener::handleSigHup()
{
    snHup_->setEnabled(false);

    char tmp;
    UNUSED_RET(::read(sigHupFd[1], &tmp, sizeof(tmp)));

    emit sigSystemSignal(SysSig::SigHup);

    snHup_->setEnabled(true);
}

void CSystemSignalsListener::handleSigTerm()
{
    snTerm_->setEnabled(false);

    char tmp;
    UNUSED_RET(::read(sigTermFd[1], &tmp, sizeof(tmp)));

    emit sigSystemSignal(SysSig::SigTerm);

    snTerm_->setEnabled(true);
}

void CSystemSignalsListener::handleSigInt()
{
    snInt_->setEnabled(false);

    char tmp;
    UNUSED_RET(::read(sigIntFd[1], &tmp, sizeof(tmp)));

    emit sigSystemSignal(SysSig::SigInt);

    snInt_->setEnabled(true);
}

void CSystemSignalsListener::handleSigSegv()
{
    snSegv_->setEnabled(false);

    char tmp;
    UNUSED_RET(::read(sigSegvFd[1], &tmp, sizeof(tmp)));

    emit sigSystemSignal(SysSig::SigSegv);

    snSegv_->setEnabled(true);
}


}
