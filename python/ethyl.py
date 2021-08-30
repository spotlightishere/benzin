import sys, struct, re
formats = {
    'pane': '''
            u8 flags_1          // 8
            u8 flags_2          // 9
            u8 alpha            // a
            u8 alpha_2          // b
            char name[0x18]     // c
            float x             // 24
            float y             // 28
            float z             // 2c
            float flip_x = 0    // 30 - note: flip_x = rotate around the x axis, which is a *vertical* flip
            float flip_y = 0    // 34
            float angle         // 38
            float xmag          // 3c
            float ymag          // 40
            float width         // 44
            float height        // 48
            etc!
        ''',
    'text': '''
            u16 len1            // 4c
            u16 len2            // 4e
            u16 mat_off         // 50
            u16 font_idx        // 52 - into fnl1
            u8 unk4             // 54
            u8 pad[3] = [0, 0, 0]// 55
            u32 name_offs       // 58
            u32 color1          // 5c
            u32 color2          // 60 (--> dc)
            float font_size_x   // 64
            float font_size_y   // 68
            float char_space    // 6c
            float line_space    // 70
            etc!
        ''',
    'pic': '''
            u32 vtx_colors[4] = [0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff] // 4c - what are these?
            u16 mat_off             // 5c
            u8 num_texcoords        // 5e, should be < 8
            u8 padding = 0          // 5f
            etc!
        ''',
    'window': '''
            float unk1  // 4c
            float unk2    // 50
            float unk3    // 54
            float unk4    // 58
            u32 unk5    // 5c - ???
            u32 offs    // 60 - from start of pane
            etc!
        ''',
    'window2': ''' // (r29)
            u32 unk1                 // 0
            u32 unk2                 // 4
            u32 unk3                 // 8
            u32 unk4                 // c
            u16 mat_off              // 10
            u8 texcoord_array_size   // 12 -- max 8
            u8 pad                   // 13
            etc! // followed by the texcoord array
        ''',
            
    'group': '''
            char name[16]
            u16 numsubs
            u16 unk
            etc!
        ''',
        
    'numoffs': '''
            u16 num
            u16 offs
            etc!
        ''',
    'lytheader': '''
            u8 a
            u8 pad[3] = [0, 0, 0]
            float width
            float height
        ''',
    'material': '''
            char name[0x14]     // 0
            s16 tev_color[4]    // 14
            s16 unk_color[4]    // 1c
            s16 unk_color_2[4]  // 24
            u32 tev_kcolor[4]   // 2c
            u32 flags = 0       // 3c - 814fee68 - see doc
            etc!
        ''',
    'texref': '''
            u16 tex_offs
            u8 wrap_s
            u8 wrap_t
        ''',
    '4b': '''
            u8 _[4]
        ''',
    '10b': '''
            u8 _[0x10]
        ''',
    'ua2': '''
            float _[5]
        ''',
    'ua7': '''
            u32 a
            u32 b
            float c
            u32 d
            u32 e
        ''',
    

}

for (dname, definition) in formats.items():
    definition = re.sub(re.compile('//.*$', re.M), '', definition)
    definition = definition.strip()
    definition = re.sub('\n[\n ]+', '\n', definition)
    df = [[j.strip() for j in i.strip().split()] for i in definition.split('\n')]
    df = []
    etc = etcn = False
    for i in definition.split('\n'):
        i = i.strip()
        if i[:3] == 'etc':
            etc = True
            if i == 'etc!':
                etcn = True
            continue
        x = [j.strip() for j in i.split()]
        typ, name = x[:2]
        try:
            assert x[2] == '='
            default = eval(' '.join(x[3:]))
        except:
            default = None
        m = re.match('^([^\[]+)\[([a-zA-Z0-9]+)\]$', name)
        if m:
            name = m.group(1)
            size = eval(m.group(2))
            df.append((typ, (name, size), default))
        else:
            df.append((typ, name, default))
    formats[dname] = df, etc, etcn
    
# Dumb pretty printer
def my_repr(b, start=''):
    '''scrap this shit and implement new one eventually'''
    special = lambda x: type(x) not in (int, str, unicode)
    if type(b) == dict:
        if len(filter(special, b.values())) == 0:
            rep = '{ '
            f = False
            for a in sorted(b.keys()):
                if f:
                    rep += ', '
                else:
                    f = True
                rep += '"%s": %s' % (str(a), my_repr(b[a]))
            rep += ' }'
        else:
            rep = '{\n'
            for a in sorted(b.keys()):
                c = b[a]
                if type(c) in (dict, list):
                    rep += '"%s":\n%s,\n' % (str(a), my_repr(c, '  '))
                else:
                    rep += '"%s": %s,\n' % ( str(a), my_repr(c))
            rep += '}'
    elif type(b) in (int, long):
        rep = hex(b)
    elif type(b) in (list, tuple):
        if len(filter(special, b)) == 0:
             rep = '[' + ', '.join(map(my_repr, b)) + ']'
        else:
            rep = '[\n'
            rep += ',\n'.join(my_repr(a, '  ') for a in b)
            rep += '\n]'
    else:
        rep = repr(b)
    return re.sub(re.compile('^', re.M), start, rep)
    
def pr_dict(*args):
    print my_repr(*args)
    
types = {
    'u8': 'B',
    'u16': 'H',
    'u32': 'I',
    'u64': 'Q',
    's8': 'b',
    's16': 'h',
    's32': 'i',
    's64': 'q',
    'float': 'f',
    'f32': 'f',
    'double': 'd',
    'f64': 'd',
    'bool': 'c',
    'char': 'c',
}

def parse_var(pos, typ, chunk):
    if not types.has_key(typ):
        raise Exception('What\'s a %s?' % typ)
    fmt = '>' + types[typ]
    size = struct.calcsize(fmt)
    
    return (pos + size), struct.unpack(fmt, chunk[pos:pos + size])[0]
def nullterm(str_plus):
    z = str_plus.find('\0')
    if z != -1:
        return str_plus[:z]
    else:
        return str_plus
        
def unnullterm(var, size):
    if len(var) > size:
        raise Exception('unnullterm: "%s" too long' % var)
    return var.ljust(size, '\0')
    
def untv(name):
    #assert '\0' not in name
    lex = len(name) + 16
    lex -= lex % 16
    return name.ljust(lex, '\0')
    
def parse_data(chunk, definition, prefix=None, start=0):
    df, etc, etcn = formats[definition]
    #print df, repr(definition)
    #print df

    pos = start
    ret = {}
    for typ, name, _ in df:
        try:
            name, size = name
        except:
            pos, var = parse_var(pos, typ, chunk)
        else:
            if typ == 'char':
                var = chunk[pos:pos+size].rstrip(chr(0))
                pos += size
            else:
                var = []
                for n in xrange(size):
                    pos, varn = parse_var(pos, typ, chunk)
                    var.append(varn)
        if ret.has_key(name):
            try:
                ret[name].append(var)
            except AttributeError:
                ret[name] = [ret[name], var]
        else:
            ret[name] = var
    #print hex(pos)
    if pos > len(chunk):
        raise Exception('too much!')
    if pos < len(chunk):
        if etc and not etcn:
            ret['etc'] = repr(chunk[pos:])
        elif not etc:
            raise Exception('Incomplete handling...')
    if prefix is not None:
        ret2 = {}
        for (k, v) in ret.items():
            ret2[prefix + '.' + k] = v
        ret = ret2
    
    if ret.keys() == ['_']: # just an array
        ret = ret['_']

    return (ret, pos) if etcn else ret
    
def unparse_var(typ, var):
    if not types.has_key(typ):
        raise Exception('What\'s a %s?' % typ)
    return struct.pack('>' + types[typ], var)
    
def unparse_data(vars, definition, prefix=None):
    df, etc, etcn = formats[definition]
    ret = ''
    
    #print df, vars
    
    if len(df) == 1:
        try:
            assert df[0][1] == '_' or df[0][1][0] == '_'
        except:
            pass
        else:
            vars = {'_': vars}
    
    for typ, name, default in df:
        size = None
        try:
            name, size = name
        except:
            pass
        if prefix is not None:
            name = prefix + '.' + name
        try:
            var = vars[name]
        except KeyError:
            if default is None:
                raise
            else:
                var = default
                
        if size is None:
            ret += unparse_var(typ, var)
        else:
            if typ == 'char':
                ret += unnullterm(var, size)
            else:
                assert len(var) == size
                for v in var:
                    ret += unparse_var(typ, v)
    return ret
    
def iff_to_chunks(z):
    pos = 0
    chunks = []
    while pos < len(z):
        cc = z[pos:pos+4]
        length, = struct.unpack('>I', z[pos+4:pos+8])
        #print cc
        #print hex(length)
        data = z[pos+8:pos+length]
        pos += length
        chunks.append((cc, data))
    return chunks
    
def bit_extract(num, start, end=None):
    if end is None:
        end = start
    mask = (2**(31 - start + 1) - 1) - (2**(31 - end) - 1)
    ret = (num & mask) >> (31 - end)
    
    return ret
    
def bit_place(num, start, end=None):
    # Just for sanity
    if end is None:
        end = start
    assert num <= 2**(end - start)
    return num << (31 - end)
    
def get_array(chunk, startpos, array_size, item_size, item_type=None):
    ar = []
    pos = startpos
    
    for n in xrange(array_size):
        ar.append(get_opt(chunk, pos, True, item_size, item_type)[0])
        pos += item_size
    return ar, pos
    
def get_opt(chunk, startpos, enabled, size, item_type=None):
    if not enabled:
        return None, startpos
    elif item_type is not None:
        ret = parse_data(chunk[startpos:startpos+size], item_type)
    elif size == 4:
        ret = struct.unpack('>I', chunk[startpos:startpos+4])[0]
    elif size % 4 == 0:
        ret = struct.unpack('>' + 'I'*(size/4), chunk[startpos:startpos+size])
    else:
        raise Exception('unhandled')
    return ret, startpos + size
    
def put_array(var, flags, start, end, item_size, item_type=None):
    ret = ''
    for el in var:
        dat, _ = put_opt(el, flags, 0, item_size, item_type)
        ret += dat
    flags |= bit_place(len(var), start, end)
    return ret, flags
        
def put_opt(var, flags, bit, size, item_type=None):
    if var is None:
        return '', flags
    else:
        flags |= bit_place(1, bit)
        if item_type is not None:
            ret = unparse_data(var, item_type)
        elif size == 4:
            ret = struct.pack('>I', var)
        elif size % 4 == 0:
            ret = struct.pack('>' + 'I'*(size/4), *var)
        else:
            raise Exception('unhandled')
        return ret, flags
        
def i2f(int):
    return struct.unpack('>f', struct.pack('>I', int))[0]