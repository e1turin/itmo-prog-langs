%include "colon.inc"

colon "'", not_empty_word
db "not empty word", 0

colon "", empty_word
db "empty word", 0

colon "4th word", fourth_word
db "fourth word entry", 0

colon "3rd word", third_word
db "third word entry", 0

colon "2nd word", second_word
db "second word entry", 0 

colon "1st word", first_word
db "first word entry", 0 
