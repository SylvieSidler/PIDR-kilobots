#include "namingGame.h"

// select 1 random word from globalWordList and put it into personalWordList
void generateWord(void) {
    randn = rand() % N;
    mydata->personalWord = globalWordList[randn];
}

// create a link between the object and the word
void generateLink(uint8_t object, uint8_t word) {
    link_obj_word link;
    link.object = object;
    link.word = word;
    link.boolean = true;
    mydata->links[0] = link;
}

// deletes all links to the object except the word in parameters 

