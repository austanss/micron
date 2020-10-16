global flushIDT

flushIDT:
	mov eax, [esp+4]
	lidt [eax]
	ret
