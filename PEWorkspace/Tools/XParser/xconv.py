import sys
import exceptions
import ctypes
from xtemplates import XClass
import math
from sys import stdin
reserved = (
    'ARRAY', 'BINARY', 'BINARY_RESOURCE', 'CHAR', 'CSTRING', 'DOUBLE', 'DWORD', 'FLOAT',
    'SDWORD', 'STRING', 'SWORD', 'TEMPLATE', 'UCHAR', 'ULONGLONG', 'UNICODE', 'WORD'
)

class XImportException(exceptions.Exception):
    def __init__(self, args = None):
        self.args = args
    
class template(XClass):
    def __init__(self, data, pos):
        XFile.addIndent('[template]')
        self.amt = 0
        self.data = []
        pos += self.readName(data, pos)
        print XFile.indent + "[template]: Skipping:", self.name,': template.'
        pos += 1; self.amt += 1    #skip '{'
        openBrackets = 1
        while openBrackets > 0:
            self.data.append(data[pos])
            if data[pos] == '{':
                openBrackets += 1
            elif data[pos] == '}':
                openBrackets -= 1
            pos += 1; self.amt += 1
        XFile.subIndent('[template]')
            
class Header(object):
    UUID = '<3D82AB43-62DA-11cf-AB39-0020AF71E433>'
    __slots__  = ('major', 'minor', 'flags')
    def __init__(self):
        self.major = None
        self.minor = None
        self.flags = None
        
class Vector(XClass):
    UUID = '<3D82AB5E-62DA-11cf-AB39-0020AF71E433>'
    __slots__ = ('x', 'y', 'z', 'amt')
    def __init__(self, data, pos):
        self.x = float(data[pos])
        self.y = float(data[pos + 1])
        self.z = float(data[pos + 2])
        self.amt = 3
    def __str__(self):
        return "[" + str(self.x) + "," + str(self.y) + "," + str(self.z) + "]"
        
class Coords2d(XClass):
    UUID = '<F6F23F44-7686-11cf-8F52-0040333594A3>'
    __slots__ = ('u', 'v', 'amt')
    def __init__(self, data, pos):
        self.u = float(data[pos])
        self.v = float(data[pos + 1])
        self.amt = 2
    def __str__(self):
        return "[" + str(self.u) + "," + str(self.v) + "]"
        
class Quaternion(object):
    UUID = '<10DD46A3-775B-11cf-8F52-0040333594A3>'
    __slots__ = ('s', 'v')
    
class Matrix4x4(object):
    UUID = '<F6F23F45-7686-11cf-8F52-0040333594A3>'
    __slots__ = ('matrix', 'amt')
    def __init__(self, data, pos):
        self.matrix = []
        for i in range(16):
            self.matrix.append(float(data[pos + i]))
        self.amt = 16
        
class ColorRGBA(object):
    UUID = '<35FF44E0-6C7C-11cf-8F52-0040333594A3>'
    __slots__ = ('red', 'green', 'blue', 'alpha', 'amt')
    def __init__(self, data, pos):
        XFile.addIndent('[ColorRGBA]')
        print XFile.indent + '[ColorRGBA]: reading expected 4 float values'
        self.red = float(data[pos])
        self.green = float(data[pos + 1])
        self.blue = float(data[pos + 2])
        self.alpha = float(data[pos + 3])
        self.amt = 4
        XFile.subIndent('[ColorRGBA]')
        
class ColorRGB(object):
    UUID = '<D3E16E81-7835-11cf-8F52-0040333594A3>'
    __slots__ = ('red', 'green', 'blue', 'amt')
    def __init__(self, data, pos):
        XFile.addIndent('[ColorRGB]')
        print XFile.indent + '[ColorRGB]: reading expected 3 float values'
        self.red = float(data[pos])
        self.green = float(data[pos + 1])
        self.blue = float(data[pos + 2])
        self.amt = 3
        XFile.subIndent('[ColorRGB]')
        
class LevelProperties(object):
    UUID = '<>'
    def __init__(self, data, pos):
        XFile.addIndent('[LevelProperties]')
        self.amt = 0
        print XFile.indent + '[LevelProperties]: reading reading expected 1 value'
        pos += 1; self.amt += 1    #skip '{'
        self.isLevel = int(data[pos])
        XFile.curXLoad.isLevel = self.isLevel
        print  XFile.indent + 'XFile.curXLoad.isLevel:', XFile.curXLoad.isLevel
        pos += 1; self.amt += 1
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[LevelProperties]')

class MetaScript(object):
    UUID = '<>'
    def __init__(self, data, pos):
        XFile.addIndent('[MetaScript]')
        self.amt = 0
        print XFile.indent + '[MetaScript]: reading reading expected 1 value'
        pos += 1; self.amt += 1    #skip '{'
        self.MetaScriptFilename = str(data[pos])
        pos += 1; self.amt += 1
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[MetaScript]')
        
class PEUUID(object):
    UUID = '<>'
    def __init__(self, data, pos):
        XFile.addIndent('[PEUUID]')
        self.amt = 0
        print XFile.indent + '[PEUUID]: reading expected four integers'
        pos += 1; self.amt += 1    #skip '{'
        self.peuuidNums = [str(data[pos]), str(data[pos+1]), str(data[pos+2]), str(data[pos+3])]
        pos += 4; self.amt += 4
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[PEUUID]')
        
class Indexed_Color(object):
    UUID = '<1630B820-7842-11cf-8F52-0040333594A3>'
    __slots__ = ('index', 'indexColor')
class Boolean(object):
    UUID = '<4885AE61-78E8-11cf-8F52-0040333594A3>'
    __slots__ = ('u') #is it u?
class Boolean2d(object):
    UUID = '<4885AE63-78E8-11cf-8F52-0040333594A3>'
    __slots__ = ('u', 'v')
class Material(XClass):
    UUID = '<3D82AB4D-62DA-11cf-AB39-0020AF71E433>'
    __slots__ = ('name', 'faceColor', 'power', 'specularColor', 'emissiveColor', 'texture', 'amt')
    def __init__(self, data, pos):
        XFile.addIndent('[Material]')
        self.amt = 0
        self.textures = []
        self.additionalBlocks = {} # [..] in .X template
        pos += self.readName(data, pos)
        print XFile.indent + '[Material]: loading:', self.name, ': material'
        pos += 1; self.amt += 1    #skip '{'
        self.faceColor = ColorRGBA(data, pos)
        pos += self.faceColor.amt; self.amt += self.faceColor.amt
        self.power = float(data[pos])
        pos += 1; self.amt += 1
        self.specularColor = ColorRGB(data, pos)
        pos += self.specularColor.amt; self.amt += self.specularColor.amt
        self.emissiveColor = ColorRGB(data, pos)
        pos += self.emissiveColor.amt; self.amt += self.emissiveColor.amt
        while data[pos] != '}':
            type = data[pos]
            print XFile.indent + '[Material]:' , 'are not at the end of the block'
            print XFile.indent + '[Material]: Block Start: class', type, 'will be handled by:', classMap[type]
            name = 'block_' + str(len(self.additionalBlocks)) + '_' + type
            cls = classMap[type]
            #maybe texture is avaialble
            pos += 1; self.amt += 1    #skip type
            block = cls(data, pos)
            self.additionalBlocks[name] = block
            if type == 'TextureFilename' or type == 'TextureFileName': self.textures.append(block)
            pos += block.amt; self.amt += block.amt
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[Material]')
        XFile.curXLoad.materials[self.name] = self
    @staticmethod
    def readNameFromList(data, pos):
        inc = 0
        pos += 1; inc += 1    #skip '{'
        name = []
        while data[pos] != '}':
            name.append(data[pos])
            pos += 1; inc += 1
        name = reduce(str.__add__, name)
        pos += 1; inc += 1    #skip '}'
        return name, inc
class TextureFilename(XClass):
    UUID = '<A42790E1-7810-11cf-8F52-0040333594A3>'
    __slots__ = ('filename', 'amt', 'name')
    def __init__(self, data, pos):
        XFile.addIndent('[TextureFilename]')
        print XFile.indent + '[TextureFilename]: reading expected string:',
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        self.filename = data[pos][1:-1]
        print self.filename
        pos += 1; self.amt += 1
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[TextureFilename]')
        
class MeshMaterialList(XClass):
    UUID = '<F6F23F42-7686-11cf-8F52-0040333594A3>'
    __slots__ = ('name', 'amt', 'nMaterials', 'materials', 'nFaceIndices', 'faceIndices')
    def __init__(self, data, pos):
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        self.nMaterials = int(data[pos])
        pos += 1; self.amt += 1
        self.nFaceIndices = int(data[pos])
        pos += 1; self.amt += 1
        self.faceIndices = []
        for i in range(self.nFaceIndices):
            self.faceIndices.append(int(data[pos]))
            pos += 1; self.amt += 1
        self.materials = []
        for i in range(self.nMaterials):
            matName, inc = Material.readNameFromList(data, pos)
            self.materials.append(matName)
            pos += inc; self.amt += inc
        pos += 1; self.amt += 1    #skip '}'
        
class MeshFace(XClass):
    UUID = '<3D82AB5F-62DA-11cf-AB39-0020AF71E433>'
    __slots__ = ('nFaceVertexIndices', 'faceVertexIndices', 'amt')
    def __init__(self, data, pos):
        self.nFaceVertexIndices = int(data[pos])
        
        pos += 1
        self.faceVertexIndices = []
        for i in range(self.nFaceVertexIndices):
            self.faceVertexIndices.append(int(data[pos]))
            pos += 1
        self.amt = self.nFaceVertexIndices + 1
        #if self.nFaceVertexIndices == 4:
        #    self.nFaceVertexIndices = 3
        #    self.faceVertexIndices = self.faceVertexIndices[0:3]
        
    def __str__(self):
        res = "[" + str(self.nFaceVertexIndices) + ":"
        for i in self.faceVertexIndices:
            res = res + str(i) + ','
        res = res +  "]"
        return res
class MeshNormals(XClass):
    UUID = '<F6F23F43-7686-11cf-8F52-0040333594A3>'
    __slots__ = ('nNormals', 'normals', 'nFaceNormals', 'faceNormals', 'amt')
    def __init__(self, data, pos):
        print 'Loading Mesh Normals...'
        self.amt = 0
        pos += self.readName(data, pos)
        
        pos += 1; self.amt += 1    #skip '{'
        
        self.nNormals = int(data[pos])
        pos += 1; self.amt += 1
        
        self.normals, inc = Vector.read(self.nNormals, data, pos)
        pos += inc;    self.amt += inc
        
        #for n in self.normals:
        #    print n
        self.nFaceNormals = int(data[pos])
        pos += 1
        self.faceNormals, inc = MeshFace.read(self.nFaceNormals, data, pos)
        pos += inc
        self.amt += (1 + inc)
        #for f in self.faceNormals:
        #    print f
        pos += 1    #skip '}'
        self.amt += 1
        print 'Loaded %d normals and face indices for %d faces' % (self.nNormals, self.nFaceNormals)
class MeshTextureCoords(XClass):
    UUID = '<F6F23F40-7686-11cf-8F52-0040333594A3>'
    __slots__ = ('nTextureCoords', 'textureCoords', 'amt', 'name')
    def __init__(self, data, pos):
        print 'Loading Mesh Texture Coordinates ...'
        self.amt = 0
        pos += self.readName(data, pos)
        
        pos += 1; self.amt += 1    #skip '{'
        self.nTextureCoords = int(data[pos])
        pos += 1; self.amt += 1
        self.textureCoords, inc = Coords2d.read(self.nTextureCoords, data, pos)
        pos += inc; self.amt += inc
        #for c in self.textureCoords:
        #    print c
        pos += 1; self.amt += 1    #skip '}'
        print 'Loaded %d Texture Coordinates' %(self.nTextureCoords,)
def convertDWORDToFloat( dword ):
    pDw = ctypes.pointer( ctypes.c_uint( dword ) )
    pF = ctypes.cast( pDw, ctypes.POINTER( ctypes.c_float ) )
    return pF[0]
class DeclData(XClass):
    UUID = ''
    def __init__(self, data, pos):
        XFile.addIndent('[DeclData]')
        self.amt = 0
        pos += self.readName(data, pos)        
        pos += 1; self.amt += 1    #skip '{'
        
        self.nElems = int(data[pos]) # how many elements just tnagents, binormals, or both..
        pos += 1; self.amt += 1
        v0 = int(data[pos]); v1 = int(data[pos+1]); v2 = int(data[pos+2]); v3 = int(data[pos+3]);
        pos += 4; self.amt += 4
        tangents = False
        if v0 == 2 and v1 == 0 and v2 == 6 and v3 == 0:
            #tangents
            tangents = True
        self.nFloats = int(data[pos])
        pos += 1; self.amt += 1
        self.tangentList = []
        if (tangents):
            for it in xrange(self.nFloats / 3):
                xw = int(data[pos])
                x = convertDWORDToFloat(xw)
                yw = int(data[pos+1])
                y = convertDWORDToFloat(yw)
                zw = int(data[pos+2])
                z = convertDWORDToFloat(zw)
                self.tangentList.append([x, y, z])
                pos += 3; self.amt += 3        
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[DeclData]')
class MeshFaceWraps(object):
    UUID = '<4885AE62-78E8-11cf-8F52-0040333594A3>'
    __slots__ = ('nFaceWrapValues', 'faceWrapValues')
class Mesh(XClass):
    UUID = '<3D82AB44-62DA-11cf-AB39-0020AF71E433>'
    __slots__ = ('name', 'nVertices', 'vertices', 'nFaces', 'faces', 'amt', 'additional', 'normals', 'materialList', 'facesByMat', 'skinWeightSets')
    def __init__(self, accum, pos):
        XFile.addIndent('[Mesh]')
        self.isSkin = False
        self.normals = None
        self.materialList = None
        self.facesByMat = None
        self.texCoords = None
        self.tangents = None
        self.skinWeightSets = []
        self.amt = 0
        pos += self.readName(accum, pos)
        if self.name == ' ' or self.name == '': 
            self.name = XFile.lastFrameName
        print XFile.indent + '[Mesh]: Loading', self.name, ': Mesh'
        
        pos += 1; self.amt += 1    #skip '{'
        
        self.nVertices = int(accum[pos])
        pos += 1; self.amt += 1
        print XFile.indent + '[Mesh]: reading expected', self.nVertices, 'vertices'
        self.vertices, inc = Vector.read(self.nVertices, accum, pos)
        pos += inc; self.amt += inc
        
        #for v in self.vertices:
        #    print v
        
        self.nFaces = int(accum[pos])
        pos += 1; self.amt += 1
        print XFile.indent + '[Mesh]: reading expected', self.nFaces, 'faces'
        self.faces, inc = MeshFace.read(self.nFaces, accum, pos)
        pos += inc; self.amt += inc
        
        #for f in self.faces:
        #    print f
        #raw_input()
        #done with obligatory data. Now start seacrhing for any other data stored in Mesh
        self.additionalBlocks = {}
        while (accum[pos] != '}'):
            #parse data until reach the end of Mesh
            type = accum[pos]
            pos += 1; self.amt += 1
            print 'Additional Data: ', type, 'handled by:', classMap[type]
            cls = classMap[type]
            name = 'block_' + str(len(self.additionalBlocks)) + '_' + type
            block = cls(accum, pos)
            self.additionalBlocks[name] = block
            if cls is MeshNormals:
                self.normals = block
            elif cls is MeshMaterialList:
                self.materialList = block
            elif cls is SkinWeights:
                self.skinWeightSets.append(block)
                self.isSkin = True
            elif cls is MeshTextureCoords:
                self.texCoords = block
            elif cls is DeclData:
                self.tangents = block.tangentList
            pos += block.amt; self.amt += block.amt
        pos += 1; self.amt += 1    #skip '}'
        if self.materialList:
            self.sortFacesByMaterial()
        self.mergeNormals()
        #self.optimizeMesh()
        XFile.subIndent('[Mesh]')
    def sortFacesByMaterial(self):
        self.facesByMat = {}
        self.facesByMatIndex = []
        for name in self.materialList.materials:
            self.facesByMat[name] = []
            self.facesByMatIndex.append([])
        fIndex = 0
        for face in self.materialList.faceIndices:
            self.facesByMat[self.materialList.materials[face]].append(fIndex)
            self.facesByMatIndex[face].append(self.faces[fIndex])
            fIndex += 1
    def floatsEqual(self, a, b):
        return abs(a - b) <= 0.0001
    def strideInData(self, stride, lst):
        i = 0
        for s in lst:
            if self.stridesEqual(stride, s):
                return True, i
        return False, -1
    def averageStride(self, strides):
        avgNormal = [0,0,0]
        avgTangent = [0,0,0]
        n = len(strides)
        for s in strides:
            avgNormal[0] += s[2].x
            avgNormal[1] += s[2].y
            avgNormal[2] += s[2].z
            avgTangent[0] += s[3][0]
            avgTangent[1] += s[3][1]
            avgTangent[2] += s[3][2]
        avgNormal[0] /= n
        avgNormal[1] /= n
        avgNormal[2] /= n
        avgTangent[0] /= n
        avgTangent[1] /= n
        avgTangent[2] /= n
        lenNorm = math.sqrt(avgNormal[0]*avgNormal[0] + avgNormal[1]*avgNormal[1] + avgNormal[2]*avgNormal[2])
        if (math.fabs(lenNorm) < 0.00001):
            lenNorm = 1.0;
            print "Warning: Normal length = 0 when averaging strides"
        avgNormal[0] /= lenNorm
        avgNormal[1] /= lenNorm
        avgNormal[2] /= lenNorm
        lenTang = math.sqrt(avgTangent[0]*avgTangent[0] + avgTangent[1]*avgTangent[1] + avgTangent[2]*avgTangent[2])
        if (math.fabs(lenTang) < 0.00001):
            lenTang = 1.0;
            print "Warning: Tangent length = 0 when averaging strides"
        avgTangent[0] /= lenTang
        avgTangent[1] /= lenTang
        avgTangent[2] /= lenTang
        for s in strides:
            s[2].x = avgNormal[0]
            s[2].y = avgNormal[1]
            s[2].z = avgNormal[2]
            s[3][0] = avgTangent[0]
            s[3][1] = avgTangent[1]
            s[3][2] = avgTangent[2]
    '''def oddStridesEqual(self, a, b):
        pa, pb = a[0], b[0]
        posEqual = self.floatsEqual(pa.x, pb.x) and self.floatsEqual(pa.y, pb.y) and self.floatsEqual(pa.z, pb.z)
        if not posEqual: return False
        tca, tcb = a[1], b[1]
        if tca != None:
            tcEqual = self.floatsEqual(tca.u, tcb.u) and self.floatsEqual(tca.v, tcb.v)
            if not tcEqual: return False
        return True
    '''
    def oddStridesEqual(self, a, b):
        pa, pb = a[0], b[0]
        posEqual = pa.x == pb.x and pa.y == pb.y and pa.z == pb.z
        if not posEqual: return False
        tca, tcb = a[1], b[1]
        if tca != None:
            tcEqual = tca.u == tcb.u and tca.v == tcb.v
            if not tcEqual: return False
        return True
    def stridesEqual(self, a, b):
        pa, pb = a[0], b[0]
        posEqual = self.floatsEqual(pa[0], pb[0]) and self.floatsEqual(pa[1], pb[1]) and self.floatsEqual(pa[2], pb[2])
        if not posEqual: return False
        tca, tcb = a[1], b[1]
        if tca != None:
            tcEqual = self.floatsEqual(tca[0], tcb[0]) and self.floatsEqual(tca[1], tcb[1])
            if not tcEqual: return False
        #tanga, tangb = a[3], b[3]
        #if tanga != None:
        #    tangEqual = self.floatsEqual(tanga[0], tangb[0]) and self.floatsEqual(tanga[1], tangb[1]) and self.floatsEqual(tanga[2], tangb[2])
        #    if not tangEqual: return False
        return True
    def optimizeMesh(self):
        data = []
        for v in self.vertices:
            data.append([(v.x, v.y, v.z), None, None, None]);
        i = 0
        if self.texCoords:
            for tc in self.texCoords.textureCoords:
                data[i][1] = (tc.u, tc.v)
                i += 1
        i = 0
        if self.tangents:
            for tang in self.tangents:
                data[i][3] = (tang[0], tang[1], tang[2])
                i += 1
        # now need to get rid of same vertices
        indexMap = {} # from non optimized indices to optimized indices
        optimizedData = []
        curIndex = 0
        for stride in data:
            isIn, index = self.strideInData(stride, optimizedData)
            if isIn:
                indexMap[curIndex] = index
            else:
                optimizedData.append(stride)
                indexMap[curIndex] = len(optimizedData) - 1
            curIndex += 1
        print 'Optimized from', len(data), 'to', len(optimizedData), 'vertices'
        raw_input()
        # now need to build normals..
    def mergeNormals(self):
        print 'Initializing Normal merge into Vertex/TexCoord list'
        texCoords = self.texCoords
        newVertexData = []
        
        self.useTexCoords = True
        if texCoords == None:
            print 'Warning: Tex coordinates are absent'
            self.useTexCoords = False
            for i in range(self.nVertices):
                newVertexData.append([self.vertices[i], None, None, None])
        else:
            texCoords = texCoords.textureCoords
            for i in range(self.nVertices):
                newVertexData.append([self.vertices[i], texCoords[i], None, None])
        if self.tangents:
            tangents = self.tangents
            for i in range(self.nVertices):
                newVertexData[i][3] = tangents[i]
        normals = self.normals
        if normals == None:
            #raise XImportException, "mergeNormals(): Normals don't exist"
            self.mergedVertexData = newVertexData
            return
        fIndex = 0
        addedNormals = 0
        for f in normals.faceNormals:
            i = 0
            origFace = self.faces[fIndex]
            for iIntoNormals in f.faceVertexIndices:
                #i - index of the point in fIndexth face
                #iIntoNormals - index of a normal in normal list for the point
                #f - face with indexes into normals
                #origFace is face with indexes into vertices and texCoords
                iIntoVertices = origFace.faceVertexIndices[i]
                if newVertexData[iIntoVertices][2] is None:
                    newVertexData[iIntoVertices][2] = normals.normals[iIntoNormals]
                    addedNormals += 1
                elif not (newVertexData[iIntoVertices][2] is normals.normals[iIntoNormals]):
                    #place taken -> create new vertex data vor this point so that could have different normal
                    if self.useTexCoords:
                        newVertexData.append([self.vertices[iIntoVertices], texCoords[iIntoVertices], normals.normals[iIntoNormals]])
                    else:
                        newVertexData.append([self.vertices[iIntoVertices], None, normals.normals[iIntoNormals]])
                    
                    origFace.faceVertexIndices[i] = len(newVertexData) - 1
                i += 1
            fIndex += 1
        leftout = filter(lambda x: x[2] is None, newVertexData)
        if len(leftout) > 0:
            print 'Warning: Some (',len(leftout), ') vertices were left without normals assigned to them'
            print 'Indexes of leftout vertices:'
            for l in leftout:
                print newVertexData.index(l) ,
            print '\n'
            for l in leftout:
                i = newVertexData.index(l)
                inter = filter(lambda x:i in x.faceVertexIndices, self.faces)
                print '%dth Vertex was used in %d faces' % (i, len(inter))
                if len(inter) > 0:
                    raise XImportException, "Normal for a used vertex was not defined"
        print 'Normal merge finished: created', len(newVertexData) - self.nVertices , 'new vertices, total vertex amount:', len(newVertexData)
        self.mergedVertexData = newVertexData
        
        #print 'Do you want to optimize the xfile? [y/n]'
        #ch = stdin.readline().lower()
        ch = 'n'
        if 'y' in ch:
            processed = [False for x in xrange(len(newVertexData))]
            optimizedVertexData = []
            indexIntoOptimizedIndex = {}
            i = 0
            for stride in newVertexData:
                if processed[i] == True: 
                    i += 1
                    continue
                strides = []
                j = 0
                unoptimizedIndices = []
                for otherStride in newVertexData:
                    if processed[j] == True: 
                        j += 1
                        continue
                    if self.oddStridesEqual(stride, otherStride):
                        strides.append(otherStride)
                        processed[j] = True
                        unoptimizedIndices.append(j)
                    j += 1
                    s = stride
                self.averageStride(strides)
                optimizedVertexData.append(strides[0])
                for index in unoptimizedIndices:
                    indexIntoOptimizedIndex[index] = len(optimizedVertexData) - 1
                i += 1
            for f in self.faces:
                for iFaceIndex in xrange(f.nFaceVertexIndices):
                    oldVertexIndex = f.faceVertexIndices[iFaceIndex]
                    f.faceVertexIndices[iFaceIndex] = indexIntoOptimizedIndex[oldVertexIndex]
            self.mergedVertexData = optimizedVertexData
            #check
            print 'Optimized down to', len(optimizedVertexData), 'strides'
        '''
        for stride in newVertexData:
            strides = []
            for otherStride in newVertexData:
                if self.oddStridesEqual(stride, otherStride):
                    strides.append(otherStride)
            s = stride
            print 'Similar strides for', s[0].x, s[0].y, s[0].z, s[1].u, s[1].v
            for s in strides:
                print '[', s[2].x, s[2].y, s[2].z, '] [', s[3][0], s[3][1], s[3][2], ']'
            raw_input()
        '''
class FrameTransformMatrix(XClass):
    __slots__ = ('frameMatrix', 'amt', 'name')
    def __init__(self, data, pos):
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        self.frameMatrix = Matrix4x4(data, pos)
        pos += self.frameMatrix.amt; self.amt += self.frameMatrix.amt
        pos += 1; self.amt += 1    #skip '}'
class Frame(XClass):
    UUID = '<3D82AB46-62DA-11cf-AB39-0020AF71E433>'
    __slots__ = ('name', 'frameTransformMatrix', 'meshes', 'amt', 'additional', 'isBone')
    def __init__(self, data, pos):
        XFile.addIndent('[Frame]')
        self.amt = 0
        self.MetaScript = None
        self.isBone = False
        self.isRef = False
        self.isAnimated = False
        self.animations = {}
        self.topFrames = {}
        self.topFrameNames = []
        #doesn't have any obligatory elements
        #move on to reading additional elements
        self.additionalBlocks = {}
        self.meshes = []
        self.frameTransformMatrix = None
        pos += self.readName(data, pos)
        if self.name[0:3].lower() == 'ref':
            self.isRef = True
            XFile.curXLoad.isSceneNodeGraph = True
        if self.name != ' ' and self.name != '':
            XFile.lastFrameName = self.name
        if self.name.endswith("Skel"):
            self.isBone = True
        if self.name.endswith("Skeleton"):
            self.isBone = True
        if self.name.endswith("Joint"):
            self.isBone = True
        if self.name.endswith("Jnt"):
            self.isBone = True
        print XFile.indent + '[Frame]: loading', self.name, ': Frame'
        pos += 1; self.amt += 1    #skip '{'
        while (data[pos] != '}'):
            #parse data until reach the end of Frame
            print XFile.indent + '[Frame]:', 'are not at the end of the block'
            type = data[pos]
            pos += 1; self.amt += 1
            print XFile.indent + '[Frame]: Block Start: class', type, 'will be handled by:', classMap[type]
            cls = classMap[type]
            name = 'block_' + str(len(self.additionalBlocks)) + '_' + type
            block = cls(data, pos)
            self.additionalBlocks[name] = block
            pos += block.amt; self.amt += block.amt
            if cls is FrameTransformMatrix:
                self.frameTransformMatrix = block
            elif cls is Mesh:
                self.meshes.append(block)
            elif cls is Frame:
                frameName = block.name
                if frameName == '':
                    XFile.curXLoad.frames[name] = block
                    self.topFrames[name] = block
                    self.topFrameNames.append(name)
                    block.name = name
                else:
                    XFile.curXLoad.frames[frameName] = block
                    self.topFrames[frameName] = block
                    self.topFrameNames.append(frameName)
                    block = frameName
            elif cls is MetaScript:
                self.MetaScript = block
            elif cls is PEUUID:
                self.peuuid = block
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[Frame]')
class VertexDuplicationIndices(XClass):
    UUID = None    #was not on MS website..
    __slots__ = ('nIndices', 'nOriginalVertices', 'indices', 'name', 'amt')
    def __init__(self, data, pos):
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        self.nIndices = int(data[pos])
        self.nOriginalVertices = int(data[pos+1])
        pos += 2; self.amt += 2
        self.indices = []
        for i in range(self.nIndices):
            self.indices.append(int(data[pos]))
            pos += 1; self.amt += 1
        pos += 1; self.amt += 1    #skip '}'

class XFile(object):
    indent = ''
    curXLoad = None
    referencedBones = None
    lastFrameName = ''
    def __init__(self, file, filename):
        self.filename = filename
        self.strippedFileName = filename[filename.rfind('/')+1:]
        XFile.curXLoad = self
        XFile.referencedBones = []
        self.isSceneNodeGraph = False
        self.isLevel = False
        if isinstance(file, str):
            f = open(file, 'r')
        else:
            f = file
        magicNum = f.read(4)
        verMajorNum = f.read(2)
        verMinorNum = f.read(2)
        formatType = f.read(4)
        floatSize = f.read(4)
        print "%s: Magic#: %s, Major: %s, Minor: %s, Format: %s, Float Size: %s" % \
        (file, magicNum, verMajorNum, verMinorNum, formatType, floatSize)
        
        lines = f.readlines()
        f.close()
        accum = ''
        for l in lines:
            pos = l.find('#')
            if (pos >=0):
                l = l[:pos]
            pos = l.find('//')
            if (pos >=0):
                l = l[:pos]
            accum = accum + l
        accum = accum.replace('\n', ' ')
        accum = accum.replace('\t', ' ')
        accum = accum.replace(';', ' ')
        accum = accum.replace(',', ' ')
        accum = accum.replace('{', '{ ')
        accum = accum.replace('}', ' }')
        accum = filter(lambda x: x != '', accum.split(' '))
        #print accum
        data = accum
        pos = 0
        self.additional = {}
        self.materials = {}
        self.templates = {}
        self.frames = {}
        self.topFrames = {}
        self.topFrameNames = []
        self.animations = {}
        while pos < len(accum):
            #parse data until reach the end of file
            type = data[pos]
            pos += 1;
            print '[XFile]: Block Start: class', type, 'will be handled by:', classMap[type]
            cls = classMap[type]
            name = 'obj_' + str(len(self.additional)) + '_' + type
            
            block = cls(data, pos)
            self.additional[name] = block
            if cls is template:    #save templates in special register
                self.templates[block.name] = block
            elif cls is Frame:
                frameName = block.name
                if frameName == '':
                    self.frames[name] = block
                    self.topFrames[frameName] = block
                    self.topFrameNames.append(frameName)
                    self.additional[name].name = name
                else:
                    self.frames[frameName] = block
                    self.topFrames[frameName] = block
                    self.topFrameNames.append(frameName)
                    self.additional[name].name = frameName
            elif cls is AnimationSet:
                animName = block.name
                self.animations[animName] = block
            elif cls is LevelProperties:
                self.levelProperties = block
            pos += block.amt;
        for bName in XFile.referencedBones:
            self.frames[bName].isBone = True
    @staticmethod
    def addIndent(typename):
        XFile.indent = XFile.indent + '  '
        print XFile.indent + typename
        XFile.indent = XFile.indent + '  '
    @staticmethod
    def subIndent(typename):
        XFile.indent = XFile.indent[:-2]
        print XFile.indent + 'END ' + typename
        XFile.indent = XFile.indent[:-2]
class AnimationOptions(XClass):
    UUID = '<E2BF56C0-840F-11cf-8F52-0040333594A3>'
    def __init__(self, data, pos):
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        self.openClosed = int(data[pos]) #0 - closed, 1 - open
        pos += 1; self.amt += 1
        self.positionQuality = int(data[pos])    #0-spline postions, 1 - linear positions
        pos += 1; self.amt += 1
        pos += 1; self.amt += 1    #skip '}'
class FloatKeys(object):
    UUID = '<10DD46A9-775B-11cf-8F52-0040333594A3>'
    def __init__(self, data, pos):
        self.amt = 0
        self.nValues = int(data[pos])
        pos += 1; self.amt += 1
        self.values = []
        for i in range(self.nValues):
            self.values.append(float(data[pos]))
            pos += 1; self.amt += 1
class TimedFloatKeys(object):
    UUID = '<F406B180-7B3B-11cf-8F52-0040333594A3>'
    def __init__(self, data, pos):
        self.amt = 0
        self.time = int(data[pos])
        pos += 1; self.amt += 1
        self.floatKeys = FloatKeys(data, pos)
        pos += self.floatKeys.amt; self.amt += self.floatKeys.amt
        
class AnimationKey(XClass):
    UUID = '<10DD46A8-775B-11cf-8F52-0040333594A3>'
    def __init__(self, data, pos):
        XFile.addIndent('[AnimationKey]')
        self.amt = 0
        pos += self.readName(data, pos)
        print XFile.indent + 'loading:', self.name,': AnimationKey'
        pos += 1; self.amt += 1    #skip '{'
        
        self.keyType = int(data[pos])    #0 - rotation, 1 - scale, 2 - position, 4 - matrix transformation
        pos += 1; self.amt += 1
        
        self.nKeys = int(data[pos])
        pos += 1; self.amt += 1
        
        print XFile.indent + '[AnimationKey]: reading', self.nKeys, 'keys'
        self.timedFloatKeys = []
        for i in range(self.nKeys):
            self.timedFloatKeys.append(TimedFloatKeys(data, pos))
            pos += self.timedFloatKeys[-1].amt; self.amt += self.timedFloatKeys[-1].amt;
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[AnimationKey]')
class Animation(XClass):
    UUID = '<3D82AB4F-62DA-11cf-AB39-0020AF71E433>'
    def __init__(self, data, pos):
        XFile.addIndent('[Animation]')
        self.amt = 0
        self.frameName = None
        self.frame = None
        self.animationOptions = None
        self.animationKeys = []
        self.blocks = {}
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        print XFile.indent + '[Animation]: loading', self.name, ': Animation'
        while (data[pos] != '}'):
            #parse data until reach the end of Animation
            if data[pos] == '{':
                #reference of animated frame
                self.frameName = ['']
                pos += 1; self.amt += 1    #skip '{'
                while data[pos] != '}':
                    self.frameName.append(data[pos])
                    pos += 1; self.amt += 1
                self.frameName = reduce(str.__add__, self.frameName)
                print XFile.indent + '[Animation]: joint name:', self.frameName
                pos += 1; self.amt += 1    #skip '}' - end of reference
            else:
                #either AnimationKey or AnimationOption
                type = data[pos]
                pos += 1; self.amt += 1
                print XFile.indent + '[Animation]: Additional Data: ', type, 'handled by:', classMap[type]
                cls = classMap[type]
                block = cls(data, pos)
                if block.name == '': block.name = 'obj_' + str(len(self.blocks)) + '_' + type
                self.blocks[block.name] = block
                pos += block.amt; self.amt += block.amt
                if cls is AnimationOptions:
                    self.animationOptions = block
                elif cls is AnimationKey:
                    self.animationKeys.append(block)
        #start
        frame = XFile.curXLoad.frames[self.frameName]
        #add this animation to referenced frame (joint)
        frame.animations[AnimationSet.curSetName] = self
        #stop
        pos += 1; self.amt += 1    #skip '}'
        XFile.subIndent('[Animation]')
class AnimationSet(XClass):
    UUID = '<3D82AB50-62DA-11cf-AB39-0020AF71E433>'
    curSetName= None
    def __init__(self, data, pos):
        XFile.addIndent('[AnimationSet]')
        self.amt = 0
        self.blocks = {}
        self.animations = []
        pos += self.readName(data, pos)
        AnimationSet.curSetName = self.name
        print XFile.indent + '[AnimationSet]: loading', self.name, ': AnimationSet'
        pos += 1; self.amt += 1    # skip '{'
        while data[pos] != '}':
            type = data[pos]
            pos += 1; self.amt += 1
            print XFile.indent + '[AnimationSet]: Additional Data:', type, 'handled by:', classMap[type]
            cls = classMap[type]
            block = cls(data, pos)
            if block.name == '':
                block.name = 'obj_' + str(len(self.blocks)) + '_' + type
            self.blocks[block.name] = block
            pos += block.amt; self.amt += block.amt
            if cls is Animation:
                self.animations.append(block)
        pos += 1; self.amt += 1    # skip '}'
        XFile.subIndent('[AnimationSet]')
        
class XSkinMeshHeader(XClass):
    def __init__(self, data, pos):
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
            
        self.nMaxSkinWeightsPerVertex = int(data[pos])
        pos += 1; self.amt += 1
        
        self.nMaxSkinWeightsPerFace = int(data[pos])
        pos += 1; self.amt += 1
        
        self.nBones = int(data[pos])
        pos += 1; self.amt += 1
        
        pos += 1; self.amt += 1    #skip '}'
class SkinWeights(XClass):
    UUID = ''
    def __init__(self, data, pos):
        self.amt = 0
        pos += self.readName(data, pos)
        pos += 1; self.amt += 1    #skip '{'
        self.boneName = data[pos][1:-1]    # get rid of " "
        XFile.referencedBones.append(self.boneName)
        pos += 1; self.amt += 1
        self.nVertices = int(data[pos])
        pos += 1; self.amt += 1
        self.vertexIndices = []
        for i in range(self.nVertices):
            self.vertexIndices.append(int(data[pos]))
            pos += 1; self.amt += 1
        self.vertexWeights = []
        for i in range(self.nVertices):
            self.vertexWeights.append(float(data[pos]))
            pos += 1; self.amt += 1
        self.transform = []
        for i in range(16):
            self.transform.append(float(data[pos]))
            pos += 1; self.amt += 1
        pos += 1; self.amt += 1    #skip '}' 
    
classMap = {
    'MeshNormals' : MeshNormals,
    'MeshTextureCoords' : MeshTextureCoords,
    'FrameTransformMatrix' : FrameTransformMatrix,
    'Mesh' : Mesh,
    'Frame' : Frame,
    'MetaScript' : MetaScript,
    'PEUUID' : PEUUID,
    'LevelProperties' : LevelProperties,
    'Material' : Material,
    'TextureFilename' : TextureFilename,
    'TextureFileName' : TextureFilename,
    'MeshMaterialList' : MeshMaterialList,
    'template' : template,
    'VertexDuplicationIndices' : template, #VertexDuplicationIndices,
    'AnimationSet' : AnimationSet,
    'Animation' : Animation,
    'AnimationKey' : AnimationKey,
    'AnimationOptions' : template, #AnimationOptions,
    'AnimTicksPerSecond' : template,
    'XSkinMeshHeader' : template, #XSkinMeshHeader,
    'SkinWeights' : SkinWeights,
    'DeclData' : DeclData,
}
templatePattern = r'templatetext{text}'

def readX(file):
    return XFile(file)
def parseTemplate(data, pos):
    name = data[pos]
    pos += 1
    uuid = data[pos]
    pos += 1
    stdout.write('class ' + name + '(XClass):\n')
    stdout.write('\tUUID = ' + uuid)
    stdout.write('\tdef __init__(self, data, pos):')
    isArray = False
    fieldType = data[pos]
    pos += 1
    if fieldType == 'array':
        isArray = True
        foeldType = data[pos]
        pos += 1
    