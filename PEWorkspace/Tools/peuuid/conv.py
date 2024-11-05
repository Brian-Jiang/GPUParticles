import uuid
def regisrtyTo4UInt32():
    s = str(uuid.uuid1())
    print 'converting %s to 4 uint32..' % (s,)
    parts = s.split('-')
    hexStr0 = parts[0]
    hexStr1 = parts[1] + parts[2]
    hexStr2 = parts[3] + parts[4][:4]
    hexStr3 = parts[4][4:]
    if not hexStr0.startswith('0x'): hexStr0 = '0x' + hexStr0
    if not hexStr1.startswith('0x'): hexStr1 = '0x' + hexStr1
    if not hexStr2.startswith('0x'): hexStr2 = '0x' + hexStr2
    if not hexStr3.startswith('0x'): hexStr3 = '0x' + hexStr3
    i0 = int(hexStr0, 16)
    i1 = int(hexStr1, 16)
    i2 = int(hexStr2, 16)
    i3 = int(hexStr3, 16)
    print 'Result: %d %d %d %d' % (i0, i1, i2, i3)
    return '%s, %s, %s, %s,' % (hexStr0, hexStr1, hexStr2, hexStr3)
    