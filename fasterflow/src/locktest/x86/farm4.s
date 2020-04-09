	.section	__TEXT,__text,regular,pure_instructions
	.globl	_my_lock
	.align	4, 0x90
_my_lock:
Leh_func_begin1:
	pushq	%rbp
Ltmp0:
	movq	%rsp, %rbp
Ltmp1:
	movq	%rdi, -8(%rbp)
	movb	$1, -10(%rbp)
LBB1_1:
	movq	-8(%rbp), %rax
	movb	-10(%rbp), %cl
	## InlineAsm Start
	lock; xchgb  %cl,(%rax)
	
	## InlineAsm End
	movb	%cl, -9(%rbp)
	movb	-9(%rbp), %al
	cmpb	$1, %al
	je	LBB1_1
	popq	%rbp
	ret
Leh_func_end1:

	.globl	_my_unlock
	.align	4, 0x90
_my_unlock:
Leh_func_begin2:
	pushq	%rbp
Ltmp2:
	movq	%rsp, %rbp
Ltmp3:
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	$0, (%rax)
	popq	%rbp
	ret
Leh_func_end2:

	.globl	_newtq
	.align	4, 0x90
_newtq:
Leh_func_begin3:
	pushq	%rbp
Ltmp4:
	movq	%rsp, %rbp
Ltmp5:
	subq	$32, %rsp
Ltmp6:
	movabsq	$528, %rax
	movq	%rax, %rdi
	callq	_malloc
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movb	$0, (%rax)
	movq	-24(%rbp), %rax
	movl	$0, 120(%rax)
	movq	-24(%rbp), %rax
	movl	$0, 124(%rax)
	movq	-24(%rbp), %rax
	movabsq	$8, %rcx
	addq	%rcx, %rax
	movabsq	$0, %rcx
	movq	%rax, %rdi
	movq	%rcx, %rsi
	movq	%rcx, -32(%rbp)
	callq	_pthread_cond_init
	movq	-24(%rbp), %rax
	movabsq	$56, %rcx
	addq	%rcx, %rax
	movq	%rax, %rdi
	movq	-32(%rbp), %rsi
	callq	_pthread_mutex_init
	movq	-24(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end3:

	.globl	_gettask
	.align	4, 0x90
_gettask:
Leh_func_begin4:
	pushq	%rbp
Ltmp7:
	movq	%rsp, %rbp
Ltmp8:
	subq	$32, %rsp
Ltmp9:
	movq	%rdi, -8(%rbp)
LBB4_1:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_my_lock
	movq	-8(%rbp), %rax
	movl	124(%rax), %eax
	movq	-8(%rbp), %rcx
	movl	120(%rcx), %ecx
	cmpl	%ecx, %eax
	jne	LBB4_3
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_my_unlock
	movq	-8(%rbp), %rax
	movabsq	$56, %rcx
	addq	%rcx, %rax
	movq	-8(%rbp), %rcx
	movabsq	$8, %rdx
	addq	%rdx, %rcx
	movq	%rcx, %rdi
	movq	%rax, %rsi
	callq	_pthread_cond_wait
LBB4_3:
	movq	-8(%rbp), %rax
	movl	124(%rax), %eax
	movq	-8(%rbp), %rcx
	movl	120(%rcx), %ecx
	cmpl	%ecx, %eax
	je	LBB4_1
	movq	-8(%rbp), %rax
	movl	124(%rax), %eax
	movq	-8(%rbp), %rcx
	movslq	%eax, %rax
	movl	128(%rcx,%rax,4), %eax
	movl	%eax, -20(%rbp)
	movq	-8(%rbp), %rax
	movl	124(%rax), %eax
	addl	$1, %eax
	movq	-8(%rbp), %rcx
	movl	%eax, 124(%rcx)
	movq	-8(%rbp), %rax
	movl	124(%rax), %eax
	cmpl	$100, %eax
	jne	LBB4_6
	movq	-8(%rbp), %rax
	movl	$0, 124(%rax)
LBB4_6:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_my_unlock
	movl	-20(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end4:

	.globl	_puttask
	.align	4, 0x90
_puttask:
Leh_func_begin5:
	pushq	%rbp
Ltmp10:
	movq	%rsp, %rbp
Ltmp11:
	subq	$32, %rsp
Ltmp12:
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
LBB5_1:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_my_lock
	movq	-8(%rbp), %rax
	movl	120(%rax), %eax
	movq	-8(%rbp), %rcx
	movl	124(%rcx), %ecx
	subl	$1, %ecx
	cmpl	%ecx, %eax
	jne	LBB5_3
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_my_unlock
LBB5_3:
	movq	-8(%rbp), %rax
	movl	120(%rax), %eax
	movq	-8(%rbp), %rcx
	movl	124(%rcx), %ecx
	subl	$1, %ecx
	cmpl	%ecx, %eax
	je	LBB5_1
	movq	-8(%rbp), %rax
	movl	120(%rax), %eax
	movq	-8(%rbp), %rcx
	movslq	%eax, %rdx
	movl	-12(%rbp), %esi
	movl	%esi, 128(%rcx,%rdx,4)
	addl	$1, %eax
	movq	-8(%rbp), %rcx
	movl	%eax, 120(%rcx)
	movq	-8(%rbp), %rax
	movl	120(%rax), %eax
	cmpl	$100, %eax
	jne	LBB5_6
	movq	-8(%rbp), %rax
	movl	$0, 120(%rax)
LBB5_6:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_my_unlock
	movq	-8(%rbp), %rcx
	movl	124(%rcx), %ecx
	addl	$1, %ecx
	movq	-8(%rbp), %rsi
	movl	120(%rsi), %esi
	movl	$1374389535, %edi
	movl	%ecx, %eax
	imull	%edi
	movl	%edx, %eax
	movl	%eax, %edi
	shrl	$31, %edi
	sarl	$5, %eax
	leal	(%rax,%rdi), %eax
	imull	$100, %eax, %eax
	subl	%eax, %ecx
	cmpl	%esi, %ecx
	jne	LBB5_8
	movq	-8(%rbp), %rax
	movabsq	$8, %rcx
	addq	%rcx, %rax
	movq	%rax, %rdi
	callq	_pthread_cond_signal
LBB5_8:
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end5:

	.globl	_worker
	.align	4, 0x90
_worker:
Leh_func_begin6:
	pushq	%rbp
Ltmp13:
	movq	%rsp, %rbp
Ltmp14:
	subq	$32, %rsp
Ltmp15:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	callq	_pthread_self
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	_gettask
	movl	%eax, %ecx
	movl	%ecx, -20(%rbp)
	movl	-20(%rbp), %ecx
	movl	%ecx, %edi
	callq	_payload
	movl	%eax, %ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rcx
	movl	-24(%rbp), %edx
	movq	%rcx, %rdi
	movl	%edx, %esi
	callq	_puttask
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end6:

	.globl	_sworker
	.align	4, 0x90
_sworker:
Leh_func_begin7:
	pushq	%rbp
Ltmp16:
	movq	%rsp, %rbp
Ltmp17:
	subq	$16, %rsp
Ltmp18:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
LBB7_1:
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	movq	%rax, %rdi
	movq	%rcx, %rsi
	callq	_worker
	jmp	LBB7_1
Leh_func_end7:

	.globl	_makeThread
	.align	4, 0x90
_makeThread:
Leh_func_begin8:
	pushq	%rbp
Ltmp19:
	movq	%rsp, %rbp
Ltmp20:
	subq	$32, %rsp
Ltmp21:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movabsq	$8, %rax
	movq	%rax, %rdi
	callq	_malloc
	movq	%rax, -32(%rbp)
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	-32(%rbp), %rdx
	movabsq	$0, %rsi
	movq	%rdx, %rdi
	movq	%rcx, %rdx
	movq	%rax, %rcx
	callq	_pthread_create
	movl	%eax, %ecx
	movl	%ecx, -20(%rbp)
	movl	-20(%rbp), %ecx
	cmpl	$0, %ecx
	je	LBB8_2
	movl	-20(%rbp), %eax
	xorb	%cl, %cl
	leaq	L_.str(%rip), %rdx
	movq	%rdx, %rdi
	movl	%eax, %esi
	movb	%cl, %al
	callq	_printf
	movl	$4294967295, %eax
	movl	%eax, %edi
	callq	_exit
LBB8_2:
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end8:

	.globl	_createfarm
	.align	4, 0x90
_createfarm:
Leh_func_begin9:
	pushq	%rbp
Ltmp22:
	movq	%rsp, %rbp
Ltmp23:
	subq	$32, %rsp
Ltmp24:
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	$0, -24(%rbp)
	jmp	LBB9_2
LBB9_1:
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rax, %rdi
	movq	%rcx, %rsi
	callq	_makeThread
	movq	-24(%rbp), %rax
	movabsq	$1, %rcx
	addq	%rcx, %rax
	movq	%rax, -24(%rbp)
LBB9_2:
	movl	-12(%rbp), %eax
	movslq	%eax, %rax
	movq	-24(%rbp), %rcx
	cmpq	%rcx, %rax
	jg	LBB9_1
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end9:

	.globl	_createpipe
	.align	4, 0x90
_createpipe:
Leh_func_begin10:
	pushq	%rbp
Ltmp25:
	movq	%rsp, %rbp
Ltmp26:
	subq	$16, %rsp
Ltmp27:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movabsq	$0, %rcx
	movq	%rax, %rdi
	movq	%rcx, %rsi
	callq	_makeThread
	movq	-16(%rbp), %rax
	movabsq	$1, %rcx
	movq	%rax, %rdi
	movq	%rcx, %rsi
	callq	_makeThread
	addq	$16, %rsp
	popq	%rbp
	ret
Leh_func_end10:

	.section	__TEXT,__cstring,cstring_literals
	.align	3
L_.str:
	.asciz	 "ERROR; return code from pthread_create() is %d\n"

	.section	__TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame0:
Lsection_eh_frame:
Leh_frame_common:
Lset0 = Leh_frame_common_end-Leh_frame_common_begin
	.long	Lset0
Leh_frame_common_begin:
	.long	0
	.byte	1
	.asciz	 "zR"
	.byte	1
	.byte	120
	.byte	16
	.byte	1
	.byte	16
	.byte	12
	.byte	7
	.byte	8
	.byte	144
	.byte	1
	.align	3
Leh_frame_common_end:
	.globl	_my_lock.eh
_my_lock.eh:
Lset1 = Leh_frame_end1-Leh_frame_begin1
	.long	Lset1
Leh_frame_begin1:
Lset2 = Leh_frame_begin1-Leh_frame_common
	.long	Lset2
Ltmp28:
	.quad	Leh_func_begin1-Ltmp28
Lset3 = Leh_func_end1-Leh_func_begin1
	.quad	Lset3
	.byte	0
	.byte	4
Lset4 = Ltmp0-Leh_func_begin1
	.long	Lset4
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset5 = Ltmp1-Ltmp0
	.long	Lset5
	.byte	13
	.byte	6
	.align	3
Leh_frame_end1:

	.globl	_my_unlock.eh
_my_unlock.eh:
Lset6 = Leh_frame_end2-Leh_frame_begin2
	.long	Lset6
Leh_frame_begin2:
Lset7 = Leh_frame_begin2-Leh_frame_common
	.long	Lset7
Ltmp29:
	.quad	Leh_func_begin2-Ltmp29
Lset8 = Leh_func_end2-Leh_func_begin2
	.quad	Lset8
	.byte	0
	.byte	4
Lset9 = Ltmp2-Leh_func_begin2
	.long	Lset9
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset10 = Ltmp3-Ltmp2
	.long	Lset10
	.byte	13
	.byte	6
	.align	3
Leh_frame_end2:

	.globl	_newtq.eh
_newtq.eh:
Lset11 = Leh_frame_end3-Leh_frame_begin3
	.long	Lset11
Leh_frame_begin3:
Lset12 = Leh_frame_begin3-Leh_frame_common
	.long	Lset12
Ltmp30:
	.quad	Leh_func_begin3-Ltmp30
Lset13 = Leh_func_end3-Leh_func_begin3
	.quad	Lset13
	.byte	0
	.byte	4
Lset14 = Ltmp4-Leh_func_begin3
	.long	Lset14
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset15 = Ltmp5-Ltmp4
	.long	Lset15
	.byte	13
	.byte	6
	.align	3
Leh_frame_end3:

	.globl	_gettask.eh
_gettask.eh:
Lset16 = Leh_frame_end4-Leh_frame_begin4
	.long	Lset16
Leh_frame_begin4:
Lset17 = Leh_frame_begin4-Leh_frame_common
	.long	Lset17
Ltmp31:
	.quad	Leh_func_begin4-Ltmp31
Lset18 = Leh_func_end4-Leh_func_begin4
	.quad	Lset18
	.byte	0
	.byte	4
Lset19 = Ltmp7-Leh_func_begin4
	.long	Lset19
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset20 = Ltmp8-Ltmp7
	.long	Lset20
	.byte	13
	.byte	6
	.align	3
Leh_frame_end4:

	.globl	_puttask.eh
_puttask.eh:
Lset21 = Leh_frame_end5-Leh_frame_begin5
	.long	Lset21
Leh_frame_begin5:
Lset22 = Leh_frame_begin5-Leh_frame_common
	.long	Lset22
Ltmp32:
	.quad	Leh_func_begin5-Ltmp32
Lset23 = Leh_func_end5-Leh_func_begin5
	.quad	Lset23
	.byte	0
	.byte	4
Lset24 = Ltmp10-Leh_func_begin5
	.long	Lset24
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset25 = Ltmp11-Ltmp10
	.long	Lset25
	.byte	13
	.byte	6
	.align	3
Leh_frame_end5:

	.globl	_worker.eh
_worker.eh:
Lset26 = Leh_frame_end6-Leh_frame_begin6
	.long	Lset26
Leh_frame_begin6:
Lset27 = Leh_frame_begin6-Leh_frame_common
	.long	Lset27
Ltmp33:
	.quad	Leh_func_begin6-Ltmp33
Lset28 = Leh_func_end6-Leh_func_begin6
	.quad	Lset28
	.byte	0
	.byte	4
Lset29 = Ltmp13-Leh_func_begin6
	.long	Lset29
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset30 = Ltmp14-Ltmp13
	.long	Lset30
	.byte	13
	.byte	6
	.align	3
Leh_frame_end6:

	.globl	_sworker.eh
_sworker.eh:
Lset31 = Leh_frame_end7-Leh_frame_begin7
	.long	Lset31
Leh_frame_begin7:
Lset32 = Leh_frame_begin7-Leh_frame_common
	.long	Lset32
Ltmp34:
	.quad	Leh_func_begin7-Ltmp34
Lset33 = Leh_func_end7-Leh_func_begin7
	.quad	Lset33
	.byte	0
	.byte	4
Lset34 = Ltmp16-Leh_func_begin7
	.long	Lset34
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset35 = Ltmp17-Ltmp16
	.long	Lset35
	.byte	13
	.byte	6
	.align	3
Leh_frame_end7:

	.globl	_makeThread.eh
_makeThread.eh:
Lset36 = Leh_frame_end8-Leh_frame_begin8
	.long	Lset36
Leh_frame_begin8:
Lset37 = Leh_frame_begin8-Leh_frame_common
	.long	Lset37
Ltmp35:
	.quad	Leh_func_begin8-Ltmp35
Lset38 = Leh_func_end8-Leh_func_begin8
	.quad	Lset38
	.byte	0
	.byte	4
Lset39 = Ltmp19-Leh_func_begin8
	.long	Lset39
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset40 = Ltmp20-Ltmp19
	.long	Lset40
	.byte	13
	.byte	6
	.align	3
Leh_frame_end8:

	.globl	_createfarm.eh
_createfarm.eh:
Lset41 = Leh_frame_end9-Leh_frame_begin9
	.long	Lset41
Leh_frame_begin9:
Lset42 = Leh_frame_begin9-Leh_frame_common
	.long	Lset42
Ltmp36:
	.quad	Leh_func_begin9-Ltmp36
Lset43 = Leh_func_end9-Leh_func_begin9
	.quad	Lset43
	.byte	0
	.byte	4
Lset44 = Ltmp22-Leh_func_begin9
	.long	Lset44
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset45 = Ltmp23-Ltmp22
	.long	Lset45
	.byte	13
	.byte	6
	.align	3
Leh_frame_end9:

	.globl	_createpipe.eh
_createpipe.eh:
Lset46 = Leh_frame_end10-Leh_frame_begin10
	.long	Lset46
Leh_frame_begin10:
Lset47 = Leh_frame_begin10-Leh_frame_common
	.long	Lset47
Ltmp37:
	.quad	Leh_func_begin10-Ltmp37
Lset48 = Leh_func_end10-Leh_func_begin10
	.quad	Lset48
	.byte	0
	.byte	4
Lset49 = Ltmp25-Leh_func_begin10
	.long	Lset49
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset50 = Ltmp26-Ltmp25
	.long	Lset50
	.byte	13
	.byte	6
	.align	3
Leh_frame_end10:


.subsections_via_symbols
