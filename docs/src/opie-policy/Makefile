DOC = opie-policy
DOCSDIR = $(OPIEDIR)/docs
HTMLDOCSDIR = $(DOCSDIR)/html
SRCDOCSDIR = $(DOCSDIR)/src
INDEX = $(HTMLDOCSDIR)/$(DOC)/index.html
FDL = $(SRCDOCSDIR)/$(DOC)/gfdl/fdl.sgml
XML = $(SRCDOCSDIR)/$(DOC)/$(DOC).xml
DOCBOOK_HTMLCNV = docbook2html

all: $(INDEX)

distclean: clean

clean:
	-rm -rf $(HTMLDOCSDIR)/$(DOC);

$(INDEX): $(FDL) $(HTMLDOCSDIR)/$(DOC) $(XML)
	$(DOCBOOK_HTMLCNV) $(XML) -o $(HTMLDOCSDIR)/$(DOC);

$(HTMLDOCSDIR)/$(DOC):
	-mkdir $(HTMLDOCSDIR)/$(DOC);
