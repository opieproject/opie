all: html

DOC := usermanual

html:
	sgmltools -b html -s stylesheet-html.dsl $(DOC).sgm
	mv -f $(DOC)/*.html .
	rmdir $(DOC)
	ln -sf book1.html index.html

clean:
	rm -f *.html
