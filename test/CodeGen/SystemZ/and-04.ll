; Test 64-bit ANDs in which the second operand is constant.
;
; RUN: llc < %s -mtriple=s390x-linux-gnu | FileCheck %s

; Use RISBG for a single bit.
define i64 @f1(i64 %a) {
; CHECK-LABEL: f1:
; CHECK: risbg %r2, %r2, 63, 191, 0
; CHECK: br %r14
  %and = and i64 %a, 1
  ret i64 %and
}

; Likewise 0xfffe.
define i64 @f2(i64 %a) {
; CHECK-LABEL: f2:
; CHECK: risbg %r2, %r2, 48, 190, 0
; CHECK: br %r14
  %and = and i64 %a, 65534
  ret i64 %and
}

; ...but 0xffff is a 16-bit zero extension.
define i64 @f3(i64 %a) {
; CHECK-LABEL: f3:
; CHECK: llghr %r2, %r2
; CHECK: br %r14
  %and = and i64 %a, 65535
  ret i64 %and
}

; Check the next value up, which can again use RISBG.
define i64 @f4(i64 %a) {
; CHECK-LABEL: f4:
; CHECK: risbg %r2, %r2, 47, 175, 0
; CHECK: br %r14
  %and = and i64 %a, 65536
  ret i64 %and
}

; Check 0xfffffffe, which can also use RISBG.
define i64 @f5(i64 %a) {
; CHECK-LABEL: f5:
; CHECK: risbg %r2, %r2, 32, 190, 0
; CHECK: br %r14
  %and = and i64 %a, 4294967294
  ret i64 %and
}

; Check the next value up, which is a 32-bit zero extension.
define i64 @f6(i64 %a) {
; CHECK-LABEL: f6:
; CHECK: llgfr %r2, %r2
; CHECK: br %r14
  %and = and i64 %a, 4294967295
  ret i64 %and
}

; Check the lowest useful NIHF value (0x00000002_ffffffff).
define i64 @f7(i64 %a) {
; CHECK-LABEL: f7:
; CHECK: nihf %r2, 2
; CHECK: br %r14
  %and = and i64 %a, 12884901887
  ret i64 %and
}

; Check the lowest useful NIHH value (0x0002ffff_ffffffff).
define i64 @f8(i64 %a) {
; CHECK-LABEL: f8:
; CHECK: nihh %r2, 2
; CHECK: br %r14
  %and = and i64 %a, 844424930131967
  ret i64 %and
}

; Check the highest useful NIHH value (0xfffaffff_ffffffff).
define i64 @f9(i64 %a) {
; CHECK-LABEL: f9:
; CHECK: nihh %r2, 65530
; CHECK: br %r14
  %and = and i64 %a, -1407374883553281
  ret i64 %and
}

; Check the highest useful NIHF value (0xfffefffe_ffffffff).
define i64 @f10(i64 %a) {
; CHECK-LABEL: f10:
; CHECK: nihf %r2, 4294901758
; CHECK: br %r14
  %and = and i64 %a, -281479271677953
  ret i64 %and
}

; Check the lowest useful NIHL value (0xffff0002_ffffffff).
define i64 @f11(i64 %a) {
; CHECK-LABEL: f11:
; CHECK: nihl %r2, 2
; CHECK: br %r14
  %and = and i64 %a, -281462091808769
  ret i64 %and
}

; Check the highest useful NIHL value (0xfffffffa_ffffffff).
define i64 @f12(i64 %a) {
; CHECK-LABEL: f12:
; CHECK: nihl %r2, 65530
; CHECK: br %r14
  %and = and i64 %a, -21474836481
  ret i64 %and
}

; Check the lowest useful NILF range (0xffffffff_00000002).
define i64 @f13(i64 %a) {
; CHECK-LABEL: f13:
; CHECK: nilf %r2, 2
; CHECK: br %r14
  %and = and i64 %a, -4294967294
  ret i64 %and
}

; Check the low end of the NILH range (0xffffffff_0002ffff).
define i64 @f14(i64 %a) {
; CHECK-LABEL: f14:
; CHECK: nilh %r2, 2
; CHECK: br %r14
  %and = and i64 %a, -4294770689
  ret i64 %and
}

; Check the next value up, which must use NILF.
define i64 @f15(i64 %a) {
; CHECK-LABEL: f15:
; CHECK: nilf %r2, 196608
; CHECK: br %r14
  %and = and i64 %a, -4294770688
  ret i64 %and
}

; Check the highest useful NILH value (0xffffffff_fffaffff).
define i64 @f16(i64 %a) {
; CHECK-LABEL: f16:
; CHECK: nilh %r2, 65530
; CHECK: br %r14
  %and = and i64 %a, -327681
  ret i64 %and
}

; Check the maximum useful NILF value (0xffffffff_fffefffe).
define i64 @f17(i64 %a) {
; CHECK-LABEL: f17:
; CHECK: nilf %r2, 4294901758
; CHECK: br %r14
  %and = and i64 %a, -65538
  ret i64 %and
}

; Check the lowest useful NILL value (0xffffffff_ffff0002).
define i64 @f18(i64 %a) {
; CHECK-LABEL: f18:
; CHECK: nill %r2, 2
; CHECK: br %r14
  %and = and i64 %a, -65534
  ret i64 %and
}

; Check the highest useful NILL value.
define i64 @f19(i64 %a) {
; CHECK-LABEL: f19:
; CHECK: nill %r2, 65530
; CHECK: br %r14
  %and = and i64 %a, -6
  ret i64 %and
}
