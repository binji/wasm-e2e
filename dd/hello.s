	.text
	.file	"test/hello.c"
	.globl	main
	.type	main,@function
main:                                   # @main
	.result 	i32
# BB#0:                                 # %entry
	i32.const	$push0=, .str
	call    	$discard=, puts, $pop0
	i32.const	$push1=, 0
	return  	$pop1
func_end0:
	.size	main, func_end0-main

	.type	.str,@object            # @.str
	.data
.str:
	.asciz	"Hello, world!\n"
	.size	.str, 15


	.imports
	.import puts "" puts (param i32) (result i32)
