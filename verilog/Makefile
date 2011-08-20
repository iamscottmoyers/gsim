verilog:
	java org.antlr.Tool *.g
	gcc -I. -m32 -g -I/usr/local/include/ -L/usr/local/lib/ -lantlr3c *.c -o verilog

clean:
	rm -f VerilogLexer.c VerilogLexer.h
	rm -f VerilogParser.c VerilogParser.h
	rm -f VerilogWalker.c VerilogWalker.h
	rm -f Verilog.tokens VerilogWalker.tokens
	rm -f verilog
	rm -rf verilog.dSYM
	rm -f *~
