TOPTARGETS := all clean

SUBDIRS := lex parse

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
		$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

.PHONY: run
run:
	./lex/lex_test
	./parse/parse_test

