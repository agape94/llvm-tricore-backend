	.text
	.file	"many_funcs.c"
	.globl	func3                           # -- Begin function func3
	.type	func3,@function
func3:                                  # @func3
# %bb.0:                                # %entry
	ret
.Lfunc_end0:
	.size	func3, .Lfunc_end0-func3
                                        # -- End function
	.globl	func2                           # -- Begin function func2
	.type	func2,@function
func2:                                  # @func2
# %bb.0:                                # %entry
	call func3
	ret
.Lfunc_end1:
	.size	func2, .Lfunc_end1-func2
                                        # -- End function
	.globl	func4                           # -- Begin function func4
	.type	func4,@function
func4:                                  # @func4
# %bb.0:                                # %entry
	call func3
	ret
.Lfunc_end2:
	.size	func4, .Lfunc_end2-func4
                                        # -- End function
	.globl	func1                           # -- Begin function func1
	.type	func1,@function
func1:                                  # @func1
# %bb.0:                                # %entry
	call func3
	call func4
	ret
.Lfunc_end3:
	.size	func1, .Lfunc_end3-func1
                                        # -- End function
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
# %bb.0:                                # %entry
	mov.aa %a14, %a10
	sub.a %a10, 8
	mov %d2, 0
	st.w [%a14] -4, %d2
	call func1
	call func2
	call func4
	ret
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
                                        # -- End function
	.ident	"clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym func3
	.addrsig_sym func2
	.addrsig_sym func4
	.addrsig_sym func1
