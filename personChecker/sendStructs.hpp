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

// Person checker
typedef struct messagePerson
{
    int isPersonInside;
};

#endif