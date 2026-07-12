; ModuleID = 'fusion.cpp'
source_filename = "fusion.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local noundef i32 @_Z6Fusionii(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  store i32 0, ptr %5, align 4
  %12 = load i32, ptr %3, align 4
  %13 = load i32, ptr %4, align 4
  %14 = icmp sgt i32 %12, %13
  br i1 %14, label %15, label %28

15:                                               ; preds = %2
  store i32 0, ptr %6, align 4
  br label %16

16:                                               ; preds = %24, %15
  %17 = load i32, ptr %6, align 4
  %18 = load i32, ptr %3, align 4
  %19 = icmp slt i32 %17, %18
  br i1 %19, label %20, label %27

20:                                               ; preds = %16
  %21 = load i32, ptr %5, align 4
  %22 = load i32, ptr %4, align 4
  %23 = add nsw i32 %21, %22
  store i32 %23, ptr %5, align 4
  br label %24

24:                                               ; preds = %20
  %25 = load i32, ptr %6, align 4
  %26 = add nsw i32 %25, 1
  store i32 %26, ptr %6, align 4
  br label %16, !llvm.loop !6

27:                                               ; preds = %16
  br label %41

28:                                               ; preds = %2
  store i32 0, ptr %7, align 4
  br label %29

29:                                               ; preds = %37, %28
  %30 = load i32, ptr %7, align 4
  %31 = load i32, ptr %4, align 4
  %32 = icmp slt i32 %30, %31
  br i1 %32, label %33, label %40

33:                                               ; preds = %29
  %34 = load i32, ptr %5, align 4
  %35 = load i32, ptr %3, align 4
  %36 = add nsw i32 %34, %35
  store i32 %36, ptr %5, align 4
  br label %37

37:                                               ; preds = %33
  %38 = load i32, ptr %7, align 4
  %39 = add nsw i32 %38, 1
  store i32 %39, ptr %7, align 4
  br label %29, !llvm.loop !8

40:                                               ; preds = %29
  br label %41

41:                                               ; preds = %40, %27
  store i32 0, ptr %8, align 4
  store i32 0, ptr %9, align 4
  store i32 0, ptr %10, align 4
  br label %42

42:                                               ; preds = %49, %41
  %43 = load i32, ptr %10, align 4
  %44 = icmp slt i32 %43, 10
  br i1 %44, label %45, label %52

45:                                               ; preds = %42
  %46 = load i32, ptr %3, align 4
  %47 = load i32, ptr %4, align 4
  %48 = add nsw i32 %46, %47
  store i32 %48, ptr %8, align 4
  br label %49

49:                                               ; preds = %45
  %50 = load i32, ptr %10, align 4
  %51 = add nsw i32 %50, 1
  store i32 %51, ptr %10, align 4
  br label %42, !llvm.loop !9

52:                                               ; preds = %42
  store i32 0, ptr %11, align 4
  br label %53

53:                                               ; preds = %60, %52
  %54 = load i32, ptr %11, align 4
  %55 = icmp slt i32 %54, 10
  br i1 %55, label %56, label %63

56:                                               ; preds = %53
  %57 = load i32, ptr %3, align 4
  %58 = load i32, ptr %4, align 4
  %59 = mul nsw i32 %57, %58
  store i32 %59, ptr %9, align 4
  br label %60

60:                                               ; preds = %56
  %61 = load i32, ptr %11, align 4
  %62 = add nsw i32 %61, 1
  store i32 %62, ptr %11, align 4
  br label %53, !llvm.loop !10

63:                                               ; preds = %53
  %64 = load i32, ptr %5, align 4
  ret i32 %64
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
!10 = distinct !{!10, !7}
