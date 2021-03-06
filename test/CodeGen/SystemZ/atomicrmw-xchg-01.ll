; Test 8-bit atomic exchange.
;
; RUN: llc < %s -mtriple=s390x-linux-gnu | FileCheck %s -check-prefix=CHECK
; RUN: llc < %s -mtriple=s390x-linux-gnu | FileCheck %s -check-prefix=CHECK-SHIFT

; Check exchange with a variable.
; - CHECK is for the main loop.
; - CHECK-SHIFT makes sure that the negated shift count used by the second
;   RLL is set up correctly.  The negation is independent of the NILL and L
;   tested in CHECK.  CHECK-SHIFT also checks that %r3 is not modified before
;   being used in the RISBG (in contrast to things like atomic addition,
;   which shift %r3 left so that %b is at the high end of the word).
define i8 @f1(i8 *%src, i8 %b) {
; CHECK-LABEL: f1:
; CHECK-DAG: sllg [[SHIFT:%r[1-9]+]], %r2, 3
; CHECK-DAG: risbg [[BASE:%r[1-9]+]], %r2, 0, 189, 0
; CHECK: l [[OLD:%r[0-9]+]], 0([[BASE]])
; CHECK: [[LABEL:\.[^:]*]]:
; CHECK: rll [[ROT:%r[0-9]+]], [[OLD]], 0([[SHIFT]])
; CHECK: risbg [[ROT]], %r3, 32, 39, 24
; CHECK: rll [[NEW:%r[0-9]+]], [[ROT]], 0({{%r[1-9]+}})
; CHECK: cs [[OLD]], [[NEW]], 0([[BASE]])
; CHECK: jlh [[LABEL]]
; CHECK: rll %r2, [[OLD]], 8([[SHIFT]])
; CHECK: br %r14
;
; CHECK-SHIFT-LABEL: f1:
; CHECK-SHIFT-NOT: %r3
; CHECK-SHIFT: sllg [[SHIFT:%r[1-9]+]], %r2, 3
; CHECK-SHIFT-NOT: %r3
; CHECK-SHIFT: lcr [[NEGSHIFT:%r[1-9]+]], [[SHIFT]]
; CHECK-SHIFT-NOT: %r3
; CHECK-SHIFT: rll
; CHECK-SHIFT-NOT: %r3
; CHECK-SHIFT: risbg {{%r[0-9]+}}, %r3, 32, 39, 24
; CHECK-SHIFT: rll {{%r[0-9]+}}, {{%r[0-9]+}}, 0([[NEGSHIFT]])
; CHECK-SHIFT: rll
; CHECK-SHIFT: br %r14
  %res = atomicrmw xchg i8 *%src, i8 %b seq_cst
  ret i8 %res
}

; Check exchange with a constant.  We should force the constant into
; a register and use the sequence above.
define i8 @f2(i8 *%src) {
; CHECK-LABEL: f2:
; CHECK: lhi [[VALUE:%r[0-9]+]], 88
; CHECK: risbg {{%r[0-9]+}}, [[VALUE]], 32, 39, 24
; CHECK: br %r14
;
; CHECK-SHIFT-LABEL: f2:
; CHECK-SHIFT: br %r14
  %res = atomicrmw xchg i8 *%src, i8 88 seq_cst
  ret i8 %res
}
