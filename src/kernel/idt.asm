global flushIDT

loadIDT64:
	mov eax, [esp+4]
	lidt [eax]
	ret
