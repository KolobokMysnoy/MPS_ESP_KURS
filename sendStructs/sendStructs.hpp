#ifndef STRUCTS
#define STRUCTS

// LuxChecker
typedef struct messageLux
{
    int insideLux;
    int outsideLux;
};

// LedCntrl
typedef struct messageLed
{
    int procent;
};

// Driver
typedef struct messageDriver
{
    int procent;
};

#endif