'''
Created on 12 mar 2014

@author: mlenart
'''

# serialize uint16 as big endian
def htons(n):
    assert n < 65536
    assert n >= 0
    res = bytearray()
    res.append((n & 0x00FF00) >> 8)
    res.append(n & 0x0000FF)
    return res

# serialize uint32 as big endian
def htonl(n):
    assert n >= 0
    res = bytearray()
    res.append((n & 0xFF000000) >> 24)
    res.append((n & 0x00FF0000) >> 16)
    res.append((n & 0x0000FF00) >> 8)
    res.append(n & 0x000000FF)
    return res

def serializeString(string):
    out = bytearray()
    out.extend(string.encode('utf8'))
    out.append(0)
    return out
