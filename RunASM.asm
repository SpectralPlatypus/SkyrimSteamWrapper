.data
extern PA : qword
.code
RunASM proc
mov [PA],rax
test rax,rax
je skip
jmp qword ptr rax
skip:
ret
RunASM endp
end
