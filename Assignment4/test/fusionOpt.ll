; ModuleID = '../test/fusion.m2r.ll'
source_filename = "fusion.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local noundef i32 @_Z6Fusionii(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp sgt i32 %0, %1
  br i1 %3, label %4, label %12

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %9, %4
  %.03 = phi i32 [ 0, %4 ], [ %10, %9 ]
  %.02 = phi i32 [ 0, %4 ], [ %8, %9 ]
  %6 = icmp slt i32 %.03, %0
  br i1 %6, label %7, label %11

7:                                                ; preds = %5
  %8 = add nsw i32 %.02, %1
  br label %9

9:                                                ; preds = %7
  %10 = add nsw i32 %.03, 1
  br label %5, !llvm.loop !6

11:                                               ; preds = %5
  br label %20

12:                                               ; preds = %2
  br label %13

13:                                               ; preds = %17, %12
  %.04 = phi i32 [ 0, %12 ], [ %18, %17 ]
  %.1 = phi i32 [ 0, %12 ], [ %16, %17 ]
  %14 = icmp slt i32 %.04, %1
  br i1 %14, label %15, label %19

15:                                               ; preds = %13
  %16 = add nsw i32 %.1, %0
  br label %17

17:                                               ; preds = %15
  %18 = add nsw i32 %.04, 1
  br label %13, !llvm.loop !8

19:                                               ; preds = %13
  br label %20

20:                                               ; preds = %19, %11
  %.2 = phi i32 [ %.02, %11 ], [ %.1, %19 ]
  br label %21

21:                                               ; preds = %25, %20
  %.01 = phi i32 [ 0, %20 ], [ %26, %25 ]
  %22 = icmp slt i32 %.01, 10
  br i1 %22, label %23, label %29

23:                                               ; preds = %21
  %24 = add nsw i32 %0, %1
  br label %27

25:                                               ; preds = %27
  %26 = add nsw i32 %.01, 1
  br label %21, !llvm.loop !9

27:                                               ; preds = %23
  %28 = mul nsw i32 %0, %1
  br label %25

29:                                               ; preds = %21
  ret i32 %.2
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
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
