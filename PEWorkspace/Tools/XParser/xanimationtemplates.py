from xtemplates import XClass
class AnimationOptions(XClass):
	UUID = '<E2BF56C0-840F-11cf-8F52-0040333594A3>'
	def __init__(self, data, pos):
		self.amt = 0
		pos += self.readName(data, pos)
		pos += 1; self.amt += 1	#skip '{'
		self.openClosed = int(data[pos]) #0 - closed, 1 - open
		pos += 1; self.amt += 1
		self.positionQuality = int(data[pos])	#0-spline postions, 1 - linear positions
		pos += 1; self.amt += 1
		pos += 1; self.amt += 1	#skip '}'
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
		self.amt = 0
		pos += self.readName(data, pos)
		pos += 1; self.amt += 1	#skip '{'
		
		self.keyType = int(data[pos])	#0 - rotation, 1 - scale, 2 - position, 4 - matrix transformation
		pos += 1; self.amt += 1
		
		self.nKeys = int(data[pos])
		pos += 1; self.amt += 1
		
		self.timedFloatKeys = []
		for i in range(self.nKeys):
			self.timedFloatKeys.append(TimedFloatKeys(data, pos))
			pos += self.timedFloatKeys[-1].amt; self.amt += self.timedFloatKeys[-1].amt;
		pos += 1; self.amt += 1	#skip '}'
class Animation(XClass):
	UUID = '<3D82AB4F-62DA-11cf-AB39-0020AF71E433>'
	def __init__(self, data, pos):
		self.amt = 0
		self.frameName = None
		self.frame = None
		self.animationOptions = None
		self.animationKeys = []
		self.additional = {}
		pos += self.readName(data, pos)
		pos += 1; self.amt += 1	#skip '{'
		while (data[pos] != '}'):
			#parse data until reach the end of Animation
			if data[pos] == '{':
				#reference of animated frame
				self.frameName = []
				pos += 1; self.amt += 1	#skip '{'
				while data[pos] != '}':
					self.frameName.append(data[pos])
					pos += 1; self.amt += 1
				self.frameName = reduce(str.__add__, self.frameName)
				pos += 1; self.amt += 1	#skip '}' - end of reference
			else:
				#either AnimationKey or AnimationOption
				type = data[pos]
				pos += 1; self.amt += 1
				print 'Additional Data: ', type, 'handled by:', classMap[type]
				cls = classMap[type]
				name = 'obj_' + str(len(self.additional)) + '_' + type
				self.additional[name] = cls(data, pos)
				pos += self.additional[name].amt; self.amt += self.additional[name].amt
				if cls is AnimationOptions:
					self.animationOptions = self.additional[name]
				elif cls is AnimationKey:
					self.animationKeys.append(self.additional[name])
		#start
		frame = XFile.curXLoad.frames[self.frameName]
		try:
			frame.animationSets[AnimationSet.curSetName][reduce(str.__add__, self.name)] = self
		except:
			frame.animationSets[AnimationSet.curSetName] = {}
			frame.animationSets[AnimationSet.curSetName][reduce(str.__add__, self.name)] = self
		#stop
		pos += 1; self.amt += 1	#skip '}'
class AnimationSet(XClass):
	UUID = '<3D82AB50-62DA-11cf-AB39-0020AF71E433>'
	curSetName= None
	def __init__(self, data, pos):
		self.amt = 0
		self.additional = {}
		self.animations = []
		pos += self.readName(data, pos)
		AnimationSet.curSetName = reduce(str.__add__, self.name)
		pos += 1; self.amt += 1	# skip '{'
		while data[pos] != '}':
			type = data[pos]
			pos += 1; self.amt += 1
			print 'Additional Data: ', type, 'handled by:', classMap[type]
			cls = classMap[type]
			name = 'obj_' + str(len(self.additional)) + '_' + type
			self.additional[name] = cls(data, pos)
			pos += self.additional[name].amt; self.amt += self.additional[name].amt
			if cls is Animation:
				self.animations.append(self.additional[name])
		pos += 1; self.amt += 1	# skip '}'
		
class XSkinMeshHeader(XClass):
	def __init__(self, data, pos):
		self.amt = 0
		pos += self.readName(data, pos)
		pos += 1; self.amt += 1	#skip '{'
		
		self.nMaxSkinWeightsPerVertex = int(data[pos])
		pos += 1; self.amt += 1
		
		self.nMaxSkinWeightsPerFace = int(data[pos])
		pos += 1; self.amt += 1
		
		self.nBones = int(data[pos])
		pos += 1; self.amt += 1
		
		pos += 1; self.amt += 1	#skip '}'
class SkinWeights(XClass):
	UUID = ''
	def __init__(self, data, pos):
		self.amt = 0
		pos += self.readName(data, pos)
		pos += 1; self.amt += 1	#skip '{'
		self.boneName = data[pos][1:-1]	# get rid of " "
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
		pos += 1; self.amt += 1	#skip '}' 
