# Para revisar las opciones de compilacion y ejecucion,
# ingrese en el terminal el comando: make

ALUM=h2o.c
PROF=test-h2o.c pss.c
HDRS=h2o.h pss.h

NAME = $(patsubst %.c,%,$(ALUM))

SHELL=bash -o pipefail

all:
	echo $(OBJS)

INCLUDE=
CFLAGS=-Wall -Werror -pedantic -std=c18 $(INCLUDE)
LDLIBS=-pthread

MAK=make --no-print-directory

readme:
	@less README.txt

$(NAME).bin $(NAME).bin-g $(NAME).bin-san $(NAME).bin-mem: $(ALUM) $(PROF) $(HDRS)

run-mem: $(NAME).bin-mem
	./$(NAME).bin-mem

run-san: $(NAME).bin-san
	./$(NAME).bin-san

run-g: $(NAME).bin-g
	./$(NAME).bin-g

run: $(NAME).bin
	./$(NAME).bin

ddd: $(NAME).ddd

ddd-san: $(NAME).ddd-san

zip:
	@rm -f resultados.txt $(NAME).zip
	@echo "Sistema operativo utilizado" > resultados.txt
	@uname -a >> resultados.txt
	@cat resultados.txt        
	@echo ==== run ==== | tee -a resultados.txt
	@$(MAK) -B run | tee -a resultados.txt
	@echo ==== run-g ==== | tee -a resultados.txt
	@$(MAK) -B run-g | tee -a resultados.txt
	@echo ==== run-san ==== | tee -a resultados.txt
	@$(MAK) -B run-san | tee -a resultados.txt
	@echo ==== run-mem ==== | tee -a resultados.txt
	@$(MAK) -B run-mem | tee -a resultados.txt
	@echo ==== zip ====
	zip -r $(NAME).zip resultados.txt $(NAME).c
	@echo "Entregue por u-cursos el archivo $(NAME).zip"

%.bin: %.c
	gcc -O -DOPT=1 $(CFLAGS) $^ $(LDLIBS) -o $@

%.bin-g: %.c
	gcc -g $(CFLAGS) $^ $(LDLIBS) -o $@

%.bin-san: %.c
	gcc -g -DSAN=1 -fsanitize=thread -DSANITIZE $(CFLAGS) $^ $(LDLIBS) -o $@

%.bin-mem: %.c
	gcc -g -DSAN=1 -fsanitize=address -fsanitize=undefined -DSANITIZE $(CFLAGS) $^ $(LDLIBS) -o $@

%.ddd: %.bin-g
	ddd $(*F).bin-g &

%.ddd-san: %.bin-san
	ddd $(*F).bin-san &

clean:
	rm -f *.o *.log *.bin*
