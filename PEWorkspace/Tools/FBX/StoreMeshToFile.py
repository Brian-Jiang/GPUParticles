
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
from SplitMaterialIntoBoneSegments import SplitMaterialIntoBoneSegments

def StoreMeshToFiles(lScene, meshName, skelRoot, finalVertices, trisSortedByMaterial, materialJointSegments, materials, package, flipAxis):
    
    assetsOut = os.path.join(os.environ['PYENGINE_WORKSPACE_DIR'], "AssetsOut", package)
    
    print "Saving Mesh: ", meshName, "to", assetsOut
    
    #store position buffer
    positionBufferFileName = '%s_pos.bufa' % (meshName)
    
    dirPath = os.path.join(assetsOut, "PositionBuffers")
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath)

    pbf = open(os.path.join(dirPath, positionBufferFileName), 'w')
    pbf.write("POSITION_BUFFER_V1\n%d\n"%(len(finalVertices),))
    for v in finalVertices:
        if flipAxis == 'x':
            pbf.write("%f %f %f\n"%(-v.position[0], v.position[1], v.position[2])) # we flip x axis
        else:
            pbf.write("%f %f %f\n"%(v.position[0], v.position[1], -v.position[2])) # we flip z axis
        
    pbf.close()
    
    #store normal buffer
    normalBufferFileName = '%s_norm.bufa' % (meshName)
    
    dirPath = os.path.join(assetsOut, "NormalBuffers")
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath)

    nbf = open(os.path.join(dirPath, normalBufferFileName), 'w')
    nbf.write("NORMAL_BUFFER\n%d\n"%(len(finalVertices),))
    for v in finalVertices:
        if flipAxis == 'x':
            nbf.write("%f %f %f\n"%(-v.normal[0], v.normal[1], v.normal[2])) # we flip x axis
        else:
            nbf.write("%f %f %f\n"%(v.normal[0], v.normal[1], -v.normal[2])) # we flip z axis
        
    nbf.close()
    
    haveTangents = len(finalVertices) and finalVertices[0].tangent != None
    
    if haveTangents:
        tangentBufferFileName = '%s_tang.bufa' % (meshName)
    
        dirPath = os.path.join(assetsOut, "TangentBuffers")
        if not os.path.exists(dirPath):
            print "Creating %s folder" % dirPath
            os.mkdir(dirPath)

        tbf = open(os.path.join(dirPath, tangentBufferFileName), 'w')
        tbf.write("TANGENT_BUFFER\n%d\n"%(len(finalVertices),))
        for v in finalVertices:
            if flipAxis == 'x':
                tbf.write("%f %f %f\n"%(-v.tangent[0], v.tangent[1], v.tangent[2])) # we flip x axis
            else:
                tbf.write("%f %f %f\n"%(v.tangent[0], v.tangent[1], -v.tangent[2])) # we flip z axis
            
        tbf.close()
    else:
        tangentBufferFileName = 'none'
    
    
    #store texcoord buffer
    texCoordBufferFileNames = []
    for iTCB in range(1):
        n = '%s_tex_coord_%d.bufa'%(meshName, iTCB)
        texCoordBufferFileNames.append(n)
        
        dirPath = os.path.join(assetsOut, "TexCoordBuffers")
        if not os.path.exists(dirPath):
            print "Creating %s folder" % dirPath
            os.mkdir(dirPath)
        
        tcbf = open(os.path.join(dirPath, n), 'w')
        tcbf.write("TEX_COORD_BUFFER\n%d\n"%(len(finalVertices),))
        for v in finalVertices:
            if v.texCoords[iTCB] != None:
                tcbf.write("%f %f\n"%(v.texCoords[iTCB][0], 1.0 - v.texCoords[iTCB][1])) #flip v(y) component of tex coord
            else:
                tcbf.write("0.0 0.0\n") #dont allow no texture buffer.
        tcbf.close()
    
    
    #store index buffer
    totalTris = 0
    for iMat in range(len(materials)):
        totalTris += len(trisSortedByMaterial[iMat])/3
    indexBufferFileName = '%s_index.bufa' % (meshName)
    
    dirPath = os.path.join(assetsOut, "IndexBuffers")
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath)
    
    ibf = open(os.path.join(dirPath, indexBufferFileName), 'w')
    ibf.write("INDEX_BUFFER\n3\n%d\n%d\n"%(totalTris, len(materials)))
    for iMat in range(len(materials)): 
        jointSegments = materialJointSegments[iMat]
        numJointSegments = len(jointSegments) #for meshes it is 1 joint segment with no joints
        ibf.write("%d\n"%(numJointSegments,))
        
        for iJointSegment in range(0,numJointSegments):
            jointSegment = jointSegments[iJointSegment]
            numJointsInSegment = len(jointSegment['joints']) #0 for non skinned meshes
            
            ibf.write("%d\n"%(numJointsInSegment,))
            if numJointsInSegment > 0:
                for localJointIndex in jointSegment['joints']:
                    ibf.write("%d " % localJointIndex)
                ibf.write("\n")
            ibf.write("%d\n"%(len(jointSegment['tris']),))
            for tri in jointSegment['tris']:
                triIndices = tri['indices']
                ibf.write("%d %d %d\n"%(triIndices[2], triIndices[1], triIndices[0])) # we change the winding order since we flip one axis
    ibf.close()
    
    #store each material
    matFileNames = []
    for iMat in range(len(materials)):
        matDict = materials[iMat]
        #print "Writing Material To File:", matDict
        matName = '%s_%s.lua' % (meshName, matDict['name'])
        matName = matName.replace(":", "__")
        matFileNames.append(matName)
    
        dirPath = os.path.join(assetsOut, "Materials")
        if not os.path.exists(dirPath):
            print "Creating %s folder" % dirPath
            os.mkdir(dirPath)
        
        mf = open(os.path.join(dirPath, matName), 'w')
    
        mf.write("function fillMaterialTable(args) -- the script format requires existence of this function\n")
        
        mf.write("args['version']=2\n")
        d = matDict.get('diffuse', [1,1,1])
        o = 1 - matDict.get('opacity', 0)
        
        mf.write("args['diffuse']={%f, %f, %f, %f}\n" % (d[0], d[1], d[2], o))
        
        s = matDict.get('shininess', 0)
        mf.write("args['shininess']=%f\n" % (s))
        
        r = matDict.get('reflectivity', 0)
        mf.write("args['reflectivity']={%f, %f, %f}\n" % (r[0], r[1], r[2]))
        
        s = matDict.get('specular', [0,0,0])
        mf.write("args['specular']={%f, %f, %f}\n" % (s[0], s[1], s[2]))
        
        e = matDict.get('emissive', [0,0,0])
        mf.write("args['emissive']={%f, %f, %f}\n" % (e[0], e[1], e[2]))
        
        numDiffuseTextures = len(matDict['diffuse_textures'])
        numNormalTextures = len(matDict['normal_textures'])
        numSpecularTextures = len(matDict['specular_textures'])
        numEmissiveTextures = len(matDict['emissive_textures'])
        numTotal = numDiffuseTextures + numNormalTextures + numSpecularTextures + numEmissiveTextures
        mf.write("args['textures']={\n")
        for dt in matDict['diffuse_textures']:
            textureFileNameExt = os.path.splitext(os.path.split(dt['filename'])[1])[0]+'.dds'
            mf.write("  {'COLOR', '%s'},\n" % textureFileNameExt)
        
        for dt in matDict['normal_textures']:
            textureFileNameExt = os.path.split(dt['filename'])[1]
            mf.write("  {'NORMAL', '%s'},\n" % textureFileNameExt)
        
        for dt in matDict['specular_textures']:
            textureFileNameExt = os.path.split(dt['filename'])[1]
            mf.write("  {'SPECULAR', '%s'},\n" % textureFileNameExt)
            
        for dt in matDict['emissive_textures']:
            textureFileNameExt = os.path.split(dt['filename'])[1]
            mf.write("  {'GLOW', '%s'},\n" % textureFileNameExt)
        
        mf.write("} -- end textures\n")
        
        mf.write("--notes overrides\n")
        mf.write(matDict['notes'])
        mf.write("\n--notes end\n")
        
        mf.write("\nend\n")
        
        mf.close()
        
    #store material set
    msFileName = '%s.mseta' % meshName
    
    dirPath = os.path.join(assetsOut, "MaterialSets")
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath)
    
    mf = open(os.path.join(dirPath, msFileName), 'w')
    
    mf.write("MATERIAL_SET\n")
    mf.write("%d\n" % len(materials))
    
    for iMat in range(len(materials)):
        mf.write("%s\n"%matFileNames[iMat])
    mf.close()
    
    if skelRoot:
        swFileName = '%s.swghta' % meshName
        
        dirPath = os.path.join(assetsOut, "SkinWeights")
        if not os.path.exists(dirPath):
            print "Creating %s folder" % dirPath
            os.mkdir(dirPath)
        
        swf = open(os.path.join(dirPath, swFileName), 'w')
        swf.write("SKIN_WEIGHTS\n")
        swf.write("%d\n" % len(finalVertices))
        for v in finalVertices:
            swf.write("%d\n" % len(v.influences))
            for i in v.influences:
                swf.write("%d %f %d\n"%(i[1], i[2], i[3]))
        swf.close()
    else:
        swFileName = "none"
        
    #write mesh
    meshFileName = '%s.mesha'%meshName
    
    dirPath = os.path.join(assetsOut, "Meshes")
    if not os.path.exists(dirPath):
        print "Creating %s folder" % dirPath
        os.mkdir(dirPath)
    
    mf = open(os.path.join(dirPath, meshFileName), 'w')
    
    mf.write("MESH\n")
    mf.write("%s\n" % positionBufferFileName)
    mf.write("%s\n" % indexBufferFileName)
    mf.write("%s\n" % texCoordBufferFileNames[0])
    mf.write("%s\n" % normalBufferFileName)
    mf.write("%s\n" % tangentBufferFileName)
    mf.write("%s\n" % msFileName)
    mf.write("%s\n" % swFileName)
    mf.close()
    