CXX := gcc
WARNINGFLAGS := -Wall -Wc++-compat -Wextra -Wno-pointer-sign --pedantic -ansi
CXXFLAGS := $(WARNINGFLAGS) -g `xml2-config --cflags`
#CXXFLAGS := $(WARNINGFLAGS) -O2 -DNDEBUG -I/usr/include/libxml2
#CXXFLAGS := $(WARNINGFLAGS) -pg -O3 -g -I/usr/include/libxml2

LIBS := `xml2-config --libs`
OBJDIR := obj
INCDIR := inc
SOURCEDIR := src
INCLUDE := -I $(INCDIR)
OBJECTS := $(OBJDIR)/main.o \
$(OBJDIR)/xml_reader.o \
$(OBJDIR)/c_writer.o \
$(OBJDIR)/cpp_writer.o \
$(OBJDIR)/graphviz_writer.o \
$(OBJDIR)/uml_package.o \
$(OBJDIR)/uml_model.o \
$(OBJDIR)/uml_class.o \
$(OBJDIR)/uml_datatype.o \
$(OBJDIR)/uml_attribute.o \
$(OBJDIR)/uml_operation.o \
$(OBJDIR)/uml_enumeration.o \
$(OBJDIR)/uml_literal.o \
$(OBJDIR)/uml_primitivetype.o \
$(OBJDIR)/uml_association.o \
$(OBJDIR)/uml_ownedend.o \
$(OBJDIR)/uml_parameter.o \
$(OBJDIR)/uml_comment.o \
$(OBJDIR)/uml_constraint.o \
$(OBJDIR)/uml_element.o \
$(OBJDIR)/uml_type.o \
$(OBJDIR)/list.o \
$(OBJDIR)/util.o \
$(OBJDIR)/hash-table.o \
$(OBJDIR)/hash-string.o \
$(OBJDIR)/compare-string.o \
$(OBJDIR)/hash-pointer.o \
$(OBJDIR)/compare-pointer.o

all : navvie

navvie : makeobjdir $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

makeobjdir :
	mkdir -p obj

$(OBJDIR)/%.o : $(SOURCEDIR)/%.c
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean :
	rm -rf $(OBJDIR)
	rm -f navvie
	rm -f gmon.out

ultraclean : clean
	rm -f $(INCDIR)/*~
	rm -f $(SOURCEDIR)/*~
	rm -f *~
	rm -f .DS_Store
	rm -f $(INCDIR)/.DS_Store
	rm -f $(SOURCEDIR)/.DS_Store
