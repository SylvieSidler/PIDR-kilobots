#include "namingGame.h"

// select 1 random word from globalWordList and put it into personalWordList
void generateWord(void) {
    randn = rand() % N;
    mydata->personalWord = globalWordList[randn];
}

// create a link between the object and the word in `mydata->links`
void generateLink(uint8_t object, uint8_t word) {
    link_obj_word link;
    link.object = object;
    link.word = word;
    link.boolean = true;
    mydata->links[0] = link;
}

// deletes all links to the object except the word in parameters 
void deleteLinksExceptWord(uint8_t object, uint8_t keepWord) {
    for (uint8_t i = 0; i < N; i++) {
        if (mydata->links[i].word != keepWord) { // && mydata->links[i].object == object // si plusieurs objets
            mydata->links[i].boolean = false;
        }
    }
}



