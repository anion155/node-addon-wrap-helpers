#pragma once

namespace nawh {
template <typename ...Args>
void UNUSED(Args&& ...args) {
    (void)(sizeof...(args));
}
}
