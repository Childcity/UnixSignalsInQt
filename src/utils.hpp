#ifndef UTILS_HPP
#define UTILS_HPP

#include <QObject>
#include <memory>

namespace childcity {

template <class T>
void UNUSED_RET(T funk){
    const auto unused = (funk);
    (void) unused;
}

struct QObjectDeleteLater {
    void operator()(QObject *o) {
        o->deleteLater();
    }
};

template<typename T>
using qt_unique_ptr = std::unique_ptr<T, QObjectDeleteLater>;

template<class T, class... _Types, std::enable_if_t<! std::is_array<T>::value, int> = 0>
inline qt_unique_ptr<T> qt_make_unique(_Types&&... _Args){
    return (qt_unique_ptr<T>(new T(std::forward<_Types>(_Args)...)));
}

}

#endif // UTILS_HPP
