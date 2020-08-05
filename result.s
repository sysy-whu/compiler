.arch armv7-a
	.text
	.align	2
	.global	div_zt
	.arch armv7-a
	.syntax unified
	.arm
	.fpu vfp
	.type	div_zt, %function
div_zt:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-12]
	mov	r3, #0
	str	r3, [fp, #-8]
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	cmp	r2, r3
	bge	.L2
	mov	r3, #0
	b	.L3
.L2:
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L4
	mov	r3, #0
	b	.L3
.L4:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	sub	r3, r2, r3
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	cmp	r2, r3
	bge	.L4
	ldr	r3, [fp, #-8]
.L3:
	mov	r0, r3
	add	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
	.size	div_zt, .-div_zt
	.align	2
	.global	mod_zt
	.syntax unified
	.arm
	.fpu vfp
	.type	mod_zt, %function
mod_zt:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-12]
	mov	r3, #0
	str	r3, [fp, #-8]
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	cmp	r2, r3
	bge	.L6
	mov	r3, #0
	b	.L7
.L6:
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L8
	mov	r3, #0
	b	.L7
.L8:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	sub	r3, r2, r3
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	cmp	r2, r3
	bge	.L8
	ldr	r3, [fp, #-16]
.L7:
	mov	r0, r3
	add	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
	.text
	.global a
	.data
	.align 2
	.type a, %object
	.size a,100
a:
	.word 1
	.word 2
	.word 3
	.word 4
	.word 5
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.text
	.align 2
	.global func
	.syntax unified
	.arm
	.fpu vfp
	.type func, %function
func:
	push {r4, fp, lr}
	add fp,sp,#8
	sub sp,sp,#24
	str r0,[fp, #-12]
	mov r1,#0
	str r1,[fp, #-16]
	mov r1,#0
	str r1,[fp, #-20]
	mov r1,#0
	str r1,[fp, #-24]
.L9:
	ldr r1,[fp, #-16]
	mov r2,#5
	cmp r1,r2
	movlt r1,#1
	movge r1,#0
	cmp r1,#0
	beq .L10
.L11:
	ldr r1,[fp, #-20]
	mov r2,#5
	cmp r1,r2
	movlt r1,#1
	movge r1,#0
	cmp r1,#0
	beq .L12
	ldr r1,[fp, #-12]
	ldr r2,[fp, #-16]
	mov r3,#4
	mul r2,r3,r2
	add r1,r2,r1
	ldr r2,[fp, #-20]
	mov r3,#4
	mul r2,r3,r2
	add r1,r2,r1
	ldr r1,[r1]
	push {r1 }
	ldr r1,[fp, #-24]
	pop {r2 }
	add r2,r1,r2
	str r2,[fp, #-24]
	mov r1,#1
	push {r1 }
	ldr r1,[fp, #-20]
	pop {r2 }
	add r2,r1,r2
	str r2,[fp, #-20]
	b .L11
.L12:
	mov r1,#1
	push {r1 }
	ldr r1,[fp, #-16]
	pop {r2 }
	add r2,r1,r2
	str r2,[fp, #-16]
	mov r1,#0
	str r1,[fp, #-20]
	b .L9
.L10:
	ldr r1,[fp, #-24]
	mov r0,r1
	sub sp,fp,#8
	pop {r4, fp, pc}
	str r0,[fp, #-12]
	sub sp, fp, #8
	@ sp needed
	pop {r4,fp, pc}
	.text
	.align 2
	.global main
	.syntax unified
	.arm
	.fpu vfp
	.type main, %function
main:
	push {r4, fp, lr}
	add fp,sp,#8
	sub sp,sp,#8
	movw r0,#:lower16:a
	movt r0,#:upper16:a
	bl func
	bl putint
	mov r0,#0
	sub sp,fp,#8
	pop {r4, fp, pc}
	sub sp, fp, #8
	@ sp needed
	pop {r4,fp, pc}

