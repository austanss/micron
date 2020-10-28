global setupPaging
setupPaging:
	; Fetch the pointer to the paging directory
	mov eax, [esp + 4]
	mov cr3, eax

	; Enable the page size extension
	mov eax, cr4
	or  eax, 0x10
	mov cr4, eax

	; Enable paging
	mov eax, cr0
	mov eax, $1
	or  eax, [1 << 0]
	or  eax, [1 << 31]
	mov cr0, eax

	ret
