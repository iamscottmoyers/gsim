vhdl:
	java org.antlr.Tool *.g
	gcc -I. -m32 -I/usr/local/include/ -L/usr/local/lib/ -lantlr3c *.c -o vhdl

clean:
	rm -f vhdlLexer.c vhdlLexer.h
	rm -f vhdlParser.c vhdlParser.h
	rm -f vhdlWalker.c vhdlWalker.h
	rm -f vhdl.tokens vhdlWalker.tokens
	rm -f vhdl
	rm -rf vhdl.dSYM
	rm -f *~
