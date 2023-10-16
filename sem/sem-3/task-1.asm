%macro test 2
db %1
db ","
db %2
%endmacro

test "hello",  " world"
