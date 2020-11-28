global loadIDT64

loadIDT64:
	mov rax, [rsp+4]
	lidt [rax]
	ret
