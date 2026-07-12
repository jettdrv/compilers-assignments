; ModuleID = 'algebraicidentityopt'
source_filename = "test/algebraicIdentity.ll"

define dso_local i32 @fun(i32 noundef %0, i32 noundef %1) {
  %3 = mul nsw i32 %1, 1
  %4 = add nsw i32 %0, %3
  ret i32 %4
}
