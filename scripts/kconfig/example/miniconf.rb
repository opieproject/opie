require "kconfig"

include Kconfig

conf_parse("arch/i386/Kconfig")
conf_read(nil)

def conf(menu)
	return unless menu.isVisible?
	prompt = menu.prompt
	if prompt.type == P_COMMENT || prompt.type == P_MENU
		print "* #{prompt.text}\n"
	end
	sym = menu.sym
	if sym
		begin
			print "#{prompt.text} (#{sym.get_string})? "
			unless sym.isChangable?
				print "\n"
				break
			end
			val = gets.strip
		end until val.empty? || sym.set_string(val)
	end
	menu.each do |child|
		conf(child)
	end
end

conf(Kconfig.rootmenu)

conf_write(nil)
