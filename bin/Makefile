clean:
	@for i in *; do \
		test "$$i" == "CVS" && continue; \
		test "$$i" == "Makefile" && continue; \
		test "$$i" == "README" && continue; \
		test "$$i" == "getprofile" && continue; \
		test "$$i" == "opie-reorgfiles" && continue; \
		test "$$i" == "opie-sh-howto.sh" && continue; \
		test "$$i" == "qpe-reorgfiles" && continue; \
		rm $$i; \
	done

all install ipk lupdate lrelease opie-lupdate opie-lrelease:
