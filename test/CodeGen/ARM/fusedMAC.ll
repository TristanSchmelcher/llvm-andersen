; RUN: llc < %s -march=arm -mattr=+neon-vfpv4 | FileCheck %s
; Check generated fused MAC and MLS.

define double @fusedMACTest1(double %d1, double %d2, double %d3) nounwind readnone noinline {
;CHECK: fusedMACTest1:
;CHECK: vfma.f64
  %1 = fmul double %d1, %d2
  %2 = fadd double %1, %d3
  ret double %2
}

define float @fusedMACTest2(float %f1, float %f2, float %f3) nounwind readnone noinline {
;CHECK: fusedMACTest2:
;CHECK: vfma.f32
  %1 = fmul float %f1, %f2
  %2 = fadd float %1, %f3
  ret float %2
}

define double @fusedMACTest3(double %d1, double %d2, double %d3) nounwind readnone noinline {
;CHECK: fusedMACTest3:
;CHECK: vfms.f64
  %1 = fmul double %d2, %d3
  %2 = fsub double %d1, %1
  ret double %2
}

define float @fusedMACTest4(float %f1, float %f2, float %f3) nounwind readnone noinline {
;CHECK: fusedMACTest4:
;CHECK: vfms.f32
  %1 = fmul float %f2, %f3
  %2 = fsub float %f1, %1
  ret float %2
}

define double @fusedMACTest5(double %d1, double %d2, double %d3) nounwind readnone noinline {
;CHECK: fusedMACTest5:
;CHECK: vfnma.f64
  %1 = fmul double %d1, %d2
  %2 = fsub double -0.0, %1
  %3 = fsub double %2, %d3
  ret double %3
}

define float @fusedMACTest6(float %f1, float %f2, float %f3) nounwind {
;CHECK: fusedMACTest6:
;CHECK: vfnma.f32
  %1 = fmul float %f1, %f2
  %2 = fsub float -0.0, %1
  %3 = fsub float %2, %f3
  ret float %3
}

define double @fusedMACTest7(double %d1, double %d2, double %d3) nounwind {
;CHECK: fusedMACTest7:
;CHECK: vfnms.f64
  %1 = fmul double %d1, %d2
  %2 = fsub double %1, %d3
  ret double %2
}

define float @fusedMACTest8(float %f1, float %f2, float %f3) nounwind {
;CHECK: fusedMACTest8:
;CHECK: vfnms.f32
  %1 = fmul float %f1, %f2
  %2 = fsub float %1, %f3
  ret float %2
}
