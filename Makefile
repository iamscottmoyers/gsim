gsim:
	cd navvie && make
	cd aire && mkdir -p build && ../navvie/navvie --read uml ./aire.uml --write cpp ./build/ --write graphviz aire.dot && cd build && patch -p1 < ../aire_cpp.patch && make
	cd vhdl && make
	cd verilog && make

clean:
	cd verilog && make clean
	cd vhdl && make clean
	cd aire && rm -f aire.dot && cd build && make clean
	cd navvie && make clean
