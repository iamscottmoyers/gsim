grammar vhdl;
options
{
    output = AST;
    language = C;
    ASTLabelType = pANTLR3_BASE_TREE;
}

tokens
{
    NONE;
}

@members
{
    #include <ctype.h>

    static unsigned int strcmp_i(const char *a, const char *b)
    {
        while(*a && *b)
        {
            if( toupper(*a) != toupper(*b) )
            {
                return 1;
            }
            a++;
            b++;
        }
        return 0;
    }
}

design_file : design_unit+ ;
design_unit : library_unit ;
library_unit : primary_unit | secondary_unit ;
primary_unit : entity_declaration ;
secondary_unit : architecture_body ;
architecture_body : ARCHITECTURE i=identifier OF simple_name IS architecture_declarative_part
                        BEGIN architecture_statement_part END ARCHITECTURE?
                        (s=simple_name
                        {
                            if(strcmp_i($s.text->chars, $i.text->chars))
                            {
                                /* TODO: should be overriding mismatch and displayRecognitionError */
                                printf("error, your architecture names don't match ");
                                printf( "('\%s' line \%d and '\%s' line \%d)\n",
                                        $i.text->chars, $i.start->line,
                                        $s.text->chars, $s.start->line );
                                exit(0);
                            }
                        })? ';' -> ^(ARCHITECTURE identifier simple_name);

architecture_declarative_part : /* TODO */ ;
architecture_statement_part : /* TODO */ ;

entity_declaration : ENTITY i=identifier IS entity_header entity_declarative_part
                        (BEGIN entity_statement_part)? END ENTITY?
                        (s=simple_name
                        {
                            if(strcmp_i($s.text->chars, $i.text->chars))
                            {
                                /* TODO: should be overriding mismatch and displayRecognitionError */
                                printf("error, your entity names don't match ");
                                printf( "('\%s' line \%d and '\%s' line \%d)\n",
                                        $i.text->chars, $i.start->line,
                                        $s.text->chars, $s.start->line );
                                exit(0);
                            }
                        })? ';' -> ^(ENTITY identifier entity_header) ;

entity_header : port_clause? ;
port_clause : PORT '(' interface_list ')' ';' -> ^(PORT interface_list) ;
interface_list : interface_element ( ';'! interface_element )* ;
interface_element : identifier ':' mode? simple_name -> ^(identifier mode? simple_name);

entity_declarative_part : /* TODO */;
entity_statement_part : /* TODO */;

simple_name : identifier ;
identifier : ID ;
mode : IN | OUT | INOUT | BUFFER | LINKAGE ;

ENTITY : ('e'|'E')('n'|'N')('t'|'T')('i'|'I')('t'|'T')('y'|'Y') ;
ARCHITECTURE : ('a'|'A')('r'|'R')('c'|'C')('h'|'H')('i'|'I')('t'|'T')('e'|'E')('c'|'C')('t'|'T')('u'|'U')('r'|'R')('e'|'E') ;
IS : ('i'|'I')('s'|'S') ;
OF : ('o'|'O')('f'|'F') ;
BEGIN : ('b'|'B')('e'|'E')('g'|'G')('i'|'I')('n'|'N') ;
END : ('e'|'E')('n'|'N')('d'|'D') ;
PORT : ('p'|'P')('o'|'O')('r'|'R')('t'|'T') ;
SIGNAL : ('s'|'S')('i'|'I')('g'|'G')('n'|'N')('a'|'A')('l'|'L') ;
IN : ('i'|'I')('n'|'N') ;
OUT : ('o'|'O')('u'|'U')('t'|'T') ;
INOUT : ('i'|'I')('n'|'N')('o'|'O')('u'|'U')('t'|'T') ;
BUFFER : ('b'|'B')('u'|'U')('f'|'F')('e'|'E')('r'|'R') ;
LINKAGE : ('l'|'L')('i'|'I')('n'|'N')('k'|'K')('a'|'A')('g'|'G')('e'|'E') ;
ID : ('a'..'z'|'A'..'Z') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')* ;
WS : (' '|'\r'|'\t'|'\u000C'|'\n') {$channel=HIDDEN;} ;
LINE_COMMENT : '--' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;} ;
