%macro colon 2; %1: "key" string, %2: label name 
    %ifid %2 
        %ifndef COLON_BEGIN
           %%next_entry: dq 0

        %else 
            %%next_entry: dq COLON_BEGIN
            
        %endif

        %define COLON_BEGIN %%next_entry
    
    %else 
        %fatal "Second argument (id) of macro `colon` must be label name."

    %endif

    %ifstr %1 
       db %1, 0
       %2:; possible entry value

    %else 
        %fatal "First argument (entry key) of macro `colon` must be string."

    %endif

%endmacro
