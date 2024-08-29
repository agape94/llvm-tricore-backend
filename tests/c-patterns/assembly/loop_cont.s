	.text
	.file	"loop_cont.c"
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
# %bb.0:                                # %entry
	mov.aa %a14, %a10
	sub.a %a10, 24
	mov %d15, 0
	st.w [%a14] -4, %d15
	st.w [%a14] -16, %d15
	st.w [%a14] -20, %d15
	j .LBB0_1
.LBB0_1:                                # %for.cond
                                        # =>This Inner Loop Header: Depth=1
	ld.w %d15, [%a14] -20
	ld.w %d2, [%a14] -8
	ge %d15, %d15, %d2
	jnz %d15, .LBB0_6
	j .LBB0_2
.LBB0_2:                                # %for.body
                                        #   in Loop: Header=BB0_1 Depth=1
	ld.w %d15, [%a14] -20
	ld.w %d2, [%a14] -12
	ne %d15, %d15, %d2
	jnz %d15, .LBB0_4
	j .LBB0_3
.LBB0_3:                                # %if.then
                                        #   in Loop: Header=BB0_1 Depth=1
	j .LBB0_5
.LBB0_4:                                # %if.end
                                        #   in Loop: Header=BB0_1 Depth=1
	ld.w %d2, [%a14] -16
	mov %d15, 1
	add %d15, %d2
	st.w [%a14] -16, %d15
	j .LBB0_5
.LBB0_5:                                # %for.inc
                                        #   in Loop: Header=BB0_1 Depth=1
	ld.w %d2, [%a14] -20
	mov %d15, 1
	add %d15, %d2
	st.w [%a14] -20, %d15
	j .LBB0_1
.LBB0_6:                                # %for.end
	ld.w %d2, [%a14] -16
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        # -- End function
	.ident	"clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
