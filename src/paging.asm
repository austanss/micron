global setupPaging
setupPaging:
	; Fetch the pointer to the paging directory
	mov rax, [rsp + 4]
	mov cr3, rax

	; Enable the page size extension
	mov rax, cr4
	or  rax, 0x10
	mov cr4, rax

	; Enable paging
	mov rax, cr0
	mov rax, $1
	or  rax, [1 << 0]
	or  rax, [1 << 31]
	mov cr0, rax

	ret