
import os.path
from DisplayCommon import *
from fbx import FbxLayerElement
from fbx import FbxSurfaceMaterial
from fbx import FbxLayeredTexture
from fbx import FbxTexture, FbxVector4, FbxMatrix, FbxQuaternion
from DisplayMaterial import DisplayMaterial
from DisplayTexture  import DisplayTexture, DisplayTextureInfo  
from DisplayLink     import DisplayLink
from DisplayShape    import DisplayShape
from fbx import FbxPropertyString
import math

def WriteJointName(joint, asf):
    asf.write("%s\n" % joint['name'])
    for child in joint['children']:
        WriteJointName(child, asf)

def ConstructRotationMatrix(x, y, z, order):
    rotX = FbxMatrix()
    rotX.SetTRS(FbxVector4(0, 0, 0), FbxVector4(x, 0, 0), FbxVector4(1, 1, 1))
    rotY = FbxMatrix()
    rotY.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, y, 0), FbxVector4(1, 1, 1))
    rotZ = FbxMatrix()
    rotZ.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, 0, z), FbxVector4(1, 1, 1))
    
    if 'xyz' in order.lower(): return rotZ * rotY * rotX
    if 'xzy' in order.lower(): return rotY * rotZ * rotX
    if 'yxz' in order.lower(): return rotZ * rotX * rotY
    if 'yzx' in order.lower(): return rotX * rotZ * rotY
    if 'zxy' in order.lower(): return rotY * rotX * rotZ
    if 'zyx' in order.lower(): return rotX * rotY * rotZ
    return rotZ * rotY * rotX
    
def StoreJointToFile(joint, parentMatrix, sfile, flipAxis):
    
    sfile.write("%s\n%d\n" % (joint['name'], joint['index']))
    if joint.has_key('matrix'):
        m = joint['matrix']
        
        fbxM = FbxMatrix()
            
        if flipAxis == 'x':
            #flip x
            #m[0], -m[1], -m[2], m[3],
            #-m[4], m[5], m[6], m[7],
            #-m[8], m[9], m[10], m[11],
            #-m[12], m[13], m[14], m[15]
            
            fbxM.Set(0, 0,  m[0]);  fbxM.Set(0, 1, -m[1]);  fbxM.Set(0, 2, -m[2]);  fbxM.Set(0, 3,  m[3])
            fbxM.Set(1, 0, -m[4]);  fbxM.Set(1, 1,  m[5]);  fbxM.Set(1, 2,  m[6]);  fbxM.Set(1, 3,  m[7])
            fbxM.Set(2, 0, -m[8]);  fbxM.Set(2, 1,  m[9]);  fbxM.Set(2, 2,  m[10]); fbxM.Set(2, 3,  m[11])
            fbxM.Set(3, 0, -m[12]); fbxM.Set(3, 1,  m[13]); fbxM.Set(3, 2,  m[14]); fbxM.Set(3, 3,  m[15])
            
            
        else:
            #we flip z on all components of matrix..
            #m[0], m[1], -m[2], m[3],
            #m[4], m[5], -m[6], m[7],
            #-m[8], -m[9], m[10], m[11],
            #m[12], m[13], -m[14], m[15]
            
            fbxM.Set(0, 0,  m[0]);  fbxM.Set(0, 1,  m[1]);  fbxM.Set(0, 2, -m[2]);  fbxM.Set(0, 3,  m[3])
            fbxM.Set(1, 0,  m[4]);  fbxM.Set(1, 1,  m[5]);  fbxM.Set(1, 2, -m[6]);  fbxM.Set(1, 3,  m[7])
            fbxM.Set(2, 0, -m[8]);  fbxM.Set(2, 1, -m[9]);  fbxM.Set(2, 2,  m[10]); fbxM.Set(2, 3,  m[11])
            fbxM.Set(3, 0,  m[12]); fbxM.Set(3, 1,  m[13]); fbxM.Set(3, 2, -m[14]); fbxM.Set(3, 3,  m[15])
            
    else:
        print " WARNING: dont have 'matrix' field for joint %s meaning it was not part of bind pose. Will set it to default values of joint" % joint['name']
        defaultT = joint['LclTranslation']
        defaultR = joint['LclRotation']
        defaultS = joint['LclScaling']
    
        preRotMatrix = joint['preRotationMatrix']
        postRotMatrixInverse = joint['postRotationMatrixInverse']
    
        rotM = ConstructRotationMatrix(defaultR[0], defaultR[1], defaultR[2], joint['rotOrder'])
        rot = FbxVector4(defaultR[0], defaultR[1], defaultR[2])
        tM = FbxMatrix()
        tM.SetTRS(FbxVector4(defaultT[0], defaultT[1], defaultT[2]), FbxVector4(0, 0, 0), FbxVector4(1.0, 1.0, 1.0))
    
        sM = FbxMatrix()
        sM.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, 0, 0), FbxVector4(defaultS[0], defaultS[1], defaultS[2]))
    
        rM = tM * preRotMatrix * rotM * postRotMatrixInverse * sM
    
        if flipAxis == 'x':
            #reverse x axis and change x values
            rM.Set(0, 1, -rM.Get(0, 1))
            rM.Set(0, 2, -rM.Get(0, 2))
            rM.Set(1, 0, -rM.Get(1, 0))
            rM.Set(2, 0, -rM.Get(2, 0))
        else:
            #reverse z axis and change z values
            rM.Set(2, 0, -rM.Get(2, 0))
            rM.Set(2, 1, -rM.Get(2, 1))
            rM.Set(0, 2, -rM.Get(0, 2))
            rM.Set(1, 2, -rM.Get(1, 2))
        fbxM = parentMatrix * rM
    sfile.write("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f" % (
        fbxM.Get(0, 0), fbxM.Get(0, 1), fbxM.Get(0, 2), fbxM.Get(0, 3),
        fbxM.Get(1, 0), fbxM.Get(1, 1), fbxM.Get(1, 2), fbxM.Get(1, 3),
        fbxM.Get(2, 0), fbxM.Get(2, 1), fbxM.Get(2, 2), fbxM.Get(2, 3),
        fbxM.Get(3, 0), fbxM.Get(3, 1), fbxM.Get(3, 2), fbxM.Get(3, 3)
    ))
        
    sfile.write("\n")
    sfile.write("%d\n" % (len(joint['children']),))
    for child in joint['children']:
        StoreJointToFile(child, fbxM, sfile, flipAxis)
def StoreSkelAndSkelAnimsToFiles(lScene, assetType, skelRoots, package, flipAxis, logLevel):
    print "  StoreSkelToFile Entry.."
    #print skelRoots
    fbxPath = str(FbxPropertyString(lScene.GetSceneInfo().Original_FileName).Get())
    fbxFileName = os.path.splitext(os.path.basename(fbxPath))[0]
    
    assetsOut = os.path.join(os.environ['PYENGINE_WORKSPACE_DIR'], "AssetsOut", package)
    
    for skelRoot in skelRoots:
        if assetType != "skeleton":
            print "Skipping storing skeleton since asset type is not skeleton"
        else:
            skeletonFileName = '%s_%s.skela' % (fbxFileName, skelRoot['name'])
            skeletonFileName = skeletonFileName.replace(":", "__")
            print "Saving Skeletons : ", skeletonFileName, "to", assetsOut
            
            dirPath = os.path.join(assetsOut, "Skeletons")
            if not os.path.exists(dirPath):
                print "Creating %s folder" % dirPath
                os.mkdir(dirPath)
            
            sfile = open(os.path.join(dirPath, skeletonFileName), 'w')
            print "  Writing ", os.path.join(dirPath, skeletonFileName)
            StoreSkelToExistingFile(sfile, skelRoot, lScene, package, flipAxis, logLevel)
            sfile.close()
        if assetType != 'animation':
            print "Skipping storing animation because asset type is not animation"
        else:
            asFileName = '%s_%s.animseta' % (fbxFileName, skelRoot['name'])
            asFileName = asFileName.replace(":", "__")
            
            dirPath = os.path.join(assetsOut, "AnimationSets")
            if not os.path.exists(dirPath):
                print "Creating %s folder" % dirPath
                os.mkdir(dirPath)
            
            asf = open(os.path.join(dirPath, asFileName), 'w')
            print "  Writing ", os.path.join(dirPath, asFileName)
            asf.write("ANIMATION_SET_V3_TV3_RQUAT_SV3\n")
            numAnims = len(skelRoot['anims'])
            asf.write("%d\n" % (numAnims)) #nun anims
            #asf.write("%d\n" % skelRoot['numJointsInHierarchy'])
            
            for animName in skelRoot['anims'].keys():
                asf.write("%s\n" % animName)
                asf.write("%d\n" % (skelRoot['numJointsInHierarchy'],)) # write number of joints used in anim, right now support only full skeleton
                #write joint names, right nwo we only export full body anims
                WriteJointName(skelRoot, asf)
                numAnimFrames = len(skelRoot['anims'][animName])
                asf.write("%d\n" % numAnimFrames) #number of frames
                for animFrame in range(0, numAnimFrames):
                    StoreJointFrameToFile(asf, skelRoot, animName, animFrame, flipAxis, logLevel)
            asf.close()
            
def StoreSkelToExistingFile(sfile, skelRoot, lScene, package, flipAxis, logLevel):
    sfile.write("SKELETON_WORLD\n")
    sfile.write("%d\n" % skelRoot['numJointsInHierarchy'])
    StoreJointToFile(skelRoot, FbxMatrix(), sfile, flipAxis)
    
            

def StoreJointFrameToFile(f, joint, animName, animFrame, flipAxis, logLevel):
    
    keys = joint['anims'][animName][animFrame]
    
    #retrieve default values of the TRS attributes
    defaultT = joint['LclTranslation']
    defaultR = joint['LclRotation']
    defaultS = joint['LclScaling']

    preRotMatrix = joint['preRotationMatrix']
    postRotMatrixInverse = joint['postRotationMatrixInverse']
    
    #store translation, rotation, scale
    
    ###################################
    # Translation
    ###################################
    missingChannels = []
    
    tx = keys['tx']
    if tx is None:
        missingChannels.append('tx')
        tx = defaultT[0]
    
    ty = keys['ty']
    if ty is None:
        missingChannels.append('ty')
        ty = defaultT[1]
    
    tz = keys['tz']
    if tz is None:
        missingChannels.append('tz')
        tz = defaultT[2]
    
    if flipAxis == 'x':
        f.write("%f %f %f" % (-tx, ty, tz)) #we flip the x axis
    else:
        f.write("%f %f %f" % (tx, ty, -tz)) #we flip the z axis
    
    if logLevel > 0 and len(missingChannels): print "WARNING: Joint %s does not have translation channels %s for anim frame %d in animation %s" % (joint['name'], str(missingChannels), animFrame, animName)
    
    
    ###################################
    # Rotation
    ###################################
    
    missingChannels = []
    
    rx = keys['rx']
    if rx is None:
        missingChannels.append('rx')
        rx = defaultR[0]
    
    ry = keys['ry']
    if ry is None:
        missingChannels.append('ry')
        ry = defaultR[1]
    
    rz = keys['rz']
    if rz is None:
        missingChannels.append('rz')
        rz = defaultR[2]
    
    rM = ConstructRotationMatrix(rx, ry, rz, joint['rotOrder'])
        
    #lets check if we get same quat with ComposeSphericalXYZ(KFbxVector4pEuler)
    #and test below proved it doesn't match what we get with creating a matrix, so we use matrix
    # since I did not see any proof that ComposeSphericalXYZ actually creates a quternion that we expect
    '''
    animRotQTest = FbxQuaternion()
    animRotQTest.ComposeSphericalXYZ(rot)
    
    animRotQ = FbxQuaternion()
    rM.GetElements(FbxVector4(), animRotQ, FbxVector4(), FbxVector4())
    
    print "Comparing Quaternions from euler -> matrix -> quat and euler -> quat"
    print("%f = %f %f = %f %f = %f %f = %f" % (animRotQ.GetAt(3), animRotQTest.GetAt(3), animRotQ.GetAt(0), animRotQTest.GetAt(0), animRotQ.GetAt(1), animRotQTest.GetAt(1), animRotQ.GetAt(2), animRotQTest.GetAt(2)))
    
    for i in xrange(4):
        if math.fabs(animRotQ.GetAt(i) - animRotQTest.GetAt(i)) > 0.001:
            print "Different!"
            raw_input()
            break
    '''
    
    rM = preRotMatrix * rM * postRotMatrixInverse #apparently post rotation is stored as an inverse of rotate axis field
    
    
    
    if flipAxis == 'x':
        #reverse x axis and change x values
        rM.Set(0, 1, -rM.Get(0, 1))
        rM.Set(0, 2, -rM.Get(0, 2))
        rM.Set(1, 0, -rM.Get(1, 0))
        rM.Set(2, 0, -rM.Get(2, 0))
    else:
        #reverse z axis and change z values
        rM.Set(2, 0, -rM.Get(2, 0))
        rM.Set(2, 1, -rM.Get(2, 1))
        rM.Set(0, 2, -rM.Get(0, 2))
        rM.Set(1, 2, -rM.Get(1, 2))
    
    
    resT, q, resShearing, resS = FbxVector4(), FbxQuaternion(), FbxVector4(), FbxVector4()
    rM.GetElements(resT, q, resShearing, resS)

    f.write(" %f %f %f %f" % (q.GetAt(3), q.GetAt(0), q.GetAt(1), q.GetAt(2)))
    #print ("Rot quat: %f %f %f %f" % (q.GetAt(3), q.GetAt(0), q.GetAt(1), q.GetAt(2)))
    
    if logLevel > 0 and len(missingChannels): print "WARNING: Joint %s does not have rotation channels %s for anim frame %d in animation %s" % (joint['name'], str(missingChannels), animFrame, animName)
    
    
    ###################################
    # Scale
    ###################################
    
    missingChannels = []
    
    sx = keys['sx']
    if sx is None:
        missingChannels.append('sx')
        sx = defaultS[0]
    
    sy = keys['sy']
    if sy is None:
        missingChannels.append('sy')
        sy = defaultS[1]
    
    sz = keys['sz']
    if sz is None:
        missingChannels.append('sz')
        sz = defaultS[2]
    
    f.write(" %f %f %f\n" % (sx, sy, sz))
    
    if logLevel > 0 and len(missingChannels): print "WARNING: Joint %s does not have scale channels %s for anim frame %d in animation %s" % (joint['name'], str(missingChannels), animFrame, animName)
    
    
    for c in joint['children']:
        StoreJointFrameToFile(f, c, animName, animFrame, flipAxis, logLevel)
    
