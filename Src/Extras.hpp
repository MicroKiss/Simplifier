#pragma once
#include <functional>

// measure time of function f in milliseconds
long long TimeIt (std::function<void ()> const &f);