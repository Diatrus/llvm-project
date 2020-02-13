; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -march=amdgcn -mcpu=verde -verify-machineinstrs < %s | FileCheck -check-prefix=GCN -check-prefix=SI %s
; RUN: llc -march=amdgcn -mcpu=tonga -verify-machineinstrs < %s | FileCheck -check-prefix=GCN -check-prefix=VI %s

define amdgpu_ps float @main(float %arg0, float %arg1) #0 {
; SI-LABEL: main:
; SI:       ; %bb.0: ; %bb
; SI-NEXT:    v_cvt_i32_f32_e32 v0, v0
; SI-NEXT:    s_mov_b32 s0, 0
; SI-NEXT:    s_mov_b32 s1, s0
; SI-NEXT:    s_mov_b32 s2, s0
; SI-NEXT:    s_mov_b32 s3, s0
; SI-NEXT:    s_mov_b32 s4, s0
; SI-NEXT:    s_mov_b32 s5, s0
; SI-NEXT:    s_mov_b32 s6, s0
; SI-NEXT:    s_mov_b32 s7, s0
; SI-NEXT:    image_load v2, v0, s[0:7] dmask:0x1 unorm
; SI-NEXT:    v_and_b32_e32 v0, 7, v0
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_lshrrev_b32_e32 v0, v0, v2
; SI-NEXT:    v_and_b32_e32 v0, 1, v0
; SI-NEXT:    v_cmp_eq_u32_e32 vcc, 1, v0
; SI-NEXT:    v_cndmask_b32_e32 v0, 0, v1, vcc
; SI-NEXT:    v_cvt_pkrtz_f16_f32_e32 v0, s0, v0
; SI-NEXT:    ; return to shader part epilog
;
; VI-LABEL: main:
; VI:       ; %bb.0: ; %bb
; VI-NEXT:    v_cvt_i32_f32_e32 v0, v0
; VI-NEXT:    s_mov_b32 s0, 0
; VI-NEXT:    s_mov_b32 s1, s0
; VI-NEXT:    s_mov_b32 s2, s0
; VI-NEXT:    s_mov_b32 s3, s0
; VI-NEXT:    s_mov_b32 s4, s0
; VI-NEXT:    s_mov_b32 s5, s0
; VI-NEXT:    s_mov_b32 s6, s0
; VI-NEXT:    s_mov_b32 s7, s0
; VI-NEXT:    image_load v2, v0, s[0:7] dmask:0x1 unorm
; VI-NEXT:    v_and_b32_e32 v0, 7, v0
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_lshrrev_b32_e32 v0, v0, v2
; VI-NEXT:    v_and_b32_e32 v0, 1, v0
; VI-NEXT:    v_cmp_eq_u32_e32 vcc, 1, v0
; VI-NEXT:    v_cndmask_b32_e32 v0, 0, v1, vcc
; VI-NEXT:    v_cvt_pkrtz_f16_f32 v0, s0, v0
; VI-NEXT:    ; return to shader part epilog
bb:
  %tmp = fptosi float %arg0 to i32
  %tmp1 = call <4 x float> @llvm.amdgcn.image.load.1d.v4f32.i32(i32 15, i32 undef, <8 x i32> undef, i32 0, i32 0)
  %tmp2.f = extractelement <4 x float> %tmp1, i32 0
  %tmp2 = bitcast float %tmp2.f to i32
  %tmp3 = and i32 %tmp, 7
  %tmp4 = shl i32 1, %tmp3
  %tmp5 = and i32 %tmp2, %tmp4
  %tmp6 = icmp eq i32 %tmp5, 0
  %tmp7 = select i1 %tmp6, float 0.000000e+00, float %arg1
  %tmp8 = call <2 x half> @llvm.amdgcn.cvt.pkrtz(float undef, float %tmp7)
  %tmp9 = bitcast <2 x half> %tmp8 to float
  ret float %tmp9
}

declare <2 x half> @llvm.amdgcn.cvt.pkrtz(float, float) #1
declare <4 x float> @llvm.amdgcn.image.load.1d.v4f32.i32(i32, i32, <8 x i32>, i32, i32) #2

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind readonly }
