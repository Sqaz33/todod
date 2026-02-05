#pragma once

namespace observer {

struct IObserver {
    virtual void update(int newState) = 0;
    virtual ~IObserver() = default;
};

}  // namespace observer