#include "script.hpp"

namespace todod {

const std::set<TodoEvent> ALL_EVENTS {
    #define X(event) TodoEvent::event,
        TODO_EVENTS(X)
    #undef X
};

} // namespace todod
