; RUN: opt -inline < %s -S -o - -inline-threshold=8 | FileCheck %s

declare void @llvm.lifetime.start(i64 %size, i8* nocapture %ptr)

@glbl = external global i32

define void @outer1() {
; CHECK: @outer1
; CHECK-NOT: call void @inner1
  %ptr = alloca i32
  call void @inner1(i32* %ptr)
  ret void
}

define void @inner1(i32 *%ptr) {
  %A = load i32* %ptr
  store i32 0, i32* %ptr
  %C = getelementptr i32* %ptr, i32 0
  %D = getelementptr i32* %ptr, i32 1
  %E = bitcast i32* %ptr to i8*
  %F = select i1 false, i32* %ptr, i32* @glbl
  call void @llvm.lifetime.start(i64 0, i8* %E)
  ret void
}

define void @outer2() {
; CHECK: @outer2
; CHECK: call void @inner2
  %ptr = alloca i32
  call void @inner2(i32* %ptr)
  ret void
}

; %D poisons this call, scalar-repl can't handle that instruction.
define void @inner2(i32 *%ptr) {
  %A = load i32* %ptr
  store i32 0, i32* %ptr
  %C = getelementptr i32* %ptr, i32 0
  %D = getelementptr i32* %ptr, i32 %A
  %E = bitcast i32* %ptr to i8*
  %F = select i1 false, i32* %ptr, i32* @glbl
  call void @llvm.lifetime.start(i64 0, i8* %E)
  ret void
}

define void @outer3() {
; CHECK: @outer3
; CHECK-NOT: call void @inner3
  %ptr = alloca i32
  call void @inner3(i32* %ptr, i1 undef)
  ret void
}

define void @inner3(i32 *%ptr, i1 %x) {
  %A = icmp eq i32* %ptr, null
  %B = and i1 %x, %A
  br i1 %A, label %bb.true, label %bb.false
bb.true:
  ; This block musn't be counted in the inline cost.
  %t1 = load i32* %ptr
  %t2 = add i32 %t1, 1
  %t3 = add i32 %t2, 1
  %t4 = add i32 %t3, 1
  %t5 = add i32 %t4, 1
  %t6 = add i32 %t5, 1
  %t7 = add i32 %t6, 1
  %t8 = add i32 %t7, 1
  %t9 = add i32 %t8, 1
  %t10 = add i32 %t9, 1
  %t11 = add i32 %t10, 1
  %t12 = add i32 %t11, 1
  %t13 = add i32 %t12, 1
  %t14 = add i32 %t13, 1
  %t15 = add i32 %t14, 1
  %t16 = add i32 %t15, 1
  %t17 = add i32 %t16, 1
  %t18 = add i32 %t17, 1
  %t19 = add i32 %t18, 1
  %t20 = add i32 %t19, 1
  ret void
bb.false:
  ret void
}
