TOPTARGETS := all clean

SUBDIRS := lex parse gen ast intergration

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
		$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

.PHONY: run
run:
	./lex/lex_test
	./parse/parse_test
	./gen/gen_test
	./ast/ast_test
	./intergration/intergration_test
mem_test:
	valgrind ./lex/lex_test
	valgrind ./parse/parse_test
	valgrind ./gen/gen_test
	valgrind ./ast/ast_test
	valgrind ./intergration/intergration_test
