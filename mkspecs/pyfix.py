import os

# patch the following
# QMAKE_CFLAGS add $(CFLAGS_EXTRA)
# QMAKE_CXXFLAGS add $(CXXFLAGS_EXTRA)
# QMAKE_LFLAGS add $(LFLAGS_EXTRA)
# QMAKE_LIBS add $(LIBS_EXTRA)

# QMAKE_CFLAGS_RELEASE include old argument
# 

add_map = { "QMAKE_CFLAGS"   : "$(CFLAGS_EXTRA)",
	    "QMAKE_CXXFLAGS" : "$(CXXFLAGS_EXTRA)",
	    "QMAKE_LFLAGS"   : "$(LFLAGS_EXTRA)",
	    "QMAKE_LIBS"     : "$(LIBS_EXTRA)" }
	    
rep_map = { "QMAKE_CFLAGS_RELEASE" : ["$(if $(CFLAGS_RELEASE),$(CFLAGS_RELEASE),", ")"] }


def split_text(line):
 """
 SPlits the text in the form of "key = value"
 and returns a two tuple-tuple
 """
 ## disabled strip and split tab stuff
 left = line.find('=')
 if left == -1:
    raise Exception, "No src line"
 key = line[0:left].strip()
 val = line[left+1:].strip()
 return (key,val)

def apply_line(line):
 """
 Apply changes to a line. This will not handle the exception
 thrown by split_text
 """
 key,val = split_text(line)

 tabs = '\t'
 if len(key) <= 14:
 	tabs = tabs + '\t'
 spac = ''
 if len(val) != 0:
 	spac = ' '

 # for minimal changes on the file we've three returns
 if add_map.has_key(key):
    val = val + spac + add_map[key]
    return key + tabs + '= ' + val + '\n'
 elif rep_map.has_key(key):
    val = rep_map[key][0] + ' ' + val + rep_map[key][1]
    return key + tabs + '= ' + val + '\n'
 return line



def apply_on_file(fi):
  """
  Apply the maps on files
  """
  lines = []
  for line in file(fi):    
     try:
     	line = apply_line(line)
     except:
        pass
     lines.append(line)

  f = file(fi,'w')
  for line in lines:
      f.write(line) 

def fix_it():
  for root, dirs, files in os.walk('./mkspecs'):
     for fil in files:
         print root + '/' + fil
	 apply_on_file(root+'/'+fil)


if __name__ == '__main__':
    fix_it()	
