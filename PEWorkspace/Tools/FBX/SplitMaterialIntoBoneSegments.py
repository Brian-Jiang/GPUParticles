
import os.path
from DisplayCommon import *
from fbx import FbxLayerElement
from fbx import FbxSurfaceMaterial
from fbx import FbxLayeredTexture
from fbx import FbxTexture
from DisplayMaterial import DisplayMaterial
from DisplayTexture  import DisplayTexture, DisplayTextureInfo  
from DisplayLink     import DisplayLink
from DisplayShape    import DisplayShape
from fbx import FbxPropertyString
MaxJoints = 16
def FindBestBone(tris, jointToVerts, finalVertices, curBones):
    if len(curBones) == MaxJoints:
        return -1
    #rate all bones and choose one to add
    newBone = -1
    maxTriAdd = -1
    for joint in jointToVerts.keys():
        numTrianglesAdd = {}
        triAddRating = 0
        numVertDependenciesAdd = 0
        numJointDependenciesAdd = 0
        jointDepAdd = []
        for numBonesKey in sorted(tris.keys()):
            if numBonesKey == 0: continue
            for tri in tris[numBonesKey]:
                if joint in tri['joints']:
                    jointsLeft = len(tri['joints']) - 1
                    if not numTrianglesAdd.has_key(jointsLeft): numTrianglesAdd[jointsLeft] = 0
                    triAddRating += 1.0 / len(tri['joints']) # 1 for triangle that will just be added, 1/2 for triangle that needs one more vert, so on..
                    numTrianglesAdd[jointsLeft] += 1
                    for vertexIndex in tri['indices']:
                        v = finalVertices[vertexIndex]
                        for inf in v.influences:
                            infJoint = inf[1]
                            if infJoint != joint and not infJoint in curBones:
                                #this joint would have to be added to curBones
                                if not infJoint in jointDepAdd:
                                    jointDepAdd.append(infJoint)
        
        if triAddRating > maxTriAdd:
            newBone = joint
            maxTriAdd = triAddRating
    print "    New bone found: %d tri add rating: %f" % (newBone, maxTriAdd)
    
    return newBone

def FixupLeftoverTris(tris, jointToVerts, finalVertices, curJoints):
    vertIndexesToDuplicate = {}
    for numBonesKey in sorted(tris.keys()):
        if numBonesKey == 0: continue
        for tri in tris[numBonesKey]:
            for vertexIndex in tri['indices']:
                v = finalVertices[vertexIndex]
                for inf in v.influences:
                    infJoint = inf[1]
                    if inf[-1] != -1:
                        #having local index != -1 means this vertex is used by current set of triangles
                        #which means we have to make a copy of this vertex
                        if not infJoint in curJoints:
                            print "ERROR: this should not happen! somehow local bone index != -1 yet the bone is not part of current set"
                            raw_input()
                        if not vertIndexesToDuplicate.has_key(vertexIndex): vertIndexesToDuplicate[vertexIndex] = []
                        vertIndexesToDuplicate[vertexIndex].append(tri)
    #create new vertices and assign them to triangles
    for dupVertexIndex in vertIndexesToDuplicate.keys():
        newVertex = FinalVertex(finalVertices[dupVertexIndex])
        
        finalVertices.append(newVertex)
        for inf in newVertex.influences:
            jointToVerts[inf[1]].append(len(finalVertices) - 1)
            inf[-1] = -1 #clear local joint ref
        print "    Adding new vertex..."
        for tri in vertIndexesToDuplicate[dupVertexIndex]:
            for i in range(3):
                vertexIndex = tri['indices'][i]
                if vertexIndex == dupVertexIndex:
                    tri['indices'][i] = len(finalVertices) - 1
        
def SplitMaterialIntoBoneSegments(materialTris, material, finalVertices, matIndex, meshIndex, logLeve):
    totalTrisLeft = len(materialTris)/3
    totalTrisBeganWith = totalTrisLeft
    print "Step 6.%d.7.%d  SplitMaterialIntoBoneSegments() Entry: total tris: %d material %s" % (meshIndex, matIndex, totalTrisLeft, material['name'])
    #store triangles as [tri + how many new joints needed] 
    tris = {} #keyed by num of joints
    jointToVerts = {}
    iVertIndex = 0
    
    processedTris = [] #list of tris that have been processed. used for error checking
    
    verticesWithNoInfluences = []
    for v in finalVertices:
        if len(v.influences) == 0:
            verticesWithNoInfluences.append(v)
        
        for inf in v.influences:
            joint = inf[1]
            if not jointToVerts.has_key(joint): jointToVerts[joint] = []
            jointToVerts[joint].append(joint)

    if len(verticesWithNoInfluences) > 0 and len(verticesWithNoInfluences) < len(finalVertices):
        #some but not all vertices don't have influences. we don't like that..
        #it might be possible but we print out warning for it because it probably was not planned.
        #if all of them have no influences, that means its just not a skinned mesh, but regular mesh
        for v in verticesWithNoInfluences:
            print "ERROR: vertex does not have any influences!"
            v.printToScreen()
            
    indicesCheck = {}
    for iv in xrange(len(finalVertices)):
        indicesCheck[iv] = 0

    jointsUsed = {}
    
    for iTri in range(len(materialTris)/3):
        tri = {}
        tri['indices'] = [materialTris[iVertIndex], materialTris[iVertIndex+1], materialTris[iVertIndex+2]] 
        tri['joints'] = []
        tri['index'] = iTri
        
        for index in tri['indices']:
            indicesCheck[index] += 1
            v = finalVertices[index]
            for i in v.influences:
                if not i[1] in tri['joints']:
                    tri['joints'].append(i[1])
        tri['originalJoints'] = tri['joints'][:]
        if not tris.has_key(len(tri['joints'])): tris[len(tri['joints'])] = []
        tris[len(tri['joints'])].append(tri)
        iVertIndex += 3

    numVerticesUsedByThisMat = 0
    for iv in xrange(len(finalVertices)):
        if indicesCheck[iv] > 0:
            numVerticesUsedByThisMat += 1
    
    print("Num of vertices used by this mat is %d" % (numVerticesUsedByThisMat, ))
    print("Pre-Validation Completed")
    raw_input()

    boneSegments = []
    curBones = []
    curTris = []
    validationTrisAdded = 0
    while totalTrisLeft > 0:
        canAddBones = MaxJoints - len(curBones)
        
        print "    Iteration: Starting out with %d tris:" % totalTrisLeft
        numJointsStr = ""
        numTrisStr = ""
        numTrisCheck = 0
        for numBonesKey in sorted(tris.keys()):
            numJointsStr += "%d " % numBonesKey
            numTrisStr += "%d " % len(tris[numBonesKey])
            numTrisCheck += len(tris[numBonesKey])
        print "    %s joints required by %s triangles" % (numJointsStr, numTrisStr)
        if numTrisCheck != totalTrisLeft:
            print "VALIDATION ERROR: somehow number of leftover triangles is not equal to sum of sorted tris"
            raw_input()
    
        newBone = -1
        for numBonesKey in sorted(tris.keys()):
            if numBonesKey == 0:
                #add all tris that require 0 new joints
                for tri in tris[0]:
                    curTris.append(tri)
                    totalTrisLeft -= 1
                    #adjust influence local indices
                    for index in tri['indices']:
                        v = finalVertices[index]
                        for i in v.influences:
                            i[-1] = curBones.index(i[1])
                            
        # we need to find a set of bones we can add to current joint segment that will result in
        # adding triangles to segment that use the vertices that use the bones
        newBone = FindBestBone(tris, jointToVerts, finalVertices, curBones) if canAddBones > 0 and totalTrisLeft > 0 else -1
        
        if newBone != -1:
            curBones.append(newBone)
            
        if newBone == -1:
            #could not add another bone to list
            if len(curBones) == 0 and len(curTris) == 0:
                print "ERROR: could not put any triangles into a bone segment. Do triangles have more than max number of joint influences?"
                break
            boneSegments.append({'joints':curBones, 'tris':curTris})
            validationTrisAdded += len(curTris)
            if totalTrisLeft == 0:
                #we are done
                break
            else:
                #starting new bone segment
                
                #we need to fixup triangles that are affected by vertices used in this joint segment but that are not in this joint segment themselves
                #we should cerate new vertices for all such triangles
                FixupLeftoverTris(tris, jointToVerts, finalVertices, curBones)
                
                curBones = []
                for processedTri in curTris:
                    processedTriIndex = processedTri['index']
                    if processedTriIndex in processedTris:
                        print "ERROR: This triangle has already been processed!"
                        raw_input()
                    processedTris.append(processedTriIndex)
                curTris = []
                
    
        #update out tri dict considering the new bone
        #some tris will move up in key since they use current bone
        newTris= {}
        for numBonesKey in sorted(tris.keys()):
            if numBonesKey == 0: continue
            #add bone to list and relax all
            for tri in tris[numBonesKey]:
                if newBone != -1:
                    if newBone in tri['joints']:
                        #print "New bone %d found in triangle:"%newBone, tri
                        tri['joints'].remove(newBone)
                        
                else:
                    #we are starting new segment so we need to restore joints of triangles and start removing joints one by one again
                    tri['joints'] = tri['originalJoints'][:]
                if not newTris.has_key(len(tri['joints'])): newTris[len(tri['joints'])] = []
                newTris[len(tri['joints'])].append(tri)
        tris = newTris
    print "  SplitMaterialIntoBoneSegments(): Ended up with %d bone segments:" % len(boneSegments)
    i = 0
    indicesCheck = {}
    for iv in xrange(len(finalVertices)):
        indicesCheck[iv] = 0

    resNumTris = 0
    for boneSegment in boneSegments:
        print "    [%d]: %d joints %d tris" % (i, len(boneSegment['joints']), len(boneSegment['tris']))
        # need to validate that local joint indices match global joint indices
        resNumTris += len(boneSegment['tris'])
        for tri in boneSegment['tris']:
            for vertexIndex in tri['indices']:
                indicesCheck[vertexIndex] += 1
                v = finalVertices[vertexIndex]
                for inf in v.influences:
                    if inf[1] != boneSegment['joints'][ inf[-1]]:
                        print "ERROR: local joint index does not match global index!"
                        print "ERROR: segment joints:", boneSegment['joints']
                        print "ERROR: vertex:"
                        v.printToScreen()
                        raw_input()
                          
        i += 1
    numVerticesUsedByThisMat = 0
    for iv in xrange(len(finalVertices)):
        if indicesCheck[iv] > 0:
            numVerticesUsedByThisMat += 1
    
    print "Num of vertices used by this material now is %d" % numVerticesUsedByThisMat

    if validationTrisAdded != totalTrisBeganWith:
        print "VALIDATION ERROR: we somehow added less triangles than we had: %d / %d" % (validationTrisAdded, totalTrisBeganWith)
        raw_input()
    print "  Result Number Triangles: %d" % (resNumTris,)
    print "  Finished Validation"

    return boneSegments