#ifndef MATH_DEFINES_H
#define MATH_DEFINES_H

#define PI32 3.14159265359f

#ifndef MIN
#define MIN(a, b) (a) > (b) ? (b) : (a)
#endif
#ifndef MAX
#define MAX(a, b) (a) < (b) ? (b) : (a)
#endif
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define MOD(a, m) ((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m))
#define SQUARE(x) ((x) * (x))

#endif