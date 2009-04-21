#!/usr/bin/python

from struct import unpack, pack

import os, os.path
import sys
import string

fun_dict = {}
buf_dict = {}
ctrl_dict = {}
io_dict = {}
var_dict = {}

def hexdump(s,sep=" "):
        return sep.join(map(lambda x: "%02x"%ord(x),s))

def ascii(s):
        s2 = ""
        for c in s:
                if ord(c)<0x20 or ord(c)>0x7e:
                        s2 += "."
                else:
                        s2 += c
        return s2

def pad(s,c,l):
	if len(s)<l:
		s += c * (l-len(s))
	return s

def chexdump(s):
	for i in range(0,len(s),16):
		print >>outf,  "%08x  %s  %s  |%s|"%(i,pad(hexdump(s[i:i+8],' ')," ",23),pad(hexdump(s[i+8:i+16],' ')," ",23),pad(ascii(s[i:i+16])," ",16))

def chexdumpstr(s):
	retval = ""
	for i in range(0,len(s),16):
		retval +=  "\n%08x  %s  %s  |%s|"%(i,pad(hexdump(s[i:i+8],' ')," ",23),pad(hexdump(s[i+8:i+16],' ')," ",23),pad(ascii(s[i:i+16])," ",16))
	return retval

def readpstring(f):
	strlen = unpack("B", f.read(1))[0]
	return f.read(strlen)

def getpstring(d):
	strlen = unpack("B", d[0])[0]
#	print >>outf,  "strlen = ",strlen
	return d[1:(strlen+1)]
	
class TResourceItem:
	def __init__(self, data):
#		chexdump(data[0:8])
		(self.posn, self.size) = unpack("<II", data[0:8])
		self.name = getpstring(data[8:])
		print >>outf,  "TResourceItem(posn=0x%x,size=0x%x,name='%s')" % (self.posn, self.size, self.name)

	def __str__(self):
		return "TResourceItem(posn=0x%x,size=0x%x,name='%s')" % (self.posn, self.size, self.name)
	
	def get_item(self, f):
		f.seek(self.posn)
		return f.read(self.size)


class TResourceCollection:
	def __init__(self, data):
		self.data = data
	
	def get_resources(self):
		num_resources, num2, num3 = unpack("<III", self.data[22:34])
		print >>outf,  "TResourceCollection(%d,%x,%x)" % (num_resources, num2, num3)
		retval = []
		ptr = 34
#		chexdump(self.data[ptr:])
		for i in range(num_resources):
			item = TResourceItem(self.data[ptr:])
			ptr += len(item.name)+9
			retval.append(item)
		
		return retval

class TMemDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])
		
	def __str__(self):
		return "TMemDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TLogDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])

	def __str__(self):
		return "TLogDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TPofFuseDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])

	def __str__(self):
		return "TPofFuseDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TPofDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])

	def __str__(self):
		return "TPofDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TXPMemDev:
	def __init__(self, data):
		self.data = data[11:]
		self.famcode, self.pinmap, self.a, self.b, self.size = unpack("<HHHBI", data[0:11])
		
	def __str__(self):
		return "TXPMemDev: famcode=%04x pinmap=%04x %04x %02x %08x %s" %(self.famcode, self.pinmap, self.a, self.b, self.size, hexdump(self.data))

class TXilinxMemDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])

	def __str__(self):
		return "TXilinxMemDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TDblBufMemDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])

	def __str__(self):
		return "TDblBufMemDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TAddTabMemDev:
	def __init__(self, data):
		self.data = data[8:]
		self.famcode, self.pinmap, self.a, self.b = unpack("<HHHH", data[0:8])

	def __str__(self):
		return "TAddTabMemDev: famcode=%04x pinmap=%04x %04x %04x %s" %(self.famcode, self.pinmap, self.a, self.b, hexdump(self.data))

class TDNCol:
	def __init__(self, data):
		self.num_elements, self.x, self.y = unpack("<III", data[0:12])
		self.elements = []
		ptr = 12
		for i in range(self.num_elements):
			element = parse_item(data[ptr:])
			self.elements.append(element)
			ptr += len(element.name)+10

	def __str__(self):
		return "TDNCol: %d %d %d %s" % (self.num_elements, self.x, self.y, string.join(map(str,self.elements), ', '))

class TDN:
	def __init__(self, data):
		self.name = getpstring(data)
		self.id = unpack("<B", data[len(self.name)+1:len(self.name)+2])[0]
#		print >>outf,  "TDN: %s %d" % (self.name, self.id)
		if self.name[0:4] == '_BUF': buf_dict[self.id] = self.name
		if self.name[0:4] == '_FUN': fun_dict[self.id] = self.name
		if self.name[0:4] == '_CTR': ctrl_dict[self.id] = self.name
		if self.name[0:4] == '_IO_': io_dict[self.id] = self.name
		if self.name[0:4] == '_VAR': var_dict[self.id] = self.name
		
	def __str__(self):
#		return "TDN: %s %d" % (self.name, self.id)
#	
#	def pretty(self):
		return "[%s %x]\n" % (self.name, self.id)

class TKernel:
	def __init__(self, data):
		self.data = data
		
#		chexdump(data)
		
	def __str__(self):
		retval = "TKernel: "
		data = self.data
		while len(data) >= 4:
			(id, wave) = unpack("<HH", data[0:4])
			retval += "%04x %04x " % (id, wave)
			retval += "\n"
			data = data[4:]
		return retval

class TXilinxAddrData:
	def __init__(self, data):
		self.data = data
#		chexdump(data)

	def __str__(self):
		return "TXilinxAddrData: " + chexdumpstr(self.data)

class TFuseCode:
	def __init__(self, data):
		self.data = data
#		chexdump(data)

	def __str__(self):
		return "TFuseCode: " + chexdumpstr(self.data)

class TFamCode:
	def __init__(self, data):
		self.data = data
#		chexdump(data)

	def __str__(self):
		retval = "TFamCode: %02x " % (ord(self.data[0]))
		data = self.data[6:]
		while len(data) >= 4:
			(id, wave) = unpack("<HH", data[0:4])
			retval += "%04x %04x " % (id, wave)
			if id in fun_dict: retval += fun_dict[id]
			retval += "\n"
			data = data[4:]
#		return "TFamCode: " + chexdumpstr(self.data)
		return retval

class TPinCode:
	def __init__(self, data):
		self.num_pins = ord(data[0])
		self.data = data[1:]
#		chexdump(data)

	def __str__(self):
		return "TPinCode: %d pins, %s" % (self.num_pins, chexdumpstr(self.data))

class TPOFCoreCode:
	def __init__(self, data):
		self.data = data
#		chexdump(data)

	def __str__(self):
		return "TPOFCoreCode: " + chexdumpstr(self.data)

class TWaveCode:
	def __init__(self, data):
		self.data = data[4:]
#		chexdump(data)
		self.type = unpack("<I", data[0:4])[0]
		if self.type == 6:
			self.len1, self.addr, self.len2 = unpack("<HHH", data[4:10])
			self.data = data[10:]
			
	def __str__(self):
		if self.type == 6:
			
			return "TWaveCode: type = 6 (805x) len1 = %04x addr=%04x len2 = %04x data = %s" % (self.len1, self.addr, self.len2, chexdumpstr(self.data))
#		if self.type == 4:
#			return "TWaveCode: type = 4 (XC3030 bitstream) len = %d" % (self.len)
		return "TWaveCode: type = %x data = %s" % (self.type, chexdumpstr(self.data))
		
class TVersion:
	def __init__(self, data):
#		chexdump(data)
		self.version = unpack("<B", data[0])

	def __str__(self):
		return "TVersion: %d" % self.version

class TVarName:
	def __init__(self, data):
		num_vars = unpack("<H", data[0:2])[0]
		ptr = 2
		self.vars = []
		for i in range(num_vars):
			element = getpstring(data[ptr:])
#			print >>outf,  "var = " + element
			self.vars.append(element)
			ptr += len(element)+1
#		chexdump(data)

	def __str__(self):
		return "TVarName: " + string.join(self.vars, ', ')

def parse_item(data):
	preamble = unpack(">H", data[0:2])[0]
	if preamble != 0x25b:
		print >>outf,  "Expected 0x25b for preamble, got"
		chexdump(data[0:16])
		exit()
		
	type_name = getpstring(data[2:])
	body = data[3+len(type_name):-1]
#	print >>outf,  "Type name = ", type_name
	if type_name == "TMemDev": return TMemDev(body)
	if type_name == "TLogDev": return TLogDev(body)
	if type_name == "TPofDev": return TPofDev(body)
	if type_name == "TPofFuseDev": return TPofFuseDev(body)
	if type_name == "TXPMemDev": return TXPMemDev(body)
	if type_name == "TXilinxMemDev": return TXilinxMemDev(body)
	if type_name == "TXilinxAddrData": return TXilinxAddrData(body)
	if type_name == "TDblBufMemDev": return TDblBufMemDev(body)
	if type_name == "TAddTabMemDev": return TAddTabMemDev(body)
	if type_name == "TDevPool": return TDevPool(body)
	
	if type_name == "TDNCol": return TDNCol(body)
	if type_name == "TDN": return TDN(body)
	if type_name == "TKernel": return TKernel(body)
	if type_name == "TFamCode": return TFamCode(body)
	if type_name == "TFuseCode": return TFuseCode(body)
	if type_name == "TPOFCoreCode": return TPOFCoreCode(body)
	if type_name == "TPinCode": return TPinCode(body)
	if type_name == "TWaveCode": return TWaveCode(body)
	if type_name == "TVersion": return TVersion(body)
	if type_name == "TVarName": return TVarName(body)
	print >>outf,  "Unknown type " + type_name
	
def print_data(indent, f):
	data = f.read(2)
	if len(data) != 2:
		return False
	preamble = unpack(">H", data)[0]
	type_name = readpstring(f)
	if preamble != 0x25b:
		print >>outf,  indent, "preamble = %x" % (preamble)
#	print >>outf,  type_name
	if type_name == "TVendorCollection":
		num1, num2, num3 = unpack("<III", f.read(12))
		print >>outf,  indent, "%s(%d, %d, %d):" % (type_name, num1, num2, num3)

		for i in range(num1):
			print >>outf, _data(indent + "   ", f)
		f.read(5)
		return True
	if type_name == "TVendor":
		vendor_name = readpstring(f)
		postamble = f.read(1)
		print >>outf,  indent, "%s: %s" % (type_name, vendor_name)
		return True
	if type_name == "TDevPool":
		num_devices, num2 = unpack("<HH", f.read(4))
#		print >>outf,  indent, num_devices
		devices = []
		for i in range(num_devices):
			devices.append([unpack("<H", f.read(2))[0]])
#			print >>outf,  indent, "value: %x" % unpack("<H", f.read(2))
#		print >>outf,  indent, "--"
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		for i in range(num_devices):
			devices[i].append(unpack("<H", f.read(2))[0])
		i = 0
		for name in f.read(num2).split("\x00"):
			if i < num_devices:
				devices[i].append(name)
				print >>outf,  type_name+": %04x %04x %04x %02x %04x %04x %02x %04x %02x %02x %s" % tuple(devices[i])
				i = i + 1
#		print >>outf,  chexdump(f.read(num2))
		f.read(1)
		return True
	if type_name == "TResourceCollection":
		num_resources, num2, num3 = unpack("<III", f.read(12))
		print >>outf,  indent, num_resources
		print >>outf,  indent, "%s(%d,%x,%x)" % (type_name, num_resources, num2, num3)
		for i in range(num_resources):
			posn, size = unpack("<II", f.read(8))
			name = readpstring(f)
			print >>outf,  indent, "%-20s posn=%8x size=%8x" % (name, posn, size)
		f.read(5)
		return True
	if type_name == "TVersion":
		print >>outf,  indent, "Version = %x" % (unpack("<B", f.read(1))[0])
		f.read(2)
		return True
	if type_name == "TVarName":
		num_vars = unpack("<H", f.read(2))[0]
		for i in range(num_vars):
			print >>outf,  "var = " + readpstring(f)
		f.read(1)
		return True
	if type_name == "TWaveCode":
		num1, codesize = unpack("<IH", f.read(6))
		print >>outf,  indent, "WaveCode = type:%x len:%x" % (num1, codesize)
		chexdump(f.read(codesize))
		f.read(1)
		return True
	if type_name == "TPinCode":
		size, foo, bar, baz, bazz, buzz = unpack("<BBHHHB", f.read(9))
		print >>outf,  indent, "size = %x foo=%d bar=%d baz=%d bazz=%x buzz=%x" % (size, foo, bar, baz, bazz, buzz)
#		for i in range(size+foo+bar*2+20):
#			print >>outf,  indent, "%x" % unpack("<B", f.read(1))
		buf = ""
		while True:
			char = f.read(1)
			if char == ']': break
			buf += char
		chexdump(buf)
#		f.read(2)
		return True
	if type_name == "TFamCode":
		num1, codesize = unpack("<HH", f.read(4))
		print >>outf,  indent, "FamCode = type:%x len:%x" % (num1, codesize)
#		chexdump(f.read(num1*4+codesize*10+6))
		buf = ""
		while True:
			char = f.read(1)
			if char == ']' and len(buf) > 0x100: break
			buf += char
		chexdump(buf)
		return True
	if type_name == "TMemDev":
		print >>outf,  indent, "MemDev:"
		chexdump(f.read(17))
		f.read(1)
		return True
	if type_name == "TKernel":
		print >>outf,  indent, "Kernel:"
		chexdump(f.read(240))
		f.read(1)
		return True
	if type_name == "TDNCol":
		num = unpack("<I", f.read(4))[0]
		print >>outf,  indent, "DNCol: %d" % (num)
		chexdump(f.read(7))
		f.read(1)
		for i in range(num):
			print >>outf, _data(indent + "   ", f)
		f.read(5)
		return True
	if type_name == "TDN":
		name = readpstring(f)
		num1 = unpack("<B", f.read(1))[0]
		print >>outf,  indent, "DN: %s %x" % (name, num1)
		
#		chexdump(f.read(13))
		f.read(2)
#		print >>outf, _data(indent, f)
		return True
	
	print >>outf,  "Unknown atom ", type_name
	return False

#f = open(sys.argv[1], "rb")
outf = open(string.replace(sys.argv[1], "rez", "txt"), "w")
#buf = f.read()

#magic = f.read(4)
#filesize, table_offset = unpack("<II",f.read(8))
#chexdump(buf[0:256])

#print >>outf,  "magic = '%s', file size = 0x%x, table offset = 0x%x" % (magic, filesize, table_offset)
#f.seek(table_offset)
#collection = TResourceCollection(f.read())
#foo = collection.get_resources()
#print >>outf,  foo

#for item in foo:
	#print >>outf,  item.name+": "+str(parse_item(item.get_item(f)))+"\n"
	
#while True:
#	if not print_data("", f): break
