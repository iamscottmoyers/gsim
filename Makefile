gsim:
	cd navvie && make
	cd navvie && mkdir -p tmp && ./navvie ../aire/aire.uml ./tmp/ > out.dot && cd tmp && patch -p1 < ../../aire/aire_cpp.patch && make
	cd vhdl && make
	cd verilog && make

clean:
	cd verilog && make clean
	cd vhdl && make clean
	cd navvie && rm -f out.dot && cd tmp && make clean
	cd navvie && make clean
