# Platform compiler (for utilities)
CC := gcc
CWARN := -Wall -Wextra -Wpedantic
CFLAGS := -O2 -g $(CWARN)
LDFLAGS := 

# RISC-V (cross-)compiler
RVCC := gcc
RVCFLAGS := -g
RVLD := ld
RVLDFLAGS := -N -z execstack --no-warn-rwx-segments

.PHONY: clean default

default: util/dump util/check encode-utf8 encode-nonul

#
# rules for building the (native) utility programs
#

util/dump: util/dump.o
	$(CC) util/dump.o $(LDFLAGS) -lelf -o util/dump 

util/dump.o: util/dump.c
	$(CC) $(CFLAGS) -c util/dump.c -o util/dump.o

util/check: util/check.o util/utf8proc.o
	$(CC) util/check.o util/utf8proc.o $(LDFLAGS) -o util/check

util/utf8proc.o: util/utf8proc.c util/utf8proc.h util/utf8proc_data.c
	$(CC) $(CFLAGS) -c util/utf8proc.c -o util/utf8proc.o

util/check.o: util/check.c util/utf8proc.h
	$(CC) $(CFLAGS) -c util/check.c -lelf -o util/check.o

#
# rules for building the (native) encoders
#

encode-utf8: encode-utf8.o
	$(CC) encode-utf8.c $(LDFLAGS) -o encode-utf8

encode-utf8.o: encode-utf8.c
	$(CC) $(CFLAGS) -c encode-utf8.c -o encode-utf8.o

encode-nonul: encode-nonul.o
	$(CC) encode-nonul.c $(LDFLAGS) -o encode-nonul

encode-nonul.o: encode-nonul.c
	$(CC) $(CFLAGS) -c encode-nonul.c -o encode-nonul.o


#
# pattern rules for building (RISC-V) first-stage payloads
#

.PRECIOUS: payloads/%.raw payloads/%.bin

payloads/%.raw: payloads/%.bin util/dump
	util/dump $< $@

payloads/%.bin: payloads/%.o
	$(RVLD) $(RVLDFLAGS) $< -o $@

payloads/%.o: payloads/%.s
	$(RVCC) $(RVCFLAGS) -c $< -o $@

#
# pattern rules for building (RISC-V) UTF-8 shellcode
#

.PRECIOUS: utf8/%.s utf8/%.raw utf8/%.bin

utf8/%.raw: utf8/%.bin util/dump util/check
	util/dump $< $@
	-util/check -u $@

utf8/%.bin: utf8/%.o
	$(RVLD) $(RVLDFLAGS) $< -o $@

utf8/%.o: utf8/%.s
	$(RVCC) $(RVCFLAGS) -c $< -o $@

utf8/%.s: payloads/%.raw encode-utf8 utf8-template.s template.sed
	./encode-utf8 $< | sed -f template.sed utf8-template.s > $@

#
# pattern rules for building (RISC-V) shellcode (with no NUL bytes)
#

.PRECIOUS: nonul/%.s nonul/%.raw nonul/%.bin

nonul/%.raw: nonul/%.bin util/dump util/check
	util/dump $< $@
	-util/check $@

nonul/%.bin: nonul/%.o
	$(RVLD) $(RVLDFLAGS) $< -o $@

nonul/%.o: nonul/%.s
	$(RVCC) $(RVCFLAGS) -c $< -o $@

nonul/%.s: payloads/%.raw encode-nonul nonul-template.s template.sed
	./encode-nonul $< | sed -f template.sed nonul-template.s > $@
