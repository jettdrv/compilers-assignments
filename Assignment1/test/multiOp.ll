; a = b + 1
; c = a - 1

define dso_local i32 @fun(i32 noundef %0){
    %2 = add nsw i32 %0, 1
    %3 = sub nsw i32 %2, 1
    %4 = add nsw i32 %0, %3
    ret i32 %4
}





