; int fun(int a, int b){
;   c = a + 0;
;   d = b * 1;
;   sum = c + d;
;   return sum;
; }
; 

define dso_local i32 @fun (i32 noundef %0, i32 noundef %1) {
    %3 = add nsw i32 %0, 0
    %4 = mul nsw i32 %1, 1
    %5 = add nsw i32 %3, %4 
    ret i32 %5
}




