#pragma once

#include "observer.hpp"

namespace repository {

class Repository : public observer::IObserver {
public:
    Repository(...);

public:
    void update(int newState) override;

    
};

} // namespace repository