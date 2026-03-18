#pragma once

namespace event {

enum class event_t : int {
    USER_ASK_ADD_TODO = 0,
    USER_ASK_ALL_TODOS,
    USER_ASK_CONCRETE_TODOS,
    USER_ASK_CHANGE_TODO,
    USER_ASK_DELETE_TODO,
    ERROR
};

}  // namespace event