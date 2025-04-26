#ifndef NAMINGGAME_H
#define NAMINGGAME_H

#include <kilombo.h>

#define N 5 // size of globalWordList -> number of words in universe 
#define OBJECT 7 // object to name

uint8_t globalWordList[] = {0, 1, 2, 3, 4}; // list of words in universe

// state of kilobot : LISTENER or SPEAKER
enum state{
    LISTENER,
    SPEAKER
};

// define a link betwee n an object and a word and its status : active or not
typedef struct {
    uint8_t object; // object to name 
    uint8_t word; // word to name the object
    bool boolean; // bool -> link is active or not
} link_obj_word;

#define TS 8 // Time in sec. for which a kilobot stays in the same state
#define SEC 32 // 1 second in kiloticks

// dictionnary of colors : 
// 0 - off
// 1 - red
// 2 - orange
// 3 - yellow
// 4 - yellowish green
// 5 - green
// 6 - cyan
// 7 - blue
// 8 - purple
// 9 - bright white
uint8_t colors[] = {
    RGB(0,0,0),  
    RGB(2,0,0),  
    RGB(2,1,0),  
    RGB(2,2,0),  
    RGB(1,2,0),  
    RGB(0,2,0),  
    RGB(0,1,1),  
    RGB(0,0,2),  
    RGB(1,0,1),   
    RGB(3,3,3)   
}; 


//////// FUNCTIONS ///////////

void generateWord(void); 
void generateLink(uint8_t object, uint8_t word);
void deleteLinksExceptWord(uint8_t object, uint8_t keepWord);

#endif // NAMINGGAME_H
