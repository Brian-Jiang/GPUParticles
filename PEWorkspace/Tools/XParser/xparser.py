import xconv
from sys import stdin
import os
import threading
assetsOutPath = ''
PYENGINE_BONE_SEGMENTATION = 14
def getMatOutFullPathFilename(mat, parsed):
    path = globals()['assetsOutPath'] + 'Materials/'
    outfilename = parsed.strippedFileName.lower() + '_' + mat.name.lower() + '_m' + '.mata'
    res = path + outfilename
    return res
def getMeshOutFullPathFilename(mesh, parsed):
    path = globals()['assetsOutPath']+ 'Meshes/'
    mesh_file = parsed.strippedFileName.lower() + '_' + mesh.name.lower() + '_mesh.mesha'
    return path + mesh_file
def getSkinOutFullPathFilename(skin, parsed):
    path = globals()['assetsOutPath'] + 'Skins/'
    outfilename = parsed.strippedFileName.lower() + '_' + skin.name.lower() + '_skin' + '.skina'
    return path + outfilename, outfilename
    
def exportMaterial(mat, parsed):
    fullPath = getMatOutFullPathFilename(mat, parsed)
    dirPath = os.path.dirname(fullPath)
    print '    exporting material:', fullPath
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath)  
    f = open(getMatOutFullPathFilename(mat, parsed), 'w')
    f.write('MATERIAL\n')
    f.write(str(mat.faceColor.red) + ' ' + str(mat.faceColor.green) + ' ' + str(mat.faceColor.blue) + ' ' + str(mat.faceColor.alpha) + '\n')
    f.write(str(mat.power) + '\n')
    f.write(str(mat.specularColor.red) + ' ' + str(mat.specularColor.green) + ' ' + str(mat.specularColor.blue) + '\n')
    f.write(str(mat.emissiveColor.red) + ' ' + str(mat.emissiveColor.green) + ' ' + str(mat.emissiveColor.blue) + '\n')
    f.write(str(len(mat.textures)) + '\n')
    for texture in mat.textures:
        f.write('COLOR\n')
        basename = os.path.basename(os.path.normpath(texture.filename))
        noExt, ext = os.path.splitext(basename)
        f.write(noExt + '.dds' + '\n')
    f.close()
    
def exportRigidMesh(mesh, parsed, skinWeights = None):
    path = globals()['assetsOutPath'] + 'VertexBuffers/'
    if not os.path.exists(path):
        print "Creating %s folder" % path
        os.mkdir(path)  
    outfilename = mesh.name.lower()
    vertex_buffer = parsed.strippedFileName.lower() + '_' + outfilename + '_vb.bufa'
    print '    exporting rigid mesh:'
    print '     ', path + vertex_buffer
    f = open(path + vertex_buffer, 'w')
    f.write('POSITION_BUFFER\n')
    f.write(str(len(mesh.mergedVertexData)))
    f.write('\n')
    for wide in mesh.mergedVertexData:
        v = wide[0]
        f.write(str(v.x) + ' ' + str(v.y) + ' ' + str(v.z) + '\n')
    f.close()
    if mesh.normals != None:
        path = globals()['assetsOutPath'] + 'NormalBuffers/'
        if not os.path.exists(path):
            print "Creating %s folder" % path
            os.mkdir(path)  
        normals_buffer = parsed.strippedFileName.lower() + '_' + outfilename + '_nb.bufa'
        print '     ', path + normals_buffer
        f = open(path + normals_buffer, 'w')
        f.write('NORMAL_BUFFER\n')
        f.write(str(len(mesh.mergedVertexData)))
        f.write('\n')
        for wide in mesh.mergedVertexData:
            n = wide[2]
            f.write(str(n.x) + ' ' + str(n.y) + ' ' + str(n.z) + '\n')
        f.close()
    else:
        print '      Warning: no normal data'
    if mesh.tangents != None:
        path = globals()['assetsOutPath'] + 'TangentBuffers/'
        if not os.path.exists(path):
            print "Creating %s folder" % path
            os.mkdir(path)  
        tangent_buffer = parsed.strippedFileName.lower() + '_' + outfilename + '_tb.bufa'
        print '     ', path + tangent_buffer
        f = open(path + tangent_buffer, 'w')
        f.write('TANGENT_BUFFER\n')
        f.write(str(len(mesh.mergedVertexData)))
        f.write('\n')
        for wide in mesh.mergedVertexData:
            n = wide[3]
            f.write(str(n[0]) + ' ' + str(n[1]) + ' ' + str(n[2]) + '\n')
        f.close()
    else:
        print '      Warning: no tangent data'
    path = globals()['assetsOutPath'] + 'IndexBuffers/'
    if not os.path.exists(path):
        print "Creating %s folder" % path
        os.mkdir(path)  
    index_buffer = parsed.strippedFileName.lower() + '_' + outfilename + '_ib.bufa'
    print '     ', path + index_buffer
    f = open(path + index_buffer, 'w')
    f.write('INDEX_BUFFER\n')
    f.write('3\n') # triangles
    #first pass to get rid of quads, pentagons, etc
    newFacesForMaterials = []
    total = 0
    for facesForMat in mesh.facesByMatIndex:
        totalPerMaterial = 0
        newFacesForMat = []
        for face in facesForMat:
            if len(face.faceVertexIndices) == 3:
                newFacesForMat.append(face.faceVertexIndices[:])
                total = total + 1
            elif len(face.faceVertexIndices) == 4:
                newFacesForMat.append(face.faceVertexIndices[:3])
                newFacesForMat.append([face.faceVertexIndices[2], face.faceVertexIndices[3], face.faceVertexIndices[0]])
                total = total + 2
            elif len(face.faceVertexIndices) == 5:
                newFacesForMat.append(face.faceVertexIndices[:3])
                newFacesForMat.append([face.faceVertexIndices[2], face.faceVertexIndices[3], face.faceVertexIndices[0]])
                newFacesForMat.append([face.faceVertexIndices[3], face.faceVertexIndices[4], face.faceVertexIndices[0]])
                total = total + 3
        newFacesForMaterials.append(newFacesForMat)
    #print out triangles
    #at this point newFacesForMat store lists of triangle indices separated by material
    # we now need to look into skin weight set to further separate the sets based on which bones they use
    f.write(str(total) + '\n') # total #of faces
    f.write(str(len(newFacesForMaterials)) + '\n') # num materials in the mesh (mesh is partitioned by mat)
    if skinWeights == None:
        # no skin weight sets were given, can just export what have
        for facesForMat in newFacesForMaterials:
            f.write('1\n') # using one joint segment (that is all faces using this material are usign same bone set. for meshes it is always 1
            f.write('0\n') # not using bones references (meshes only)
            f.write(str(len(facesForMat)) + '\n') # num faces for  material
            for face in facesForMat:
                for i in face:
                    f.write(str(i) + ' ')
                f.write('\n')
    else:
        g_BoneSegmentation = globals()["PYENGINE_BONE_SEGMENTATION"]
        #skin weights were given
        jointSeparatedFacesForMaterials = []
        jointSeparations = []
        for facesForMat in newFacesForMaterials:
            skinWeightSeparatedFacesForMaterial = [] #stores further segmentation of a piece
            jointSets = []
            # we will look into each triangle and create a dependency set for it
            # then we will combine existing triangles into sets of triangles and with
            # joint sets < g_BoneSegmentation joints
            iface = 0
            for face in facesForMat:
                jointSet = set()
                for i in face:
                    # skinWeights[i] = skin weights for vertex i
                    faceSet = set([sw[0] for sw in skinWeights[i]])
                    jointSet = jointSet.union(faceSet) # 0th = joint id, 1st = weight
                # find one with least amount of moodification to bone set
                # and add the face to it
                indexOfSetToAddTo = -1
                minDifFound = 0x7FFFFFFF
                for iset in xrange(len(jointSets)):
                    difference = jointSet.difference(jointSets[iset]) #diference of an existing set and new set
                    l = len(difference)
                    if l < minDifFound and ( l + len(jointSets[iset]) <= g_BoneSegmentation ):
                        # we only look at this value if combined with the current set it does not excceed max allowed number of bones
                        minDifFound = l
                        indexOfSetToAddTo = iset
                        if l == 0:  #no difference -> can break right away, no need to search further
                            break
                if indexOfSetToAddTo != -1:
                    #found a set to add to
                    jointSets[indexOfSetToAddTo] = jointSets[indexOfSetToAddTo].union(jointSet)
                    skinWeightSeparatedFacesForMaterial[indexOfSetToAddTo].append(face)
                else:
                    #add new set
                    jointSets.append(jointSet)
                    skinWeightSeparatedFacesForMaterial.append([face])
                iface += 1
            print "Result of segmenting a set of same material indices:"
            
            f.write(str(len(jointSets)) + '\n') # number of bone segments in this material
            for iset in xrange(len(jointSets)):
                print "set %d with %d faces :" % (iset, len(skinWeightSeparatedFacesForMaterial[iset])), jointSets[iset]
                jointList = list(jointSets[iset])
                jointList.sort()
                f.write(str(len(jointList)) + '\n') # number of bones used in the bone segment
                for jointId in jointList: # 
                    f.write(str(jointId))
                    f.write(' ')
                f.write('\n')
                f.write(str(len(skinWeightSeparatedFacesForMaterial[iset])) + '\n') # num faces for  material
                for face in skinWeightSeparatedFacesForMaterial[iset]:
                    for i in face:
                        f.write(str(i) + ' ')
                        for sw in skinWeights[i]:
                            localBoneIndex = jointList.index(sw[0])
                            if sw[2] != -1 and sw[2] != localBoneIndex:
                                print "PYENGINE ERROR: a vertex [%d] skin weight references different reduced bones (reduced to fit into reisters" % i
                                print "This should be fixed by adding more wide vertices to merged data and referencing new vertex in the processed face"
                                raw_input()
                            sw[2] = localBoneIndex
                    f.write('\n')
        
    f.close()
    #-----------------------MaterialSets-----------------------
    path = globals()['assetsOutPath'] + 'MaterialSets/'
    if not os.path.exists(path):
        print "Creating %s folder" % path
        os.mkdir(path)  
    material_set = parsed.strippedFileName.lower() + '_' + outfilename + '_ms.mseta'
    print '     ', path + material_set
    f = open(path + material_set, 'w')
    f.write('MATERIAL_SET\n')
    f.write(str(len(mesh.materialList.materials)) + '\n') # num materials
    for matname in mesh.materialList.materials:
        f.write(parsed.strippedFileName.lower() + '_'  + matname + '_m' + '.mata' + '\n')
    f.close()
    #-----------------------TexCoordBuffers--------------------
    if mesh.texCoords != None:
        path = globals()['assetsOutPath'] + 'TexCoordBuffers/'
        if not os.path.exists(path):
            print "Creating %s folder" % path
            os.mkdir(path)  
        texcoord_buffer = parsed.strippedFileName.lower() + '_' + outfilename + '_tcb.bufa'
        print '     ', path + texcoord_buffer
        f = open(path + texcoord_buffer, 'w')
        f.write('TEXCOORD_BUFFER\n')
        f.write(str(len(mesh.mergedVertexData)) + '\n')
        for wide in mesh.mergedVertexData:
            coord = wide[1]
            f.write(str(coord.u) + ' ' + str(coord.v) + '\n')
        f.close()
    else:
        print '      Warning: not tex coord data'
    
    path = globals()['assetsOutPath']+ 'Meshes/'
    if not os.path.exists(path):
        print "Creating %s folder" % path
        os.mkdir(path)  
    mesh_file = parsed.strippedFileName.lower() + '_' + outfilename + '_mesh.mesha'
    print '     ', path + mesh_file
    f = open(path + mesh_file, 'w')
    f.write('MESH\n')
    f.write( parsed.strippedFileName.lower() + '_' + outfilename + '_vb' + '.bufa')
    f.write('\n')
    f.write(parsed.strippedFileName.lower() + '_' + outfilename + '_ib' + '.bufa')
    f.write('\n')
    if mesh.texCoords != None:
        f.write(parsed.strippedFileName.lower() + '_' + outfilename + '_tcb' + '.bufa')
    else:
        f.write('none')
    f.write('\n')
    if mesh.normals != None:
        f.write(parsed.strippedFileName.lower() + '_' + outfilename + '_nb' + '.bufa')
    else:
        f.write('none')
    f.write('\n')
    if mesh.tangents != None:
        f.write(parsed.strippedFileName.lower() + '_' + outfilename + '_tb' + '.bufa')
    else:
        f.write('none')
    f.write('\n')
    f.write(parsed.strippedFileName.lower() + '_' + outfilename + '_ms' + '.mseta')
    f.write('\n')
    f.close()
    return mesh_file
def exportSkin(skin, parsed):
    pathFilename, filename = getSkinOutFullPathFilename(skin, parsed)
    print '    exporting Skin:', pathFilename
    dirPath = os.path.dirname(pathFilename)
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath) 
    file = open(pathFilename, 'w')
    file.write('SKIN\n')
    file.write(parsed.strippedFileName.lower() + '_' + skin.name.lower() + '_mesh.mesha\n')
    file.write(parsed.strippedFileName.lower() + '_skeleton.skela\n')
    file.write(parsed.strippedFileName.lower() + '_' + skin.name.lower() + '_skin_weights.swghta\n')
    file.write(parsed.strippedFileName.lower() + '_animation_set' + '.animseta\n')
    file.write(parsed.strippedFileName.lower() + '.sset\n')
    file.write(parsed.strippedFileName.lower() + '.nset\n')
    file.close()
    return filename
    
def preprocessSkinWeights(skin, boneNameToIndex, parsed):
    #path = globals()['assetsOutPath'] + 'SkinWeights/'
    #outfilename = parsed.strippedFileName.lower() + '_' + skin.name.lower() + '_skin_weights' + '.swghta'
    #print '    exporting Skin Weights:', path + outfilename
    #file = open(path + outfilename, 'w')
    #file.write('SKIN_WEIGHTS\n')
    nVertices = skin.nVertices
    #file.write(str(nVertices) + '\n')
    weightsPerVertex = {}
    for iv in range(nVertices):
        weightsPerVertex[iv] = []
    for wset in skin.skinWeightSets:
        boneIndex = boneNameToIndex[wset.boneName]
        for iVertex in range(wset.nVertices):
            vertexIndex = wset.vertexIndices[iVertex]
            vertexWeight = wset.vertexWeights[iVertex]
            weightsPerVertex[vertexIndex].append((boneIndex, vertexWeight))
    sk = weightsPerVertex.keys()[:]
    weightList = [] #store the same info in sorted list rahther than dictionary
    sk.sort()
    for k in sk:
        #file.write(str(len(weightsPerVertex[k])) + '\n')
        weightList.append([])
        for ipair in weightsPerVertex[k]:
            #file.write(str(ipair[0]) + ' ' + str(ipair[1]) + '\n')
            weightList[-1].append( [ipair[0], ipair[1], -1] )
    #file.close()
    return weightList
def exportPreprocessedSkinWeights(skin, weightList, parsed):
    path = globals()['assetsOutPath'] + 'SkinWeights/'
    if not os.path.exists(path):
        print "Creating %s folder" % path
        os.mkdir(path)  
    outfilename = parsed.strippedFileName.lower() + '_' + skin.name.lower() + '_skin_weights' + '.swghta'
    print '    exporting preprocessed Skin Weights:', path + outfilename
    file = open(path + outfilename, 'w')
    file.write('SKIN_WEIGHTS\n')
    nVertices = len(weightList)
    file.write(str(len(weightList)) + '\n')
    for k in xrange(len(weightList)):
        file.write(str(len(weightList[k])) + '\n')
        for ipair in weightList[k]:
            file.write(str(ipair[0]) + ' ' + str(ipair[1]) + ' ' + str(ipair[2]) + '\n')
    file.close()
    return weightList
def exportFrameTransformMatrix(m, file):
    for val in m.frameMatrix.matrix:
        file.write(str(val))
        file.write(' ')
    file.write('\n')
def exportFrameTransformMatrixWithCommas(m, file):
    for val in m.frameMatrix.matrix:
        file.write(str(val))
        file.write(', ')
def exportFramePEUUIDWithNoLastComma(peuuid, file):
    for val in peuuid.peuuidNums[:-1]:
        file.write(str(val))
        file.write(', ')
    file.write(str(peuuid.peuuidNums[-1]))
        
def countBones(root, res):
    res += 1
    for fname in root.topFrameNames:
        f = root.topFrames[fname]
        res = countBones(f, res)
    return res
def exportBones(root, curBoneIndex, file, boneNameToIndex, hierarchyDict, needToExport):
    boneNameToIndex[root.name] = curBoneIndex
    hierarchyDict['name'] = root.name
    hierarchyDict['children'] = []
    if needToExport:
        file.write(root.name + '\n') #bone name
        file.write(str(curBoneIndex)) #bone index
        file.write('\n')
        exportFrameTransformMatrix(root.frameTransformMatrix, file)
        file.write(str(len(root.topFrames)) + '\n') # num of immediate children
    curBoneIndex += 1
    for fname in root.topFrameNames:
        f = root.topFrames[fname]
        hierarchyDict['children'].append({})
        curBoneIndex = exportBones(f, curBoneIndex, file, boneNameToIndex, hierarchyDict['children'][-1], needToExport)
    return curBoneIndex
def getSkeletonOutFullPathFilename(skel, parsed):
    path = globals()['assetsOutPath'] + 'Skeletons/'
    outfilename = parsed.strippedFileName.lower() + '_skeleton' + '.skela'
    return path + outfilename
def exportSkeleton(skel, parsed, needToExport):
    file = None
    if needToExport:
        print 'Exporting skeleton'
        print '    exporting skeleton:', getSkeletonOutFullPathFilename(skel, parsed)
        fullPath = getSkeletonOutFullPathFilename(skel, parsed)
        dirPath = os.path.dirname(fullPath)
        if not os.path.exists(dirPath):
            print "Creating %s folder" % dirPath
            os.mkdir(dirPath)  
    
        file = open(getSkeletonOutFullPathFilename(skel, parsed), 'w')
        file.write('SKELETON\n')
        file.write(str(countBones(skel, 0)) + '\n')
    boneNameToIndex = {}
    hierarchyDict = {}
    exportBones(skel, 0, file, boneNameToIndex, hierarchyDict, needToExport)
    if needToExport:
        file.close()
    return boneNameToIndex, hierarchyDict

def exportAnimations(boneNameToIndex, parsed, needToExport):
    if needToExport:
        path = globals()['assetsOutPath'] + 'AnimationSets/'
        if not os.path.exists(path):
            print "Creating %s folder" % path
            os.mkdir(path)  
        speedSetPath =  globals()['assetsOutPath'] + 'AnimationSpeedSets/'
        if not os.path.exists(speedSetPath):
            print "Creating %s folder" % speedSetPath
            os.mkdir(speedSetPath)  
        nameSetPath =  globals()['assetsOutPath'] + 'AnimationNameSets/'
        if not os.path.exists(nameSetPath):
            print "Creating %s folder" % nameSetPath
            os.mkdir(nameSetPath)  
        outfilename = parsed.strippedFileName.lower() + '_animation_set' + '.animseta'
        speedSetFilename = parsed.strippedFileName.lower() + '.sset'
        nameSetFilename = parsed.strippedFileName.lower() + '.nset'
        print '    exporting animation set:', path + outfilename
        file = open(path + outfilename, 'w')
        speedSetFile = open(speedSetPath + speedSetFilename, 'w')
        nameSetFile = open(nameSetPath + nameSetFilename, 'w')
        file.write('ANIMATION_SET\n')
        speedSetFile.write('DC_V64\n')
        nameSetFile.write('DC_STRING64\n')
    
    indexToBone = {}
    for k in boneNameToIndex:
        indexToBone[boneNameToIndex[k]] = k
    #sk = indexToBone.sort()
    animNames = []
    for k in indexToBone:
        print '--------------------------', k
        frame = parsed.frames[indexToBone[k]]
        for animkey in frame.animations:
            if not animkey in animNames:
                animNames.append(animkey)
    print indexToBone
    animationSetsMaxTimes = {} # store how many frames are in each animation. Take the max, since some joints could potentially skip frames
    animationSetsMinTimes = {} 
    for animName in animNames:
        animationSetsMaxTimes[animName] = 0
        animationSetsMinTimes[animName] = 100000000
    for k in indexToBone:
        frame = parsed.frames[indexToBone[k]]
        for animkey in frame.animations:
            anim = frame.animations[animkey]
            start = anim.animationKeys[0].timedFloatKeys[0].time
            if start < animationSetsMinTimes[animkey]: animationSetsMinTimes[animkey] = start
            end = anim.animationKeys[0].timedFloatKeys[-1].time
            if end > animationSetsMaxTimes[animkey]: animationSetsMaxTimes[animkey] = end
    print 'Animation durations:'
    for animName in animNames:
        print animName+':', animationSetsMinTimes[animName], ':', animationSetsMaxTimes[animName]
    animationSets = {}
    for animName in animNames:
        firstFrameTime = animationSetsMinTimes[animName]
        lastFrameTime = animationSetsMaxTimes[animName]
        animationSets[animName] = []
        for frame in xrange(firstFrameTime, lastFrameTime+1):
            animationSets[animName].append([])
            for joint in indexToBone:
                animationSets[animName][-1].append([]) # each list is a matrix
                jointFrame = parsed.frames[indexToBone[joint]]
                animationSets[animName][-1][joint] = jointFrame.frameTransformMatrix.frameMatrix.matrix[:] #by default assign bind pose matrix
                #jointFrame.frameTransformMatrix.frameMatrix.matrix[:]
                #[[[1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0] for joint in indexToBone] for frame in xrange(firstFrameTime, lastFrameTime+1)] # list of animation keys (float[16]) for all joints [0..n-1]
    
    # fill out the anmation sets structure
    for animName in animNames:
        for k in indexToBone:
            joint = parsed.frames[indexToBone[k]]
            if joint.animations.has_key(animName):
                #joint has this animation
                curAnim = joint.animations[animName]
                #framesLeft = range(len(animationSets[animName]))
                for keys in curAnim.animationKeys[0].timedFloatKeys:
                    for im in xrange(16):
                        animationSets[animName][keys.time - 1][k][im] = keys.floatKeys.values[im]
            else:
                print "warning: joint", joint.name, "has no animation keys for animation", animName
    if needToExport:
        file.write(str(len(animationSets)) + '\n') #num of animations
        file.write(str(len(indexToBone)) + '\n') #num of joints
        speedSetFile.write(str(len(animationSets)) + '\n')
        nameSetFile.write(str(len(animationSets)) + '\n')
    
    realNameToNameStartEnd = {}
    for k in animationSets:
        animName = str(k)
        #check if partial body animation
        startJoint = 0
        endJoint = len(indexToBone) - 1
        if k.startswith('p_'):
            dashPos = k.find('-')
            if dashPos != -1:
                startBoneName = k[2:dashPos]
                startJoint = boneNameToIndex[startBoneName]
                nameEndPos = k.find('__')
                if nameEndPos != -1:
                    endBoneName = k[dashPos+1:nameEndPos]
                    animName = k[nameEndPos+2:]
                    if endBoneName in boneNameToIndex:
                        endJoint = boneNameToIndex[endBoneName]
                    else:
                        #error
                        print 'Partial Body Animation Error: referenced bone', startBoneName, 'is not part of the skeleton.'
                        print 'Press Any Key to continue..'
                        raw_input()
                else:
                    #error
                    print 'Partial Body Animation Error: bone name formatting is wrong. Please end bone name with "__"'
                    print 'Press Any Key to continue..'
                    raw_input()
            else:
                print 'Partial Body Animation Error: bone name formatting is wrong. Please end first bone name with "-"'
                print 'Press Any Key to continue..'
                raw_input()
        realNameToNameStartEnd[animName] = (k, startJoint, endJoint)
    skeys = realNameToNameStartEnd.keys()[:]
    skeys.sort()
    iAnim = 0
    for k in skeys:
        if needToExport: speedSetFile.write('1\n')
        prevName = realNameToNameStartEnd[k][0]
        animName = k
        if needToExport: nameSetFile.write(animName + '\n')
        startJoint = realNameToNameStartEnd[k][1]
        endJoint = realNameToNameStartEnd[k][2]
        if needToExport:
            file.write(animName + '\n') #animation name        
            file.write(str(startJoint) + ' ' + str(endJoint) + '\n')    
            file.write(str(len(animationSets[prevName])) + '\n') # number of frames
            timeIndex = 0 #animation index, should be incresing (for error checking)
            #write for each time
            for timeKey in animationSets[prevName]:
                #file.write('  Time: ' + str(timeIndex) + '\n')
                timeIndex += 1
                jointIndex = 0
                #write matrix for each joint
                for jointKey in timeKey:
                    #file.write('    Joint: ' + str(jointIndex) + '\n')
                    jointIndex += 1
                    for val in jointKey:
                        file.write(str(val) + ' ')
                    file.write('\n')
        iAnim = iAnim + 1
    if needToExport:
        file.close()
        speedSetFile.close()
        nameSetFile.close()
    
    print "Exported animations in order:"
    print skeys
    
    return skeys
def parseContainerForSkeleton(c, parsed, skelsToExport):
    boneNameToIndex = {}
    hierarchyDict = {}
    tdict = {}
    for fname in c.topFrameNames:
        f = c.topFrames[fname]
        if f.isBone:
            t, tdict = exportSkeleton(f, parsed, fname in skelsToExport)
            for k in t:
                boneNameToIndex[k] = t[k]
            if len(tdict) > 0:
                hierarchyDict = tdict
        else:
            t, tdict = parseContainerForSkeleton(f, parsed, skelsToExport)
            for k in t:
                boneNameToIndex[k] = t[k]
            if len(tdict) > 0:
                hierarchyDict = tdict
    
    return boneNameToIndex, hierarchyDict
def exportSkinWeightForAllSkinsInFile(c, boneNameToIndex, parsed):
    res = []
    for fname in c.topFrameNames:
        f = c.topFrames[fname]
        for m in f.meshes:
            if m.isSkin:
                # result of a signel call is list of (bone, weigth) tuples
                # note, at this point multiple skin export from single file is not supported
                res.append( exportSkinWeights(m, boneNameToIndex, parsed) )
    return res
def materialsExport(args):
    """ parses the list of assets that are to be exported and exports them """
    print 'materialsExport', args
    parsed, targetPackage, matsToExport = args['parsed'], args['targetPackage'], args['matsToExport']
    meshesToExport, skeletonsToExport, skinsToExport = args['meshesToExport'], args['skeletonsToExport'], args['skinsToExport']
    boneNameToIndex = args['boneNameToIndex']
    
    dirPath = os.path.dirname(globals()['assetsOutPath'])
    print "PE: EXPORTER: PROGRESS: Checking if target package '%s' exists: " % dirPath
    if not os.path.exists(dirPath):
        print "PE: EXPORTER: PROGRESS: Creating %s folder" % dirPath
        os.mkdir(dirPath)  
    
    for k in matsToExport:
        if k:
            exportMaterial(parsed.materials[k], parsed)
    for meshKey in meshesToExport:
        if meshKey:
            for k in parsed.frames.keys():
                for m in parsed.frames[k].meshes:
                    if not m.isSkin and m.name == meshKey:
                        exportRigidMesh(m, parsed, None) #3rd arg is set of skin weights. if valid, it will be used to segment mesh into n-bone sets
    
    
    for skinKey in skinsToExport:
        if skinKey:
            for k in parsed.frames.keys():
                for m in parsed.frames[k].meshes:
                    if m.isSkin and m.name == skinKey:
                        skinWeights = preprocessSkinWeights(m, boneNameToIndex, parsed)
                        exportRigidMesh(m, parsed, skinWeights) #here skin weights might be modified
                        exportPreprocessedSkinWeights(m, skinWeights, parsed)
                        skinFilename = exportSkin(m, parsed)
                        #res.append(skinFilename)
    
    names = []
    for skelDict in skeletonsToExport:
        if skelDict:
            print skelDict['name']
            names.append(skelDict['name'])
    parseContainerForSkeleton(parsed, parsed, names)
    animNames = exportAnimations(boneNameToIndex, parsed, True);

def materialsExportUI(parsed, targetPackage, mayaCommands):
    res = []
    if mayaCommands.has_key('materialUI'):
        #we have a ui window we can envoke
        #assemble list of materials for the maya UI'
        matsForUI = []
        destinations = []
        meshesToExport = []
        meshDestinations = []
        skinsToExport = []
        skinDestinations = []
        
        skeletonsToExport = []
        skeletonDestinations = []
        for k in parsed.materials.keys():
            matsForUI.append(k)
            destinations.append(getMatOutFullPathFilename(parsed.materials[k], parsed))
        
        for k in parsed.frames.keys():
            for m in parsed.frames[k].meshes:
                if not m.isSkin:
                    meshesToExport.append(m.name)
                    meshDestinations.append(getMeshOutFullPathFilename(m, parsed))
                else:
                    skinsToExport.append(m.name)
                    pathFilename, filename = getSkinOutFullPathFilename(m, parsed)
                    skinDestinations.append(pathFilename)
        #parses the file and returns a map from bone names to an index in a skeleton
        boneNameToIndex, hierarchyDict = parseContainerForSkeleton(parsed, parsed, [])
        if len(hierarchyDict.keys()) > 0:
            skeletonsToExport.append(hierarchyDict)
            skeletonDestinations.append(getSkeletonOutFullPathFilename(None, parsed))
        
        print 'hierarchyDict: ', hierarchyDict
        #raw_input()
        animNames = exportAnimations(boneNameToIndex, parsed, False);
        print 'hierarchyDict: ', hierarchyDict
        #raw_input()
        
        print "calling maya materialUI callback"
        mayaCommands['materialUI'](
            {
                'call': materialsExport, 
                'args':{
                    'parsed' : parsed, 'targetPackage':targetPackage, 
                    'matsToExport':matsForUI, 'matCheckBoxes':[],
                    'destinations':destinations,
                    'meshCheckBoxes':[],
                    'meshesToExport' : meshesToExport,
                    'meshDestinations' : meshDestinations,
                    'skeletonsToExport' : skeletonsToExport,
                    'skeletonCheckBoxes' : [],
                    'skeletonDestinations' : skeletonDestinations,
                    'skinsToExport' : skinsToExport,
                    'skinDestinations' : skinDestinations,
                    'skinCheckBoxes' : [],
                    'boneNameToIndex' : boneNameToIndex,
                    'animNames' : animNames
                }
            }
        )
    else:
        #no UI.. do old fashioned command line communication
        print '-' * 79    
        print 'Materials detected:'
        for k in parsed.materials.keys():
            print k
        print 'Choose from material export options:'
        print '  Export all materials [a]'
        print "  Don't export any materials [n]"
        print '  Choose for each material separately [e]'
        ch = stdin.readline().lower()
        if 'a' in ch:
            for k in parsed.materials.keys():
                exportMaterial(parsed.materials[k], parsed)
        elif 'e' in ch:
            for k in parsed.materials.keys():
                print '    Do you want to export material', k,'? [y/n]'
                ch = stdin.readline().lower()
                if 'y' in ch:
                    exportMaterial(parsed.materials[k], parsed)
                    
        print '-' * 79
        print 'Rigid meshes detected:'
        for k in parsed.frames.keys():
            for m in parsed.frames[k].meshes:
                if not m.isSkin:
                    print m.name
        print 'Choose from mesh export options:'
        print '  Export all rigid meshes [a]'
        print "  Don't export any rigid meshes [n]"
        print '  Choose for each rigid mesh separately [e]'
        ch = stdin.readline().lower()
        if 'a' in ch:
            for k in parsed.frames.keys():
                for m in parsed.frames[k].meshes:
                    if not m.isSkin:
                        meshFilename = exportRigidMesh(m, parsed)
                        res.append(meshFilename)
        elif 'e' in ch:
            for k in parsed.frames.keys():
                for m in parsed.frames[k].meshes:
                    if not m.isSkin:
                        print '    Do you want to export rigid mesh', m.name,'? [y/n]'
                        ch = stdin.readline().lower()
                        if 'y' in ch:
                            meshFilename = exportRigidMesh(m, parsed)
                            res.append(meshFilename)
        #SKELETONS------------------------------------------------------------------
        print 'Warning: skeletons are exported automatically'
        boneNameToIndex = parseContainerForSkeleton(parsed, parsed)
        exportSkinWeightForAllSkinsInFile(parsed, boneNameToIndex, parsed)
        
        #ANIMATIONS-----------------------------------------------------------------
        exportAnimations(boneNameToIndex, parsed)
        
        #SKINS ---------------------------------------------------------------------
        print 'Skins are exported automatically:'
        print 'Skins detected:'
        for k in parsed.frames:
            for m in parsed.frames[k].meshes:
                if m.isSkin:
                    print m.name
        for k in parsed.frames:
            for m in parsed.frames[k].meshes:
                if m.isSkin:
                    exportRigidMesh(m, parsed)
                    skinFilename = exportSkin(m, parsed)
                    res.append(skinFilename)
    return res
def generateGameObjectLoadingCommand(name, metaScriptName, metaScriptPackage,
    m, peuuidVals ):
    res = 'LevelLoader.CreateGameObject("%s",'% name
    for v in m:
        res = res + str(v) + ', '
    if metaScriptName != None:
        res = res + "'%s', '%s'," % (metaScriptName, metaScriptPackage)
        #cp1 = os.environ["PYENGINE_WORKSPACE_DIR"] + '/AssetsIn/XFiles/' + frame.MetaScript.MetaScriptFilename[1:-1]
        #cp2 = path + frame.MetaScript.MetaScriptFilename[1:-1]
        #cp1 = os.path.normpath(cp1)
        #cp2 = os.path.normpath(cp2)
        #print 'copying MetaScript file', cp1, 'to', cp2 
        #os.system('copy "%s" "%s"' % (cp1, cp2))
    else: # no MetaScript
        res = res + 'nil,nil,'
    for val in peuuidVals[:-1]:
        res = res + str(val) + ', '
    res = res + str(peuuidVals[-1]) + ')'
    return res
def parseXFile(xfilename, targetPackage, mayaCommands):
    res = []
    #print 'Enter .x filename:'
    #filename = stdin.readline().lower().strip(' \n')
    filename = xfilename.lower().strip(' \n')
    if not filename.endswith('.x'): filename = filename + '.X'
    globals()['assetsOutPath'] = filename[:filename.rfind("/")] # to xfiles
    globals()['assetsOutPath'] = globals()['assetsOutPath'][:globals()['assetsOutPath'].rfind("/")] # to assets in
    globals()['assetsOutPath'] = globals()['assetsOutPath'][:globals()['assetsOutPath'].rfind("/")] # to workspace
    globals()['assetsOutPath'] = globals()['assetsOutPath'] + '/AssetsOut/' + targetPackage + '/'
    print globals()['assetsOutPath']
    path = os.environ["PYENGINE_WORKSPACE_DIR"]
    path = path + "/AssetsIn/XFiles/"
    print 'opening:', filename, '..'
    #try:
    f = open(filename, 'r')
    #except exception e:
    parsed = xconv.XFile(f, filename)
    f.close()
    if parsed.isSceneNodeGraph:
        pass
    else:
        if parsed.isLevel:
            print 'PE: EXPORTER: PROGRESS: Level is detected'
            dirPath = os.path.dirname(globals()['assetsOutPath'])
            print "PE: EXPORTER: PROGRESS: Checking if target package '%s' exists: " % dirPath
            if not os.path.exists(dirPath):
                print "PE: EXPORTER: PROGRESS: Creating %s folder" % dirPath
                os.mkdir(dirPath)  
            
            path = globals()['assetsOutPath'] + 'Levels/'
            dirPath = path
            print "PE: EXPORTER: PROGRESS: Checking if target package '%s' exists: " % dirPath
            if not os.path.exists(dirPath):
                print "PE: EXPORTER: PROGRESS: Creating %s folder" % dirPath
                os.mkdir(dirPath)  

            outfilename = parsed.strippedFileName.lower() + '_level' + '.levela'
            print '    exporting Level:', path + outfilename
            file = open(path + outfilename, 'w')
            #file.write('LEVEL\n')
            #file.write('%d\n' % len(parsed.frames))
            print 'Level objects:'
            print parsed.frames
            for k in parsed.frames.keys():
                frame = parsed.frames[k]
                print frame.name
                print frame.frameTransformMatrix
                command = ''
                if frame.MetaScript:
                    command = generateGameObjectLoadingCommand(frame.name, frame.MetaScript.MetaScriptFilename[1:-1], targetPackage,
                        frame.frameTransformMatrix.frameMatrix.matrix, frame.peuuid.peuuidNums)
                    
                    cp1 = os.environ["PYENGINE_WORKSPACE_DIR"] + '/AssetsIn/XFiles/' + frame.MetaScript.MetaScriptFilename[1:-1]
                    cp2 = path + frame.MetaScript.MetaScriptFilename[1:-1]
                    cp1 = os.path.normpath(cp1)
                    cp2 = os.path.normpath(cp2)
                    print 'copying MetaScript file', cp1, 'to', cp2 
                    if os.environ["PYENGINE_WORKSPACE_DIR"].startswith("/"):
                    	os.system('cp "%s" "%s"' % (cp1, cp2))
                    else:
                    	os.system('copy "%s" "%s"' % (cp1, cp2))
                else: # no MetaScript
                    command = generateGameObjectLoadingCommand(frame.name, None, None,
                        frame.frameTransformMatrix.frameMatrix.matrix, frame.peuuid.peuuidNums)
                    
                file.write('%s\n\n' % command)
                
            
            file.close()
            res.append(outfilename)
        else:
            #this will fill in information about assets found in file and call the maya UI command given from cvxporter
            #the UI will be used to select the assets to be exported
            res = res + materialsExportUI(parsed, targetPackage, mayaCommands)
            
                
    
    #print 'Do you want to parse another file? [y/n]'
    #ch = stdin.readline().lower()
    #if not 'y' in ch: break
    return res
