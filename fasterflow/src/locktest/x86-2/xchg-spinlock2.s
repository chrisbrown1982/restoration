	.text
	.align 4,0x90
.globl _my_lock
_my_lock:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	.align 4,0x90
L2:
	movl	$1, %eax
	lock; xchgb  %al,(%rdi)
	
	decb	%al
	je	L2
	leave
	ret
LFE2:
	.align 4,0x90
.globl _my_unlock
_my_unlock:
LFB3:
	pushq	%rbp
LCFI2:
	movq	%rsp, %rbp
LCFI3:
	movb	$0, (%rdi)
	leave
	ret
LFE3:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0x0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
.globl _my_lock.eh
_my_lock.eh:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB2-.
	.set L$set$2,LFE2-LFB2
	.quad L$set$2
	.byte	0x0
	.byte	0x4
	.set L$set$3,LCFI0-LFB2
	.long L$set$3
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE1:
.globl _my_unlock.eh
_my_unlock.eh:
LSFDE3:
	.set L$set$5,LEFDE3-LASFDE3
	.long L$set$5
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB3-.
	.set L$set$6,LFE3-LFB3
	.quad L$set$6
	.byte	0x0
	.byte	0x4
	.set L$set$7,LCFI2-LFB3
	.long L$set$7
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$8,LCFI3-LCFI2
	.long L$set$8
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE3:
	.subsections_via_symbols
