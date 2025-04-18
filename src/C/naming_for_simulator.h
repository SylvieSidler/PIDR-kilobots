enum state{
    LISTENER,
    SPEAKER,
    RIEN
};

typedef struct {
    uint8_t object;
    uint8_t word;
    uint8_t link;
} link_obj_word_t;

#define Nb_diff_obj 1
#define Nb_max_diff_words_for_objs 100
