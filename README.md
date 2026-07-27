# Summary
An shellcode attack written in C on a RISC-V system that can execute arbitrary code. The attack itself is a completely UTF-8 compliant string and is initiated by a string buffer overflow attack. The attack consists of two parts: the RISC-V assembly that is UTF-8 compatible and loads the payload, and the executable payload itself, which is written in C and has no such UTF-8 requirements. 

# Collaborators:
Wei-Jet Chiew,
Cory Chang
