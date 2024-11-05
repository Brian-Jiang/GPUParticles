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

from DisplayCommon import *
from fbx import FbxDeformer
from fbx import FbxAMatrix
#from fbx import FbxLayeredTexture
#from fbx import FbxTexture
import math
showInfo = False
def DisplayLink(pGeometry, finalVertices, originalControlPointToNewVerticesMapping, skelRoots, skelDirectMaps, meshIndex, logLevel):
    DisplayString("Step 6.%d.6 DisplayLink entry... (looking for eSkin deformers of Geometry)" % meshIndex)
   
    #Display cluster now
    lSkinCount=pGeometry.GetDeformerCount(FbxDeformer.eSkin)
    print "    %d eSkin deformers found" % (lSkinCount,)
    
    skelRoot = None # will be chosen from skelRoots
    skelDirectMap = None
    unusedJointsInSkinBind = None #stores a list of unused bones by skin clusters
    
    #lLinkCount = pGeometry.GetLinkCount()
    for i in range(lSkinCount):
        lClusterCount = pGeometry.GetDeformer(i, FbxDeformer.eSkin).GetClusterCount()
        print "    eSkin deformer has %d clusters" % (lClusterCount,)
        for j in range(lClusterCount):
            
            lCluster = pGeometry.GetDeformer(i, FbxDeformer.eSkin).GetCluster(j)
            #lLink = pGeometry.GetLink(i)

            lClusterModes = [ "Normalize", "Additive", "Total1" ]

            print "      Cluster", j, "Mode: ", lClusterModes[lCluster.GetLinkMode()], "Link Name:", lCluster.GetLink().GetName() if lCluster.GetLink() else "N\\A"
            
            #lString1 = "        Link Indices: "
            #lString2 = "        Weight Values: "

            lIndexCount = lCluster.GetControlPointIndicesCount()
            lIndices = lCluster.GetControlPointIndices()
            lWeights = lCluster.GetControlPointWeights()

            boneName = lCluster.GetLink().GetName()
            if skelRoot == None:
                #choose skel based on first boneName encountered
                for iSkel in range(len(skelDirectMaps)):
                    directMap = skelDirectMaps[iSkel]
                    if directMap.has_key(boneName):
                        skelRoot = skelRoots[iSkel]
                        skelDirectMap = skelDirectMaps[iSkel]
                        unusedJointsInSkinBind = skelDirectMap.keys()[:]
                        break
            if skelRoot == None:
                print 'ERROR: could not find skeleton for bone "%s"' % boneName
            joint = None
            if skelDirectMap:
                joint = skelDirectMap[boneName]
                
            for k in range(lIndexCount):
                #since we could have duplicated control points, there could be multiple final vertices corresponding to this control point
                originalToFinalVerts = originalControlPointToNewVerticesMapping[lIndices[k]]
                if originalToFinalVerts:
                    #it is possible originalToFinalVerts is None in case the vertex is actually not used by any tris
                    for iFV in originalToFinalVerts:
                        finalVertices[iFV].setInfluence(boneName, skelDirectMap[boneName]['index'], lWeights[k])
                    if boneName in unusedJointsInSkinBind:
                        unusedJointsInSkinBind.remove(boneName)
                #lString1 += str(lIndices[k])
                #lString2 += str(lWeights[k])

                #if k < lIndexCount - 1:
                #    lString1 += ", "
                #    lString2 += ", "

            #print(lString1)
            #print(lString2)

            lMatrix = FbxAMatrix()
            
            if showInfo: Display3DVector("        Transform Translation: ", lMatrix.GetT())
            if showInfo: Display3DVector("        Transform Rotation: ", lMatrix.GetR())
            if showInfo: Display3DVector("        Transform Scaling: ", lMatrix.GetS())

            lMatrix = lCluster.GetTransformLinkMatrix(lMatrix)
            if showInfo: Display3DVector("        Transform Link Translation: ", lMatrix.GetT())
            if showInfo: Display3DVector("        Transform Link Rotation: ", lMatrix.GetR())
            if showInfo: Display3DVector("        Transform Link Scaling: ", lMatrix.GetS())
            
            
            if joint:
                m = [
                    lMatrix.Get(0,0), lMatrix.Get(0,1), lMatrix.Get(0,2), lMatrix.Get(0,3),
                    lMatrix.Get(1,0), lMatrix.Get(1,1), lMatrix.Get(1,2), lMatrix.Get(1,3),
                    lMatrix.Get(2,0), lMatrix.Get(2,1), lMatrix.Get(2,2), lMatrix.Get(2,3),
                    lMatrix.Get(3,0), lMatrix.Get(3,1), lMatrix.Get(3,2), lMatrix.Get(3,3)
                ]
                different = False
                epsilon = 0.001
                for mv in xrange(16):
                    if math.fabs(m[mv] - joint['matrix'][mv]) > epsilon:
                        different = True
                if different:
                    print ("    Warning: The Link Transform did not match the bind pose matrix of the %s joint" % (boneName,))
                    print ("    If we keep current bind pose, this cluster's skin weights won't work")
                    print ("    Setting joint bind pose matrix to this Link Transform:")
                    
                    print ("    Old Joint Transform:")
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][0], joint['matrix'][1], joint['matrix'][2], joint['matrix'][3]))
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][4], joint['matrix'][5], joint['matrix'][6], joint['matrix'][7]))
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][8], joint['matrix'][9], joint['matrix'][10], joint['matrix'][11]))
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][12], joint['matrix'][13], joint['matrix'][14], joint['matrix'][15]))
            
                    joint['matrix'][0], joint['matrix'][1], joint['matrix'][2], joint['matrix'][3] = lMatrix.Get(0,0), lMatrix.Get(0,1), lMatrix.Get(0,2), lMatrix.Get(0,3)
                    joint['matrix'][4], joint['matrix'][5], joint['matrix'][6], joint['matrix'][7] = lMatrix.Get(1,0), lMatrix.Get(1,1), lMatrix.Get(1,2), lMatrix.Get(1,3)
                    joint['matrix'][8], joint['matrix'][9], joint['matrix'][10], joint['matrix'][11] = lMatrix.Get(2,0), lMatrix.Get(2,1), lMatrix.Get(2,2), lMatrix.Get(2,3)
                    joint['matrix'][12], joint['matrix'][13], joint['matrix'][14], joint['matrix'][15] = lMatrix.Get(3,0), lMatrix.Get(3,1), lMatrix.Get(3,2), lMatrix.Get(3,3)
            
                    print ("    New Joint Transform:")
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][0], joint['matrix'][1], joint['matrix'][2], joint['matrix'][3]))
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][4], joint['matrix'][5], joint['matrix'][6], joint['matrix'][7]))
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][8], joint['matrix'][9], joint['matrix'][10], joint['matrix'][11]))
                    print ("      %.2f %.2f %.2f %.2f " % (joint['matrix'][12], joint['matrix'][13], joint['matrix'][14], joint['matrix'][15]))
            
            if lCluster.GetAssociateModel():
                lMatrix = lCluster.GetTransformAssociateModelMatrix(lMatrix)
                if showInfo: DisplayString("        Associate Model: ", lCluster.GetAssociateModel().GetName())
                if showInfo: Display3DVector("        Associate Model Translation: ", lMatrix.GetT())
                if showInfo: Display3DVector("        Associate Model Rotation: ", lMatrix.GetR())
                if showInfo: Display3DVector("        Associate Model Scaling: ", lMatrix.GetS())
    print "  End DisplayLink()"
    return skelRoot, skelDirectMap, unusedJointsInSkinBind
