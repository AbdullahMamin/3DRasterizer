#ifndef UTIL_H
#define UTIL_H

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(x, low, high) (min(max((x), (low)), (high)))

#endif // UTIL_H
