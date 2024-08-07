	.file	"cube.c"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	get_char
	.type	get_char, @function
get_char:
.LFB22:
	.cfi_startproc
	mov	eax, 88
	cmp	dil, 5
	ja	.L1
	movzx	edi, dil
	lea	rax, CSWTCH.5[rip]
	movzx	eax, BYTE PTR [rax+rdi]
.L1:
	ret
	.cfi_endproc
.LFE22:
	.size	get_char, .-get_char
	.p2align 4
	.globl	get_piece
	.type	get_piece, @function
get_piece:
.LFB23:
	.cfi_startproc
	movzx	ecx, sil
	mov	eax, 88
	sal	ecx, 3
	shr	rdi, cl
	cmp	dil, 5
	ja	.L5
	movzx	edi, dil
	lea	rax, CSWTCH.5[rip]
	movzx	eax, BYTE PTR [rax+rdi]
.L5:
	ret
	.cfi_endproc
.LFE23:
	.size	get_piece, .-get_piece
	.p2align 4
	.globl	apply_move
	.type	apply_move, @function
apply_move:
.LFB26:
	.cfi_startproc
	push	r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	mov	eax, esi
	lea	r9, side_faces[rip]
	mov	r10, rdi
	push	r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movsx	esi, ah
	movzx	r8d, al
	lea	r11, turn_mask_table[rip]
	push	rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	and	esi, 3
	push	rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	mov	ecx, esi
	lea	rbx, [r9+r8*4]
	movsx	esi, sil
	sal	ecx, 4
	sub	esi, 1
	mov	r9, rbx
	movzx	ebp, cl
	sub	rsp, 72
	.cfi_def_cfa_offset 112
	mov	rdx, QWORD PTR fs:40
	mov	QWORD PTR 56[rsp], rdx
	xor	edx, edx
	movdqu	xmm0, XMMWORD PTR 112[rsp]
	mov	rdx, QWORD PTR 112[rsp+r8*8]
	movaps	XMMWORD PTR [rsp], xmm0
	rol	rdx, cl
	movdqu	xmm0, XMMWORD PTR 128[rsp]
	movaps	XMMWORD PTR 16[rsp], xmm0
	movdqu	xmm0, XMMWORD PTR 144[rsp]
	movaps	XMMWORD PTR 32[rsp], xmm0
	mov	QWORD PTR [rsp+r8*8], rdx
	mov	edx, 1
	sal	r8, 2
.L9:
	lea	r13d, [rsi+rdx]
	movzx	eax, BYTE PTR -1[rbx+rdx]
	mov	ecx, ebp
	add	rdx, 1
	and	r13d, 3
	movzx	edi, BYTE PTR [r9+r13]
	mov	rax, QWORD PTR 112[rsp+rax*8]
	add	r13, r8
	mov	r12, QWORD PTR [rsp+rdi*8]
	rol	rax, cl
	xor	rax, r12
	and	rax, QWORD PTR [r11+r13*8]
	xor	rax, r12
	mov	QWORD PTR [rsp+rdi*8], rax
	cmp	rdx, 5
	jne	.L9
	movdqa	xmm0, XMMWORD PTR [rsp]
	movups	XMMWORD PTR [r10], xmm0
	movdqa	xmm0, XMMWORD PTR 16[rsp]
	movups	XMMWORD PTR 16[r10], xmm0
	movdqa	xmm0, XMMWORD PTR 32[rsp]
	movups	XMMWORD PTR 32[r10], xmm0
	mov	rax, QWORD PTR 56[rsp]
	sub	rax, QWORD PTR fs:40
	jne	.L13
	add	rsp, 72
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	mov	rax, r10
	pop	rbx
	.cfi_def_cfa_offset 32
	pop	rbp
	.cfi_def_cfa_offset 24
	pop	r12
	.cfi_def_cfa_offset 16
	pop	r13
	.cfi_def_cfa_offset 8
	ret
.L13:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE26:
	.size	apply_move, .-apply_move
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%c%c%c\n%c %c\n%c%c%c\n"
	.text
	.p2align 4
	.globl	print_face
	.type	print_face, @function
print_face:
.LFB27:
	.cfi_startproc
	mov	rdx, rdi
	push	rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	mov	rax, rdi
	mov	ecx, 88
	shr	rdx, 32
	cmp	dl, 5
	ja	.L15
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rdx]
.L15:
	mov	rdx, rax
	mov	r10d, 88
	shr	rdx, 40
	cmp	dl, 5
	ja	.L16
	movzx	edx, dl
	lea	rsi, CSWTCH.5[rip]
	movsx	r10d, BYTE PTR [rsi+rdx]
.L16:
	mov	rdx, rax
	mov	ebx, 88
	shr	rdx, 48
	cmp	dl, 5
	ja	.L17
	movzx	edx, dl
	lea	rsi, CSWTCH.5[rip]
	movsx	ebx, BYTE PTR [rsi+rdx]
.L17:
	mov	rdx, rax
	mov	r9d, 88
	shr	rdx, 24
	cmp	dl, 5
	ja	.L18
	movzx	edx, dl
	lea	rsi, CSWTCH.5[rip]
	movsx	r9d, BYTE PTR [rsi+rdx]
.L18:
	mov	rdx, rax
	mov	r8d, 88
	shr	rdx, 56
	cmp	rdx, 5
	ja	.L19
	lea	rsi, CSWTCH.5[rip]
	movsx	r8d, BYTE PTR [rsi+rdx]
.L19:
	mov	rdx, rax
	mov	r11d, 88
	shr	rdx, 16
	cmp	dl, 5
	ja	.L20
	movzx	edx, dl
	lea	rsi, CSWTCH.5[rip]
	movsx	r11d, BYTE PTR [rsi+rdx]
.L20:
	mov	rdx, rax
	mov	edi, 88
	shr	rdx, 8
	cmp	dl, 5
	ja	.L21
	movzx	edx, dl
	lea	rsi, CSWTCH.5[rip]
	movsx	edi, BYTE PTR [rsi+rdx]
.L21:
	mov	esi, 88
	cmp	al, 5
	ja	.L22
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	esi, BYTE PTR [rdx+rax]
.L22:
	sub	rsp, 8
	.cfi_def_cfa_offset 24
	mov	edx, edi
	lea	rdi, .LC0[rip]
	xor	eax, eax
	push	rcx
	.cfi_def_cfa_offset 32
	mov	ecx, r11d
	push	r10
	.cfi_def_cfa_offset 40
	push	rbx
	.cfi_def_cfa_offset 48
	call	printf@PLT
	add	rsp, 32
	.cfi_def_cfa_offset 16
	pop	rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE27:
	.size	print_face, .-print_face
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC1:
	.string	"    %c%c%c\n    %c%c%c\n    %c%c%c\n"
	.section	.rodata.str1.1
.LC2:
	.string	"%c%c%c %c%c%c %c%c%c %c%c%c\n"
	.section	.rodata.str1.8
	.align 8
.LC3:
	.string	"    %c%c%c\n    %c%c%c\n    %c%c%c\n\n"
	.text
	.p2align 4
	.globl	print_cube
	.type	print_cube, @function
print_cube:
.LFB28:
	.cfi_startproc
	push	r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	push	r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	push	r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	mov	r13d, 88
	push	r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	push	rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	push	rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	sub	rsp, 24
	.cfi_def_cfa_offset 80
	mov	rax, QWORD PTR 80[rsp]
	mov	rdi, QWORD PTR 112[rsp]
	mov	r12, QWORD PTR 88[rsp]
	mov	rbp, QWORD PTR 96[rsp]
	mov	rdx, rax
	mov	QWORD PTR [rsp], rdi
	mov	rbx, QWORD PTR 104[rsp]
	shr	rdx, 32
	mov	r15, QWORD PTR 120[rsp]
	cmp	dl, 5
	ja	.L34
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	r13d, BYTE PTR [rcx+rdx]
.L34:
	mov	rdx, rax
	mov	r11d, 88
	shr	rdx, 40
	cmp	dl, 5
	ja	.L35
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	r11d, BYTE PTR [rcx+rdx]
.L35:
	mov	rdx, rax
	mov	r9d, 88
	shr	rdx, 48
	cmp	dl, 5
	ja	.L36
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	r9d, BYTE PTR [rcx+rdx]
.L36:
	mov	rdx, rax
	mov	edi, 88
	shr	rdx, 24
	cmp	dl, 5
	ja	.L37
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	edi, BYTE PTR [rcx+rdx]
.L37:
	mov	rdx, rax
	mov	r8d, 88
	shr	rdx, 56
	cmp	rdx, 5
	ja	.L38
	lea	rcx, CSWTCH.5[rip]
	movsx	r8d, BYTE PTR [rcx+rdx]
.L38:
	mov	rdx, rax
	mov	ecx, 88
	shr	rdx, 16
	cmp	dl, 5
	ja	.L39
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rdx]
.L39:
	mov	rsi, rax
	mov	edx, 88
	shr	rsi, 8
	cmp	sil, 5
	ja	.L40
	movzx	esi, sil
	lea	rdx, CSWTCH.5[rip]
	movsx	edx, BYTE PTR [rdx+rsi]
.L40:
	mov	esi, 88
	cmp	al, 5
	ja	.L41
	movzx	eax, al
	lea	rsi, CSWTCH.5[rip]
	movsx	esi, BYTE PTR [rsi+rax]
.L41:
	push	r13
	.cfi_def_cfa_offset 88
	xor	eax, eax
	mov	r13d, 88
	push	r11
	.cfi_def_cfa_offset 96
	push	r9
	.cfi_def_cfa_offset 104
	mov	r9d, 85
	push	rdi
	.cfi_def_cfa_offset 112
	lea	rdi, .LC1[rip]
	call	printf@PLT
	mov	rax, QWORD PTR 32[rsp]
	add	rsp, 32
	.cfi_def_cfa_offset 80
	shr	rax, 16
	cmp	al, 5
	ja	.L42
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	r13d, BYTE PTR [rdx+rax]
.L42:
	mov	rax, QWORD PTR [rsp]
	mov	r11d, 88
	shr	rax, 8
	cmp	al, 5
	ja	.L43
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	r11d, BYTE PTR [rdx+rax]
.L43:
	mov	rax, QWORD PTR [rsp]
	mov	edi, 88
	cmp	al, 5
	ja	.L44
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	edi, BYTE PTR [rdx+rax]
.L44:
	mov	rdx, r12
	mov	DWORD PTR 8[rsp], 88
	shr	rdx, 16
	cmp	dl, 5
	ja	.L45
	movzx	edx, dl
	lea	rax, CSWTCH.5[rip]
	movsx	eax, BYTE PTR [rax+rdx]
	mov	DWORD PTR 8[rsp], eax
.L45:
	mov	rdx, r12
	mov	DWORD PTR 12[rsp], 88
	shr	rdx, 8
	cmp	dl, 5
	ja	.L46
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rdx]
	mov	DWORD PTR 12[rsp], ecx
.L46:
	mov	r10d, 88
	cmp	r12b, 5
	ja	.L47
	movzx	edx, r12b
	lea	rcx, CSWTCH.5[rip]
	movsx	r10d, BYTE PTR [rcx+rdx]
.L47:
	mov	rdx, rbp
	mov	r14d, 88
	shr	rdx, 16
	cmp	dl, 5
	ja	.L48
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	r14d, BYTE PTR [rcx+rdx]
.L48:
	mov	rdx, rbp
	mov	r9d, 88
	shr	rdx, 8
	cmp	dl, 5
	ja	.L49
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	r9d, BYTE PTR [rcx+rdx]
.L49:
	mov	r8d, 88
	cmp	bpl, 5
	ja	.L50
	movzx	edx, bpl
	lea	rcx, CSWTCH.5[rip]
	movsx	r8d, BYTE PTR [rcx+rdx]
.L50:
	mov	rdx, rbx
	mov	ecx, 88
	shr	rdx, 16
	cmp	dl, 5
	ja	.L51
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rdx]
.L51:
	mov	rsi, rbx
	mov	edx, 88
	shr	rsi, 8
	cmp	sil, 5
	ja	.L52
	movzx	esi, sil
	lea	rdx, CSWTCH.5[rip]
	movsx	edx, BYTE PTR [rdx+rsi]
.L52:
	mov	esi, 88
	cmp	bl, 5
	ja	.L53
	movzx	esi, bl
	lea	rax, CSWTCH.5[rip]
	movsx	esi, BYTE PTR [rax+rsi]
.L53:
	sub	rsp, 8
	.cfi_def_cfa_offset 88
	push	r13
	.cfi_def_cfa_offset 96
	lea	r13, .LC2[rip]
	push	r11
	.cfi_def_cfa_offset 104
	push	rdi
	.cfi_def_cfa_offset 112
	mov	eax, DWORD PTR 40[rsp]
	mov	rdi, r13
	push	rax
	.cfi_def_cfa_offset 120
	mov	eax, DWORD PTR 52[rsp]
	push	rax
	.cfi_def_cfa_offset 128
	xor	eax, eax
	push	r10
	.cfi_def_cfa_offset 136
	push	r14
	.cfi_def_cfa_offset 144
	mov	r14d, 88
	call	printf@PLT
	mov	rax, QWORD PTR 64[rsp]
	add	rsp, 64
	.cfi_def_cfa_offset 80
	shr	rax, 24
	cmp	al, 5
	ja	.L54
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	r14d, BYTE PTR [rdx+rax]
.L54:
	mov	rax, QWORD PTR [rsp]
	mov	r11d, 88
	shr	rax, 56
	cmp	rax, 5
	ja	.L55
	lea	rdx, CSWTCH.5[rip]
	movsx	r11d, BYTE PTR [rdx+rax]
.L55:
	mov	rax, r12
	mov	r9d, 88
	shr	rax, 24
	cmp	al, 5
	ja	.L56
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	r9d, BYTE PTR [rdx+rax]
.L56:
	mov	rax, r12
	mov	edi, 88
	shr	rax, 56
	cmp	rax, 5
	ja	.L57
	lea	rdx, CSWTCH.5[rip]
	movsx	edi, BYTE PTR [rdx+rax]
.L57:
	mov	rax, rbp
	mov	edx, 88
	shr	rax, 24
	cmp	al, 5
	ja	.L58
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	edx, BYTE PTR [rdx+rax]
.L58:
	mov	rax, rbp
	mov	r8d, 88
	shr	rax, 56
	cmp	rax, 5
	ja	.L59
	lea	rcx, CSWTCH.5[rip]
	movsx	r8d, BYTE PTR [rcx+rax]
.L59:
	mov	rax, rbx
	mov	ecx, 88
	shr	rax, 24
	cmp	al, 5
	ja	.L60
	movzx	eax, al
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rax]
.L60:
	mov	rax, rbx
	mov	esi, 88
	shr	rax, 56
	cmp	rax, 5
	ja	.L61
	lea	rsi, CSWTCH.5[rip]
	movsx	esi, BYTE PTR [rsi+rax]
.L61:
	sub	rsp, 8
	.cfi_def_cfa_offset 88
	xor	eax, eax
	push	r14
	.cfi_def_cfa_offset 96
	push	66
	.cfi_def_cfa_offset 104
	push	r11
	.cfi_def_cfa_offset 112
	push	r9
	.cfi_def_cfa_offset 120
	mov	r9d, 70
	push	82
	.cfi_def_cfa_offset 128
	push	rdi
	.cfi_def_cfa_offset 136
	mov	rdi, r13
	push	rdx
	.cfi_def_cfa_offset 144
	mov	edx, 76
	call	printf@PLT
	mov	rax, QWORD PTR 64[rsp]
	add	rsp, 64
	.cfi_def_cfa_offset 80
	mov	DWORD PTR 8[rsp], 88
	shr	rax, 32
	cmp	al, 5
	ja	.L62
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	eax, BYTE PTR [rdx+rax]
	mov	DWORD PTR 8[rsp], eax
.L62:
	mov	rax, QWORD PTR [rsp]
	mov	r14d, 88
	shr	rax, 40
	cmp	al, 5
	ja	.L63
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	r14d, BYTE PTR [rdx+rax]
.L63:
	mov	r10, QWORD PTR [rsp]
	mov	r11d, 88
	shr	r10, 48
	cmp	r10b, 5
	ja	.L64
	movzx	r10d, r10b
	lea	rax, CSWTCH.5[rip]
	movsx	r11d, BYTE PTR [rax+r10]
.L64:
	mov	rax, r12
	mov	r10d, 88
	shr	rax, 32
	cmp	al, 5
	ja	.L65
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	r10d, BYTE PTR [rdx+rax]
.L65:
	mov	rax, r12
	mov	edi, 88
	shr	rax, 40
	cmp	al, 5
	ja	.L66
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	edi, BYTE PTR [rdx+rax]
.L66:
	mov	rax, r12
	mov	r12d, 88
	shr	rax, 48
	cmp	al, 5
	ja	.L67
	movzx	r12d, al
	lea	rax, CSWTCH.5[rip]
	movsx	r12d, BYTE PTR [rax+r12]
.L67:
	mov	rdx, rbp
	mov	eax, 88
	shr	rdx, 32
	cmp	dl, 5
	ja	.L68
	movzx	edx, dl
	lea	rax, CSWTCH.5[rip]
	movsx	eax, BYTE PTR [rax+rdx]
.L68:
	mov	rdx, rbp
	mov	r9d, 88
	shr	rdx, 40
	cmp	dl, 5
	ja	.L69
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	r9d, BYTE PTR [rcx+rdx]
.L69:
	shr	rbp, 48
	mov	r8d, 88
	cmp	bpl, 5
	ja	.L70
	movzx	ebp, bpl
	lea	rdx, CSWTCH.5[rip]
	movsx	r8d, BYTE PTR [rdx+rbp]
.L70:
	mov	rdx, rbx
	mov	ecx, 88
	shr	rdx, 32
	cmp	dl, 5
	ja	.L71
	movzx	edx, dl
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rdx]
.L71:
	mov	rsi, rbx
	mov	edx, 88
	shr	rsi, 40
	cmp	sil, 5
	ja	.L72
	movzx	esi, sil
	lea	rdx, CSWTCH.5[rip]
	movsx	edx, BYTE PTR [rdx+rsi]
.L72:
	shr	rbx, 48
	mov	esi, 88
	cmp	bl, 5
	ja	.L73
	movzx	ebx, bl
	lea	rsi, CSWTCH.5[rip]
	movsx	esi, BYTE PTR [rsi+rbx]
.L73:
	sub	rsp, 8
	.cfi_def_cfa_offset 88
	mov	ebx, DWORD PTR 16[rsp]
	push	rbx
	.cfi_def_cfa_offset 96
	push	r14
	.cfi_def_cfa_offset 104
	push	r11
	.cfi_def_cfa_offset 112
	push	r10
	.cfi_def_cfa_offset 120
	push	rdi
	.cfi_def_cfa_offset 128
	mov	rdi, r13
	push	r12
	.cfi_def_cfa_offset 136
	push	rax
	.cfi_def_cfa_offset 144
	xor	eax, eax
	call	printf@PLT
	mov	rax, r15
	add	rsp, 64
	.cfi_def_cfa_offset 80
	mov	edx, 88
	shr	rax, 32
	cmp	al, 5
	ja	.L74
	movzx	eax, al
	lea	rdx, CSWTCH.5[rip]
	movsx	edx, BYTE PTR [rdx+rax]
.L74:
	mov	rax, r15
	mov	ecx, 88
	shr	rax, 40
	cmp	al, 5
	ja	.L75
	movzx	eax, al
	lea	rcx, CSWTCH.5[rip]
	movsx	ecx, BYTE PTR [rcx+rax]
.L75:
	mov	rax, r15
	mov	r9d, 88
	shr	rax, 48
	cmp	al, 5
	ja	.L76
	movzx	eax, al
	lea	rsi, CSWTCH.5[rip]
	movsx	r9d, BYTE PTR [rsi+rax]
.L76:
	mov	rax, r15
	mov	r11d, 88
	shr	rax, 24
	cmp	al, 5
	ja	.L77
	movzx	eax, al
	lea	rsi, CSWTCH.5[rip]
	movsx	r11d, BYTE PTR [rsi+rax]
.L77:
	mov	rax, r15
	mov	r8d, 88
	shr	rax, 56
	cmp	rax, 5
	ja	.L78
	lea	rsi, CSWTCH.5[rip]
	movsx	r8d, BYTE PTR [rsi+rax]
.L78:
	mov	rax, r15
	mov	r10d, 88
	shr	rax, 16
	cmp	al, 5
	ja	.L79
	movzx	eax, al
	lea	rsi, CSWTCH.5[rip]
	movsx	r10d, BYTE PTR [rsi+rax]
.L79:
	mov	rax, r15
	mov	edi, 88
	shr	rax, 8
	cmp	al, 5
	ja	.L80
	movzx	eax, al
	lea	rsi, CSWTCH.5[rip]
	movsx	edi, BYTE PTR [rsi+rax]
.L80:
	mov	esi, 88
	cmp	r15b, 5
	ja	.L81
	movzx	eax, r15b
	lea	rsi, CSWTCH.5[rip]
	movsx	esi, BYTE PTR [rsi+rax]
.L81:
	mov	DWORD PTR 104[rsp], edx
	xor	eax, eax
	mov	edx, edi
	lea	rdi, .LC3[rip]
	mov	DWORD PTR 96[rsp], ecx
	mov	ecx, r10d
	mov	DWORD PTR 88[rsp], r9d
	mov	r9d, 68
	mov	DWORD PTR 80[rsp], r11d
	add	rsp, 24
	.cfi_def_cfa_offset 56
	pop	rbx
	.cfi_def_cfa_offset 48
	pop	rbp
	.cfi_def_cfa_offset 40
	pop	r12
	.cfi_def_cfa_offset 32
	pop	r13
	.cfi_def_cfa_offset 24
	pop	r14
	.cfi_def_cfa_offset 16
	pop	r15
	.cfi_def_cfa_offset 8
	jmp	printf@PLT
	.cfi_endproc
.LFE28:
	.size	print_cube, .-print_cube
	.section	.rodata
	.type	CSWTCH.5, @object
	.size	CSWTCH.5, 6
CSWTCH.5:
	.byte	85
	.byte	82
	.byte	70
	.byte	76
	.byte	66
	.byte	68
	.globl	turn_mask_table
	.align 32
	.type	turn_mask_table, @object
	.size	turn_mask_table, 192
turn_mask_table:
	.quad	16777215
	.quad	16777215
	.quad	16777215
	.quad	16777215
	.quad	1099511562240
	.quad	-281474976710401
	.quad	1099511562240
	.quad	1099511562240
	.quad	72057589742960640
	.quad	-281474976710401
	.quad	16777215
	.quad	1099511562240
	.quad	-281474976710401
	.quad	-281474976710401
	.quad	-281474976710401
	.quad	1099511562240
	.quad	16777215
	.quad	1099511562240
	.quad	72057589742960640
	.quad	-281474976710401
	.quad	72057589742960640
	.quad	72057589742960640
	.quad	72057589742960640
	.quad	72057589742960640
	.globl	side_masks
	.align 32
	.type	side_masks, @object
	.size	side_masks, 32
side_masks:
	.quad	16777215
	.quad	1099511562240
	.quad	72057589742960640
	.quad	-281474976710401
	.globl	side_faces
	.align 16
	.type	side_faces, @object
	.size	side_faces, 24
side_faces:
	.byte	4
	.byte	1
	.byte	2
	.byte	3
	.byte	0
	.byte	4
	.byte	5
	.byte	2
	.byte	0
	.byte	1
	.byte	5
	.byte	3
	.byte	0
	.byte	2
	.byte	5
	.byte	4
	.byte	0
	.byte	3
	.byte	5
	.byte	1
	.byte	2
	.byte	1
	.byte	4
	.byte	3
	.ident	"GCC: (GNU) 14.2.1 20240802"
	.section	.note.GNU-stack,"",@progbits
