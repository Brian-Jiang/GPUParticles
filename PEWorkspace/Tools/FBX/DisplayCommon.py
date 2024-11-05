"""

 Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 
 
"""

def DisplayString(pHeader, pValue="" , pSuffix=""):
    lString = pHeader
    lString += str(pValue)
    lString += pSuffix
    print(lString)

def DisplayBool(pHeader, pValue, pSuffix=""):
    lString = pHeader
    if pValue:
        lString += "true"
    else:
        lString += "false"
    lString += pSuffix
    print(lString)

def DisplayInt(pHeader, pValue, pSuffix=""):
    lString = pHeader
    lString += str(pValue)
    lString += pSuffix
    print(lString)

def DisplayDouble(pHeader, pValue, pSuffix=""):
    print("%s%f%s" % (pHeader, pValue, pSuffix))

def Display2DVector(pHeader, pValue, pSuffix=""):
    print("%s%f, %f%s" % (pHeader, pValue[0], pValue[1], pSuffix))

def Display3DVector(pHeader, pValue, pSuffix=""):
    print("%s%f, %f, %f%s" % (pHeader, pValue[0], pValue[1], pValue[2], pSuffix))

def Vector3DToFile(f, pValue):
    if pValue:
        f.write("%f %f %f\n" % (pValue[0], pValue[1], pValue[2]))
    else:
        f.write("%f %f %f\n" % (0, 0, 0))

def Vector2DToFile(f, pValue):
    if pValue:
        f.write("%f %f\n" % (pValue[0], pValue[1]))
    else:
        f.write("%f %f\n" % (0, 0))
def Display4DVector(pHeader, pValue, pSuffix=""):
    print("%s%f, %f, %f, %f%s" % (pHeader, pValue[0], pValue[1], pValue[2], pValue[3], pSuffix))

def DisplayColor(pHeader, pValue, pSuffix=""):
    print("%s%f (red), %f (green), %f (blue)%s" % (pHeader, pValue.mRed, pValue.mGreen, pValue.mBlue, pSuffix))
def V2ToList(v2):
    return [v2[0], v2[1]]
def V3ToList(v3):
    return [v3[0], v3[1], v3[2]]
def ColorToList(c):
    return [c.mRed, c.mGreen, c.mBlue]
def V4ToList(v4):
    return [v4[0], v4[1], v4[2], v4[3]]

from copy import deepcopy 

from copy import copy
EPSILON = 0.000001
def V2Same(v0, v1):
    return abs(v0[0] - v1[0]) <= EPSILON and abs(v0[1] - v1[1]) <= EPSILON
def V3Same(v0, v1):
    return V2Same(v0, v1) and abs(v0[2] - v1[2]) <= EPSILON
def V4Same(v0, v1):
    return V3Same(v0, v1) and abs(v0[3] - v1[3]) <= EPSILON
class FinalVertex:
    def __init__(self, cpy = None):
        self.position = deepcopy(cpy.position) if cpy else None
        self.texCoords = deepcopy(cpy.texCoords) if cpy else [None] * 8
        self.normal = deepcopy(cpy.normal) if cpy else None
        self.tangent = deepcopy(cpy.tangent) if cpy else None
        self.influences = deepcopy(cpy.influences) if cpy else []
        self.useCount = 0
    def setPosition(self, index, value):
        if index != 0:
            print("PE: Error: there is only one position in vertex. Index given: %d" % index)
        else:
            if self.position == None or V3Same(self.position, value):
                self.position = value
                return True
        return False
    def setNormal(self, index, value, force = False):
        if index != 0:
            print("PE: Error: there is only one normal in vertex. Index given: %d" % index)
        else:
            if self.normal == None or V3Same(self.normal, value) or force or self.useCount == 1:
                self.normal = value
                return True
        return False
    def setTangent(self, index, value, force = False):
        if index != 0:
            print("PE: Error: there is only one tangent in vertex. Index given: %d" % index)
        else:
            if self.tangent == None or V3Same(self.tangent, value) or force or self.useCount == 1:
                self.tangent = value
                return True
        return False
    def setTexCoord(self, index, value, force = False):
        if index >= 8:
            print("PE: Error: there are max of 8 tex coord sets. Index given: %d" % index)
        else:
            if self.texCoords[index] == None or V2Same(self.texCoords[index], value) or force or self.useCount == 1:
                self.texCoords[index] = value
                return True
        return False
    def setInfluence(self, boneName, boneIndex, weight):
        self.influences.append([boneName, boneIndex, weight, -1]) #-1 for local bone index
        return True
    def getInfluenceSum(self):
        sum = 0
        for i in self.influences:
            sum += i[2]
        return sum
    def printToScreen(self):
        print("Position :", self.position)
        print("TexCoords:", self.texCoords)
        print("Normal   :", self.normal)
        print("Tangent  :", self.tangent)
        print("Skin weights num: %d adding up to %f" % (len(self.influences), self.getInfluenceSum()))
        summary = ''
        for i in self.influences:
            summary += "%s[%d]:%f local[%d] " % (i[0], i[1], i[2], i[3])
        print(summary)
        print("Use Count:", self.useCount)
    def logToScreen(self, logger, doPrint):
        logger.AddLine("Position : %s" % str(self.position), doPrint)
        logger.AddLine("TexCoords: %s" % str(self.texCoords), doPrint)
        logger.AddLine("Normal   : %s" % str(self.normal), doPrint)
        logger.AddLine("Tangent  : %s" % str(self.tangent), doPrint)
        logger.AddLine("Skin weights num: %d adding up to %f" % (len(self.influences), self.getInfluenceSum()), doPrint)
        summary = ''
        for i in self.influences:
            summary += "%s[%d]:%f local[%d] " % (i[0], i[1], i[2], i[3])
        logger.AddLine(summary, doPrint)
        logger.AddLine("Use Count: %d" % self.useCount, doPrint)
    
