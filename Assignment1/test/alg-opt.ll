; ModuleID = 'a'
source_filename = "test/algebraicIdentity.ll"

define dso_local i32 @fun(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %0, %1
  ret i32 %3
}
