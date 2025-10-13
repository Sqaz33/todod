#pragma once 

#include <memory>
#include <set>

#include "iobserver.hpp"

namespace subject {

struct ISubject {
public:
    virtual void attach(
        std::shared_ptr<observer::IObserver> obs) = 0;
    virtual void dettach(
        std::shared_ptr<observer::IObserver> obs) = 0;

public:
    virtual ~ISubject() = default;

protected:
    virtual void update(int newState) = 0;

    std::set<
        std::weak_ptr<observer::IObserver>,
        std::owner_less<std::weak_ptr<observer::IObserver>>> obs_;
};

void ISubject::attach(
        std::shared_ptr<observer::IObserver> obs) 
{ obs_.insert(obs); }

void ISubject::dettach(
    std::shared_ptr<observer::IObserver> obs)
{ obs_.erase(obs); }

void ISubject::update(int newState) {
    for (auto it = obs_.begin(); it != obs_.end();) {
        if (it->expired()) {
            it = obs_.erase(it);
        } else {
            auto shared = it->lock();
            shared->update(newState);
        }
    };
}

} //  namespace subject