include ../.config

clean:
ifdef CONFIG_TARGET_MACOSX
	find ./ -name "*.dylib*" -print0 | xargs -0 rm -f
else
	find ./ -name "*.so*" -print0 | xargs -0 rm -f
endif

all install ipk lupdate lrelease opie-lupdate opie-lrelease:
