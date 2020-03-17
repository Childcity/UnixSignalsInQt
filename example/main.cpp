#include "../src/csystemsignalslistener.h"

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

// Will quit this thread Qt EventLoop and safely call all destrucors
// after catching SIGINT, SIGTERM, SIGSEGV from system
void connectSystemSignals(const QCoreApplication &a)
{
    using namespace childcity;
    using SysSig = CSystemSignalsListener::SysSig;

    try {
        CSystemSignalsListener::GetInstance();
    } catch (SetupSystemSignalException e) {
        // Do nothing. My app should continue working.
    }

    QObject::connect(&CSystemSignalsListener::GetInstance(), &CSystemSignalsListener::sigSystemSignal, &a, [&a](SysSig sig){
        qInfo() << "Cought system signal: " << sig;
        switch (sig) {
            case SysSig::SigHup:
                // Do nothing on HUB.
                return;
            case SysSig::SigTerm:
                [[fallthrough]];
            case SysSig::SigInt:
                a.quit();
                break;
            case SysSig::SigSegv:
                // Obviously, without next 3 lines in the code, there would not be core file.
                const auto pid = static_cast<pid_t>(QCoreApplication::applicationPid());
                signal(SIGSEGV, nullptr); // nullptr == SIG_DFL
                kill(pid, SIGSEGV);
                break;
        }
    }, Qt::ConnectionType::DirectConnection); // Should be always Qt::ConnectionType::DirectConnection !!!
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    connectSystemSignals(a);

    // Rise SIGTERM for test purpose
    QTimer::singleShot(0, &a, [&]{ raise(SIGTERM); });

    return a.exec();
}

// Result after run of this program
/*
 *
 * Running... ~/TestSystemSignals/build_Shared-Profile/TestSystemSignals...
 * Cought system signal:  childcity::CSystemSignalsListener::SysSig(SigTerm)
 * ~/TestSystemSignals/build_Shared-Profile/TestSystemSignals exit with code 0
 *
 */
