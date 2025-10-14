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

} //  namespace subject