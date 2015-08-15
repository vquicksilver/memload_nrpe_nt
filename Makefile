CC=x86_64-w64-mingw32-gcc

memload_nrpe_nt.exe: memload_nrpe_nt.c
	$(CC) -Wall -std=c99 memload_nrpe_nt.c -o memload_nrpe_nt.exe

clean:
	rm -f memload_nrpe_nt.exe
