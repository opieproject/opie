require "kconfig"

include Kconfig

conf_parse("arch/i386/Kconfig")
conf_read(nil)

sym = Kconfig::Symbol.find(ARGV[0])
if !sym
	print "Symbol #{ARGV[0]} not found!\n"
	exit
end

sym.calc_value
print "symbol: #{sym.name}\n"
print "  type: #{Kconfig::Symbol.type_name(sym.type)}\n"
print "  value: #{sym.get_string}\n"
print "  choice\n" if sym.isChoice?
print "  choice value\n" if sym.isChoiceValue?
print "  properties:\n" if sym.prop
sym.each do |prop|
	case prop.type
	when P_PROMPT
		print "    prompt: #{prop.text}\n"
	when P_DEFAULT
		prop.def.calc_value
		print "    default: #{prop.def.get_string}\n"
	when P_CHOICE
		print "    choice reference\n"
	else
		print "    unknown property: #{Property.type_name(prop.type)}\n"
	end
	print "      dep: #{prop.visible.expr}\n" if prop.visible.expr
end
