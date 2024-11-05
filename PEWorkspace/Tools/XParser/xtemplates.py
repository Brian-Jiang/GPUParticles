class XClass(object):
	@classmethod
	def read(cls, n, data, pos):
		'reads n objects and returns list of these objects and number of data elements read from data'
		res = []
		inc = 0
		for i in range(n):
			res.append(cls(data, pos))
			pos += res[-1].amt
			inc += res[-1].amt
		return res, inc
	def readName(self, data, pos):
		start = data.index('{', pos)
		self.name = reduce(lambda x,y: x + '_' + y if y != '' and x != '' else x + y, data[pos:start] + [''])
		self.amt += start - pos
		#print 'Name:', self.name
		return start - pos
		