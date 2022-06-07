
SHELL=bash -o pipefail

BIN=npub.bin-mem
NAME=$(patsubst %.bin,%,$(patsubst %.bin-g,%,$(patsubst %.bin-san,%,$(patsubst %.bin-mem,%,$(BIN)))))
SRCS=$(NAME).c
XTRA=

INCLUDE= -InKernel
CFLAGS= $(XTRA) -DNTHREADS -Wall -Werror -pedantic -Wno-unused-function -Wno-unused-variable -std=c18 $(INCLUDE)
LDLIBS= -lm -lrt -lpthread

MAK=make --no-print-directory

CFLAGS_PTH= -Wall -Werror -pedantic -std=c18 $(INCLUDE)

ARCH=amd64
NTHSRC=nKernel/nthread.c nKernel/nutil.c nKernel/ntime.c nKernel/pss.c \
       nKernel/sched-fcfs.c nKernel/sched-rr.c \
       nKernel/sched-pri-1core.c nKernel/sched-fcfs-1core.c \
       nKernel/sem.c nKernel/mutex-cond.c nKernel/nmsgs.c \
       nKernel/nStack-$(ARCH).s nKernel/nsystem.c nKernel/npub.c
NTHHDR=nKernel/nthread.h nKernel/nthread-impl.h nKernel/nSystem.h

PTHSRC=nKernel/pss.c
PTHHDR=nKernel/pss.h

all:
	less README.txt
run:
	@$(MAK) XTRA=-DNOINI test-pub.bin
	./test-pub.bin

run-g:
	@$(MAK) test-pub.bin-g
	./test-pub.bin-g

run-mem:
	@$(MAK) test-pub.bin-mem
	./test-pub.bin-mem

run-san:
	@$(MAK) test-pub.bin-san
	./test-pub.bin-san

run-tricore:
	@$(MAK) test-pub.bin
	./test-pub.bin -slice 100 -ncores 3

run-tricore-g:
	@$(MAK) test-pub.bin-g
	./test-pub.bin-g -slice 100 -ncores 3

run-tricore-mem:
	@$(MAK) test-pub.bin-mem
	./test-pub.bin-mem -slice 100 -ncores 3

run-tricore-san:
	@$(MAK) test-pub.bin-san
	./test-pub.bin-san -slice 100 -ncores 3

run-ddd:
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	@$(MAK) XTRA=-DNOINI test-pub.bin-g
	ddd --command launch.gdb test-pub.bin-g &
	gdbserver :2345 test-pub.bin-g

run-ddd-tricore:
	@gdbserver --version >& /dev/null || ( echo "Instale gdbserver con: sudo apt-get install gdbserver"; false)
	@$(MAK) XTRA=-DNOINI test-pub.bin-g
	ddd --command launch.gdb test-pub.bin-g &
	gdbserver :2345 test-pub.bin-g -ncores 3 -slice 100

zip:
	@rm -f resultados.txt $(NAME).zip
	@echo "Sistema operativo utilizado" > resultados.txt
	@uname -a >> resultados.txt
	@cat resultados.txt        
	@echo ==== run ==== | tee -a resultados.txt
	@$(MAK) -B run | tee -a resultados.txt
	@echo ==== run-g ==== | tee -a resultados.txt
	@$(MAK) -B run-g | tee -a resultados.txt
	@echo ==== run-mem ==== | tee -a resultados.txt
	@$(MAK) -B run-mem | tee -a resultados.txt
	@echo ==== run-tricore ==== | tee -a resultados.txt
	@$(MAK) -B run-tricore | tee -a resultados.txt || true
	@echo ==== run-tricore-g ==== | tee -a resultados.txt
	@$(MAK) -B run-tricore-g | tee -a resultados.txt || true
	@echo ==== run-tricore-mem ==== | tee -a resultados.txt
	@$(MAK) -B run-tricore-mem | tee -a resultados.txt || true
	@echo ==== zip ====
	zip -r $(NAME).zip resultados.txt nKernel/$(NAME).c
	@echo "Entregue por u-cursos el archivo $(NAME).zip"

#all:
#	@echo BIN=$(BIN) NAME=$(NAME) SRCS=$(SRCS)

$(NAME).bin $(BIN).bin-g $(BIN).bin-san $(BIN).bin-mem: $(NTHHDR)

$(NAME).bin-g: $(SRCS) $(NTHSRC) 
	gcc -g $(CFLAGS) $(SRCS) $(NTHSRC) $(LDLIBS) -o $@

$(NAME).bin: $(SRCS) $(NTHSRC)
	gcc -O2 -DOPT -g $(CFLAGS) $(SRCS) $(NTHSRC) $(LDLIBS) -o $@

$(NAME).bin-san: $(SRCS) $(NTHSRC)
	gcc -g -fsanitize=thread -DSANITIZE $(CFLAGS) $(SRCS) $(NTHSRC) $(LDLIBS) -o $@

$(NAME).bin-mem: $(SRCS) $(NTHSRC)
	gcc -g -fsanitize=address -fsanitize=undefined -DSANITIZE $(CFLAGS) $(SRCS) $(NTHSRC) $(LDLIBS) -o $@

$(SRC).pth-g $(SRC).pth $(SRC).pth-san $(SRC).pth-mem: $(PTHHDR)

$(SRC).pth-g: $(SRC).c $(PTHSRC)
	gcc -g $(CFLAGS_PTH) $^ $(LDLIBS) -o $@

$(SRC).pth: $(SRC).c
	gcc -O2 -DOPT $(CFLAGS_PTH) $^ $(LDLIBS) -o $@

%.bin: FORCE
	@$(MAK) BIN=$@ $@

%.bin-g: FORCE
	@$(MAK) BIN=$@ $@

%.bin-mem: FORCE
	@$(MAK) BIN=$@ $@

%.bin-san: FORCE
	@$(MAK) BIN=$@ $@

FORCE:

clean:
	rm -f *.o *.bin* *.pth* core
