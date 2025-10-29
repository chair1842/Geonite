#pragma once

template<typename T>
T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}
// This function performs linear interpolation between two values a and b based on the parameter t.