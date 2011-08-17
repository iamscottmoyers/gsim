tree grammar vhdlWalker;
options {
    tokenVocab	    = vhdl;
    ASTLabelType    = pANTLR3_BASE_TREE;
    language	    = C;
}

design_file : design_unit+ ;
design_unit : library_unit ;
library_unit : primary_unit | secondary_unit ;
primary_unit : entity_declaration ;
secondary_unit : architecture_body ;
architecture_body : ^(ARCHITECTURE identifier simple_name) ;
entity_declaration: ^(ENTITY identifier entity_header) ;
entity_header : port_clause? ;
port_clause : ^(PORT interface_list) ;
interface_list : interface_element+ ;
interface_element : ^(identifier mode? simple_name) ;

simple_name : identifier ;
identifier: ID ;
mode : IN | OUT | INOUT | BUFFER | LINKAGE ;

