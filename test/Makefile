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
