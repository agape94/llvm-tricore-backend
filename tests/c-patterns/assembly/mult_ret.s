	.text
	.file	"mult_ret.c"
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
# %bb.0:                                # %entry
	mov.aa %a14, %a10
	sub.a %a10, 8
	mov %d15, 0
	st.w [%a14] -4, %d15
	ld.w %d15, [%a14] -8
	lt %d15, %d15, 1
	jnz %d15, .LBB0_2
	j .LBB0_1
.LBB0_1:                                # %if.then
	ld.w %d2, [%a14] -8
	mov %d15, 10
	add %d15, %d2
	st.w [%a14] -8, %d15
	mov %d15, 1
	st.w [%a14] -4, %d15
	j .LBB0_7
.LBB0_2:                                # %if.end
	ld.w %d15, [%a14] -8
	ge %d15, %d15, 0
	jnz %d15, .LBB0_4
	j .LBB0_3
.LBB0_3:                                # %if.then2
	ld.w %d2, [%a14] -8
	mov %d15, 20
	add %d15, %d2
	st.w [%a14] -8, %d15
	j .LBB0_4
.LBB0_4:                                # %if.end4
	ld.w %d15, [%a14] -8
	lt %d15, %d15, 101
	jnz %d15, .LBB0_6
	j .LBB0_5
.LBB0_5:                                # %if.then6
	ld.w %d2, [%a14] -8
	mov %d15, 100
	add %d15, %d2
	st.w [%a14] -8, %d15
	mov %d15, 2
	st.w [%a14] -4, %d15
	j .LBB0_7
.LBB0_6:                                # %if.end8
	ld.w %d2, [%a14] -8
	mov %d15, 3
	add %d15, %d2
	st.w [%a14] -8, %d15
	mov %d15, 0
	st.w [%a14] -4, %d15
	j .LBB0_7
.LBB0_7:                                # %return
	ld.w %d2, [%a14] -4
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        # -- End function
	.ident	"clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
