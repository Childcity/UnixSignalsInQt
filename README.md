# UnixSignalsInQt
Simple UNIX Signals catcher in Qt

If you would like to write any program in Qt/C++, you must:
- take care of the correct exit from [Qt Event Loop](https://wiki.qt.io/Threads_Events_QObjects);
- correct release all allocated memory;

If user press Ctrl+C in Terminal or send SIGTERM, you're program will be closed unexpectedly with error code.

This realization based on offisial [Qt documentation](https://doc.qt.io/qt-5/unix-signals.html) for solving such kind of problem. 
Try it and you will see, this is convenient!

# Little Example of using it (full in ./example/main.cpp)
- ## Header files for this example
	```cpp
		#include "../src/csystemsignalslistener.h"

		#include <QCoreApplication>
		#include <QTimer>
		#include <QDebug>
	```

- ## Function, that do all stuff with System Signals
	```cpp
		void connectSystemSignals(const QCoreApplication &a)
		{	
			using namespace childcity;
			using SysSig = CSystemSignalsListener::SysSig;

			try {
				// This will instantiate Singleton object in static memory
				CSystemSignalsListener::GetInstance();
			} catch (SetupSystemSignalException e) {
				// Do nothing. You can exit(ERROR_CODE) or do other things
			}

			// Connect system signals to method, which will be emited after system signal raised
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
	```

- ## Main function
	```cpp
		int main(int argc, char *argv[])
		{
			QCoreApplication a(argc, argv);
			
			connectSystemSignals(a); 

			// YOURE PRETTY COOL CODE IS ----> HERE <----- :)

			return a.exec();
		}
	```

### That's all you need ;)
