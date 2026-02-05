#include "isubject.hpp"

namespace subject {

void ISubject::attach(std::shared_ptr<observer::IObserver> obs) {
    obs_.insert(obs);
}

void ISubject::dettach(std::shared_ptr<observer::IObserver> obs) {
    obs_.erase(obs);
}

void ISubject::notify(int newState) {
    for (auto it = obs_.begin(); it != obs_.end();) {
        if (it->expired()) {
            it = obs_.erase(it);
        } else {
            auto shared = it->lock();
            shared->update(newState);
            ++it;
        }
    };
}

}  // namespace subject