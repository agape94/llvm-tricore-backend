	.text
	.file	"high_callstack.c"
	.globl	f5                              # -- Begin function f5
	.type	f5,@function
f5:                                     # @f5
# %bb.0:                                # %entry
	ret
.Lfunc_end0:
	.size	f5, .Lfunc_end0-f5
                                        # -- End function
	.globl	f4                              # -- Begin function f4
	.type	f4,@function
f4:                                     # @f4
# %bb.0:                                # %entry
	call f5
	ret
.Lfunc_end1:
	.size	f4, .Lfunc_end1-f4
                                        # -- End function
	.globl	f3                              # -- Begin function f3
	.type	f3,@function
f3:                                     # @f3
# %bb.0:                                # %entry
	call f4
	ret
.Lfunc_end2:
	.size	f3, .Lfunc_end2-f3
                                        # -- End function
	.globl	f2                              # -- Begin function f2
	.type	f2,@function
f2:                                     # @f2
# %bb.0:                                # %entry
	call f3
	ret
.Lfunc_end3:
	.size	f2, .Lfunc_end3-f2
                                        # -- End function
	.globl	f1                              # -- Begin function f1
	.type	f1,@function
f1:                                     # @f1
# %bb.0:                                # %entry
	call f2
	ret
.Lfunc_end4:
	.size	f1, .Lfunc_end4-f1
                                        # -- End function
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
# %bb.0:                                # %entry
	mov.aa %a14, %a10
	sub.a %a10, 8
	mov %d2, 0
	st.w [%a14] -4, %d2
	call f1
	ret
.Lfunc_end5:
	.size	main, .Lfunc_end5-main
                                        # -- End function
	.ident	"clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym f5
	.addrsig_sym f4
	.addrsig_sym f3
	.addrsig_sym f2
	.addrsig_sym f1
