#ifndef __MNG_H__
#define __MNG_H__

#include "../../../include/kilolib.h"
#define NO 10 // # of agent playing as objects
#define NA 25 // # of active agents
#define ND 1 // # of different objects
#define MM 100 // # max amount of words associated per object
#define PS 20 // Probability (%) of an agent to become a speaker for TS sec, tested every TS sec.  (= 1 iteraion)
#define SEC 32 // 1 second in kiloticks
#define TS 8 // Time in sec. for which a robot stays a speaker
#define MAXT 500 // Number of iterations before stopping the experiment
#define PRINT 0 // Define wether to print things or not
#define MOVEMENT 1 // Shall the robot move 

// Note that ND^2 * MM must be inferior or equal to 255, otherwise memory issues could arise

typedef struct {
    uint8_t object_id; 
    uint8_t word_id; 
    uint8_t active; // Is this relation active or not
} relation_t;

enum MNG_STATE{
    OBJECT = 0,
    SPEAKER,
    HEARER
};

enum MOV_STATE{
    STRAIGHT = 0,
    TURNING
};

#endif//__MNG_H__
