; ModuleID = 'LICM.ll'
source_filename = "LICM.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local noundef i32 @_Z4LICMv() #0 {
  br label %1

1:                                                ; preds = %11, %0
  %.01 = phi i32 [ 20, %0 ], [ %10, %11 ]
  %2 = add nsw i32 5, 10
  %3 = srem i32 %.01, 2
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %7

6:                                                ; preds = %1
  br label %7

7:                                                ; preds = %6, %5
  %.0 = phi i32 [ 2, %5 ], [ 3, %6 ]
  %8 = add nsw i32 %2, 1
  %9 = add nsw i32 %.0, 2
  %10 = sub nsw i32 %.01, 1
  br label %11

11:                                               ; preds = %7
  %12 = icmp sge i32 %10, 0
  br i1 %12, label %1, label %13, !llvm.loop !6

13:                                               ; preds = %11
  ret i32 5
}

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Debian clang version 19.1.7 (22)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
