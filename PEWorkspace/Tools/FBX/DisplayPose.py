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
#from fbx import FbxCamera
from fbx import FbxMatrix
def DisplayPose(pScene, skelRoots, skelDirectMaps, meshNames, logLevel):
    lPoseCount = pScene.GetPoseCount()
    print "Total poses: %d" % lPoseCount
    if lPoseCount == 0:
        print "ERROR: there are no bind poses in the file"
    
    meshToBindMatrixMap = {}
    
    
    for i in range(lPoseCount):
        lPose = pScene.GetPose(i)

        lName = lPose.GetName()
        print ("Pose Name: %s, is bind pose: %s number of items in pose: %s" % (lName, str(lPose.IsBindPose()), str( lPose.GetCount())))

        for j in range(lPose.GetCount()):
            lName = lPose.GetNodeName(j).GetCurrentName()
            
            if logLevel > 0:
                DisplayString("    Matrix value: ","")

            float16 = []
            lMatrixValue = ""
            for k in range(4):
                lMatrix = lPose.GetMatrix(j)
                isLocal = lPose.IsLocalMatrix(j)
                lRow = lMatrix.GetRow(k)

                lRowValue = "%9.4f %9.4f %9.4f %9.4f\n" % (lRow[0], lRow[1], lRow[2], lRow[3])
                float16 = float16 + [lRow[0], lRow[1], lRow[2], lRow[3]]
                lMatrixValue += "        " + lRowValue

            if logLevel > 0:
                DisplayString("", lMatrixValue)
                DisplayString("    Item name: ", lName)
                
            boneName = lName
            skelDirectMap = None
    
            if lPose.IsBindPose():
                #will export this pose as skeleton
                
                #choose skel based on first boneName encountered
                for iSkel in range(len(skelDirectMaps)):
                    directMap = skelDirectMaps[iSkel]
                    if directMap.has_key(boneName):
                        skelDirectMap = skelDirectMaps[iSkel]
                        break
                
                if skelDirectMap == None:
                    #havent figure out skeleton to use yet, or the joint is nto in selected skeleton
                    if boneName in meshNames:
                        #meshes can have bind transform. because PrimeEngine doesnt have a mesh transform, we will
                        #multiply the bind pose transform into the mesh vertices
                        print '    NOTE: Mesh "%s" has bind pose transform.' % (boneName,)
                        print '    We will multiply all control points by it'
                        print '    This potetnailly will make mesh unusable as non skinned mesh'
                        meshName = boneName
                        meshToBindMatrixMap[meshName] = FbxMatrix(lPose.GetMatrix(j))
                    else:
                        print '    ERROR: the joint "%s" is not a mesh in file and is not in any skeleton' % (boneName,)
                else:
                    skelDirectMap[boneName]['matrix'] = float16
            
            if not lPose.IsBindPose():
                # Rest pose can have local matrix
                if logLevel > 0:
                    DisplayBool("    Is local space matrix: ", lPose.IsLocalMatrix(j))

        print "    Checking if all joins of all skeletons' joints are in bind pose.."
        for skelDirectMap in skelDirectMaps:
            #check if we have bind pose matrices for all joints of the skeleton
            joinntsWithNoBindPoseXForm = []
            for directKey in skelDirectMap.keys():
                if type(directKey) is int:
                    continue #we also key the dict by index, but these values are duplicates
                if not skelDirectMap[directKey].has_key('matrix'):
                    joinntsWithNoBindPoseXForm.append(directKey)
            if len(joinntsWithNoBindPoseXForm):
                print '    WARNING: these joints in have no bind pose matrix (expected if no verts are influenced by them):'
                print '    %s' %    str(joinntsWithNoBindPoseXForm)
            
    lPoseCount = pScene.GetCharacterPoseCount()

    for i in range(lPoseCount):
        lPose = pScene.GetCharacterPose(i)
        lCharacter = lPose.GetCharacter()

        if not lCharacter:
            break

        DisplayString("Character Pose Name: ", lCharacter.mName.Buffer())

        lNodeId = eCharacterHips

        while lCharacter.GetCharacterLink(lNodeId, lCharacterLink):
            lAnimStack = None
            if lAnimStack == None:
                lScene = lCharacterLink.mNode.GetScene()
                if lScene:
                    lAnimStack = lScene.GetMember(FBX_TYPE(FbxAnimStack), 0)

            lGlobalPosition = lCharacterLink.mNode.GetGlobalFromAnim(KTIME_ZERO, lAnimStack)

            DisplayString("    Matrix value: ","")

            lMatrixValue = ""
            for k in range(4):
                lRow = lGlobalPosition.GetRow(k)

                lRowValue = "%9.4f %9.4f %9.4f %9.4f\n" % (lRow[0], lRow[1], lRow[2], lRow[3])
                lMatrixValue += "        " + lRowValue

            DisplayString("", lMatrixValue)

            lNodeId = ECharacterNodeId(int(lNodeId) + 1)
    return meshToBindMatrixMap