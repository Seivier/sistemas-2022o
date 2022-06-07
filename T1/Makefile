# Para revisar las opciones de compilacion y ejecucion,
# ingrese en el terminal el comando: make

SHELL=bash -o pipefail

INCLUDE=
CFLAGS=-Wall -Werror -pedantic -std=c18 $(INCLUDE)
LDLIBS=-pthread

MAK=make --no-print-directory

readme:
	@less README.txt

sat.bin sat.bin-g sat.bin-san: sat.c test-sat.c sat.h

run-san: sat.bin-san
	./sat.bin-san

run-g: sat.bin-g
	./sat.bin-g

run: sat.bin
	./sat.bin

ddd: sat.ddd

ddd-san: sat.ddd-san

zip:
	@rm -f resultados.txt sat.zip
	@echo "Sistema operativo utilizado"
	@uname -a > resultados.txt
	@echo ==== run ==== | tee -a resultados.txt
	@$(MAK) -B run | tee -a resultados.txt
	@echo ==== run-g ==== | tee -a resultados.txt
	@$(MAK) -B run-g | tee -a resultados.txt
	@echo ==== run-san ==== | tee -a resultados.txt
	@$(MAK) -B run-san | tee -a resultados.txt
	@echo ==== zip ====
	zip -r sat.zip resultados.txt sat.c
	@echo "Entregue por u-cursos el archivo sat.zip"

%.bin: %.c
	gcc -O -DOPT=1 $(CFLAGS) $^ $(LDLIBS) -o $@

%.bin-g: %.c
	gcc -g $(CFLAGS) $^ $(LDLIBS) -o $@

%.bin-san: %.c
	gcc -g -DSAN=1 -fsanitize=address -DSANITIZE $(CFLAGS) $^ $(LDLIBS) -o $@

%.ddd: %.bin-g
	ddd $(*F).bin-g &

%.ddd-san: %.bin-san
	ddd $(*F).bin-san &

clean:
	rm -f *.o *.log *.bin*
