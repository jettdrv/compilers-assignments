; int fun(int a, int b){
;   c = a * 15;
;   d = b / 8;
;   sum = c + d;
;   return sum;
; }
; 

define dso_local i32 @fun (i32 noundef %0, i32 noundef %1) {
    %3 = mul nsw i32 17, %0
    %4 = udiv i32 %1, 8
    %5 = add nsw i32 %3, %4 
    ret i32 %5
}