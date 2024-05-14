CC         := gcc
LINKER     := $(CC)
CFLAGS     := -O2 -Wall -msse3 -march=haswell
LDFLAGS    := -lm

all:
	make run_all
	make plot_all

run:
	$(CC) $(CFLAGS) $(VERSION).c
	echo "version = '$(VERSION)';" > output_$(VERSION).m
	echo "Cholesky = [" >> output_$(VERSION).m
	./a.out >> output_$(VERSION).m
	echo "];" >> output_$(VERSION).m

run_all:
	./run_all.sh

plot_all:
	./output_to_csv.sh
	./plot_all.py

do_all:
	make run_all
	make plot_all

clean:
	rm -f *.o *~ core *.x *.out

cleanall:
	rm -f *.o *~ core *.x output*.m *.eps *.png *.csv *.out