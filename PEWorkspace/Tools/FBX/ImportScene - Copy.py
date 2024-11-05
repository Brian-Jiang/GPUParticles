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
#artem@artem-usc-mbpc /cygdrive/c/Projects/vanilla/DL/PEWorkspace/Tools
#$ ./fbx.sh ../AssetsIn/Maya/Vampire/vampire-t-pose.fbx mesh

import sys
import os
import os.path
sys.path.append(os.path.join(os.environ['PYENGINE_WORKSPACE_DIR'], 'External', 'DownloadedLibraries', 'FBX-PythonSDK-2014.1', 'lib', 'Python26_x86'))
sys.path.append(os.path.join(os.environ['PYENGINE_WORKSPACE_DIR'], 'External', 'DownloadedLibraries', 'FBX Python SDK', '2014.1', 'lib', 'Python26'))

#sys.path.append(os.path.join(os.environ['PYENGINE_WORKSPACE_DIR'], 'Tools', 'FBX'))
print sys.path
import traceback

from DisplayGlobalSettings  import *
from DisplayHierarchy       import DisplayHierarchy
from DisplayMarker          import DisplayMarker
from DisplayMesh            import DisplayMesh
from DisplayUserProperties  import DisplayUserProperties
from DisplayPivotsAndLimits import DisplayPivotsAndLimits
from DisplaySkeleton        import DisplaySkeleton
from DisplayNurb            import DisplayNurb
from DisplayPatch           import DisplayPatch
from DisplayCamera          import DisplayCamera
from DisplayLight           import DisplayLight
from DisplayLodGroup        import DisplayLodGroup
from DisplayPose            import DisplayPose
from DisplayAnimation       import DisplayAnimation
from DisplayGenericInfo     import DisplayGenericInfo
from AnalyzeSkeletonHierarchy import AnalyzeSkeletonHierarchy
from StoreSkelToFile        import StoreSkelAndSkelAnimsToFiles
from fbx import FbxPropertyString
from Logger import Logger
def DisplayMetaData(pScene):
    sceneInfo = pScene.GetSceneInfo()
    if sceneInfo:
        print("\n\n--------------------\nMeta-Data\n--------------------\n")
        print("    Title: %s" % sceneInfo.mTitle.Buffer())
        print("    Subject: %s" % sceneInfo.mSubject.Buffer())
        print("    Author: %s" % sceneInfo.mAuthor.Buffer())
        print("    Keywords: %s" % sceneInfo.mKeywords.Buffer())
        print("    Revision: %s" % sceneInfo.mRevision.Buffer())
        print("    Comment: %s" % sceneInfo.mComment.Buffer())

        thumbnail = sceneInfo.GetSceneThumbnail()
        if thumbnail:
            print("    Thumbnail:")

            if thumbnail.GetDataFormat() == FbxThumbnail.eRGB_24 :
                print("        Format: RGB")
            elif thumbnail.GetDataFormat() == FbxThumbnail.eRGBA_32:
                print("        Format: RGBA")

            if thumbnail.GetSize() == FbxThumbnail.eNOT_SET:
                print("        Size: no dimensions specified (%ld bytes)", thumbnail.GetSizeInBytes())
            elif thumbnail.GetSize() == FbxThumbnail.e64x64:
                print("        Size: 64 x 64 pixels (%ld bytes)", thumbnail.GetSizeInBytes())
            elif thumbnail.GetSize() == FbxThumbnail.e128x128:
                print("        Size: 128 x 128 pixels (%ld bytes)", thumbnail.GetSizeInBytes())

def DisplayContent(pScene, skelRoots, skelDirectMaps, meshToBindMatrixMap, peParserContext):
    peParserContext.logger.StartScope("DisplayContent() ", True)
    lNode = pScene.GetRootNode()
    if lNode:
        DisplayNodeContent(pScene, lNode, skelRoots, skelDirectMaps, meshToBindMatrixMap, '', peParserContext)
    peParserContext.logger.EndScope("DisplayContent() done", True)

def DisplayNodeContent(lScene, pNode, skelRoots, skelDirectMaps, meshToBindMatrixMap, parentName, peParserCtx):
    
    UnsupportedAttrTypeMap = {
        FbxNodeAttribute.eCameraStereo : 'FbxNodeAttribute.eCameraStereo',
        FbxNodeAttribute.eCameraSwitcher : 'FbxNodeAttribute.eCameraSwitcher',
        FbxNodeAttribute.eContentLoaded : 'FbxNodeAttribute.eContentLoaded',
        FbxNodeAttribute.eLODGroup : 'FbxNodeAttribute.eLODGroup',
        FbxNodeAttribute.eLine : 'FbxNodeAttribute.eLine',
        FbxNodeAttribute.eNull : 'FbxNodeAttribute.eNull',
        FbxNodeAttribute.eNurbsCurve : 'FbxNodeAttribute.eNurbsCurve',
        FbxNodeAttribute.eNurbsSurface : 'FbxNodeAttribute.eNurbsSurface',
        FbxNodeAttribute.eOpticalMarker : 'FbxNodeAttribute.eOpticalMarker',
        FbxNodeAttribute.eOpticalReference : 'FbxNodeAttribute.eOpticalReference',
        FbxNodeAttribute.eShape : 'FbxNodeAttribute.eShape',
        FbxNodeAttribute.eSubDiv : 'FbxNodeAttribute.eSubDiv',
        FbxNodeAttribute.eTrimNurbsSurface : 'FbxNodeAttribute.eTrimNurbsSurface',
        FbxNodeAttribute.eUnknown : 'FbxNodeAttribute.eUnknown',
    }

    AttrTypeMap = {
        FbxNodeAttribute.eSkeleton : 'FbxNodeAttribute.eSkeleton',
        FbxNodeAttribute.eMarker : 'FbxNodeAttribute.eMarker',
        FbxNodeAttribute.eMesh : 'FbxNodeAttribute.eMesh',
        FbxNodeAttribute.eNurbs : 'FbxNodeAttribute.eNurbs',
        FbxNodeAttribute.ePatch : 'FbxNodeAttribute.ePatch',
        FbxNodeAttribute.eCamera : 'FbxNodeAttribute.eCamera',
        FbxNodeAttribute.eLight : 'FbxNodeAttribute.eLight',
    }
    CombinedMap = {}
    for k in AttrTypeMap.keys():
        CombinedMap[k] = AttrTypeMap[k]
    for k in UnsupportedAttrTypeMap.keys():
        CombinedMap[k] = UnsupportedAttrTypeMap[k]

    fullName = parentName + pNode.GetName()
    peParserCtx.logger.StartScope(fullName)
    if pNode.GetNodeAttribute() == None:
        peParserCtx.logger.AddLine("NULL Node Attribute\n", peParserCtx.logLevel > 0)
        lAttributeType = None
    else:
        lAttributeType = (pNode.GetNodeAttribute().GetAttributeType())

    peParserCtx.logger.AddLine( "  Node Name: %s type: %s" % (pNode.GetName(), CombinedMap.get(lAttributeType, "UNKNOWN") if lAttributeType else "NULL"), peParserContext.logLevel > 0)
    if lAttributeType and not CombinedMap.has_key(lAttributeType):
        peParserCtx.logger.AddLine("URGENT WARNING: unknown type %d" % (int(lAttributeType),))
        print dir(FbxNodeAttribute)
    if UnsupportedAttrTypeMap.has_key(lAttributeType):
        peParserCtx.logger.AddLine("URGENT WARNING: unsupported attribute type %s" % (UnsupportedAttrTypeMap[lAttributeType],))

    if lAttributeType == FbxNodeAttribute.eNull:
        lNull = pNode.GetNodeAttribute()
        print "TODO: this is likely a size multiplier. value: lNull.Size.Get():", lNull.Size.Get()
        print "Look:", lNull.Look.Get()

    if lAttributeType == FbxNodeAttribute.eMarker:
        DisplayMarker(pNode)
    elif lAttributeType == FbxNodeAttribute.eSkeleton:
        DisplaySkeleton(pNode, peParserCtx)
    elif lAttributeType == FbxNodeAttribute.eMesh:
        if peParserCtx.assetType != 'mesh':
            print "Skipping FbxNodeAttribute.eMesh Node since assetType is not mesh"
        else:
            #we are going to call DisplayMesh() later. We first want to go through all content and gather
            #information baout all joints and skeltons, and then call DisplayMesh()
            #This way we can guarantee we have all joint info, bind pose info, etc. When calling DisplayMesh()
            #meshName = DisplayMesh(lScene, assetType, pNode, hierarchy, skelRoots, skelDirectMaps, meshToBindMatrixMap, len(meshNames), peParserContext)
            peParserCtx.meshNames.append(pNode.GetName())
            
    elif lAttributeType == FbxNodeAttribute.eNurbs:
        DisplayNurb(pNode)
    elif lAttributeType == FbxNodeAttribute.ePatch:
        DisplayPatch(pNode)
    elif lAttributeType == FbxNodeAttribute.eCamera:
        DisplayCamera(pNode)
    elif lAttributeType == FbxNodeAttribute.eLight:
        DisplayLight(pNode)

    nodeDict = peParserCtx.directMap[pNode.GetName()]
    DisplayUserProperties(pNode, peParserCtx)
    DisplayTarget(pNode, peParserCtx)
    DisplayPivotsAndLimits(pNode, peParserCtx)
    
    DisplayTransformPropagation(pNode, peParserCtx, nodeDict)
    DisplayGeometricTransform(pNode, peParserCtx, nodeDict)
    
    #display all the info in separate log scope
    peParserCtx.logger.StartScope("Detailed node contents")
    for k in ['name', 'fullName', 'translation', 'rotation', 'rotationMatrix', 'scale',
        'preRotation', 'postRotation', 'postRotationMatrix', 'postRotationMatrixInverse',
        'rotationPivot', 'rotationOffset', 'rotationPivotMatrix',
        'scalingPivot', 'scalingOffsetMatrix','fixedUpScalingOffsetMatrix',
        'rotationOffsetMatrix', 'fixedUpRotationOffsetMatrix',
        'transformInheritance', 'rotOrder', 'rotOrderLimitsOnly', 'finalLocalMatrix', 'finalWorldMatrix', 'scalePivotFixup',
        'xform', 'otherVersion3']:
        peParserCtx.logger.AddLine("'%s' : %s" % (k, Logger.FbxObjectToString(nodeDict[k])))
    peParserCtx.logger.EndScope()
    
    for i in range(pNode.GetChildCount()):
        DisplayNodeContent(lScene, pNode.GetChild(i), skelRoots, skelDirectMaps, meshToBindMatrixMap, fullName + '|', peParserContext)

    peParserCtx.logger.EndScope()
    
def DisplayTarget(pNode, peParserContext):
    if pNode.GetTarget():
        peParserContext.logger.AddLine("    Target Name: %s" % pNode.GetTarget().GetName())

def DisplayTransformPropagation(pNode, peParserCtx, nodeDict):
    peParserCtx.logger.AddLine("    Transformation Propagation", False)
    
    # Rotation Space
    lRotationOrder = pNode.GetRotationOrder(FbxNode.eSourcePivot)

    nodeDict['rotOrder'] = ""
    if lRotationOrder == eEulerXYZ:
        nodeDict['rotOrder'] = "Euler XYZ"
    elif lRotationOrder == eEulerXZY:
        nodeDict['rotOrder'] = "Euler XZY"
    elif lRotationOrder == eEulerYZX:
        nodeDict['rotOrder'] = "Euler YZX"
    elif lRotationOrder == eEulerYXZ:
        nodeDict['rotOrder'] = "Euler YXZ"
    elif lRotationOrder == eEulerZXY:
        nodeDict['rotOrder'] = "Euler ZXY"
    elif lRotationOrder == eEulerZYX:
        nodeDict['rotOrder'] = "Euler ZYX"
    elif lRotationOrder == eSphericXYZ:
        nodeDict['rotOrder'] = "Spheric XYZ"
        
    peParserCtx.logger.AddLine("Rotation Order: %s" % (nodeDict['rotOrder'],), True)
    
    # Use the Rotation space only for the limits
    # (keep using eEULER_XYZ for the rest)
    if pNode.GetUseRotationSpaceForLimitOnly(FbxNode.eSourcePivot):
        nodeDict['rotOrderLimitsOnly'] = True
        peParserCtx.logger.AddLine("        Use the Rotation Space for Limit specification only: Yes", False)
    else:
        nodeDict['rotOrderLimitsOnly'] = False
        peParserCtx.logger.AddLine("        Use the Rotation Space for Limit specification only: No", False)

    # Inherit Type
    lInheritType = pNode.GetTransformationInheritType()

    if lInheritType == FbxTransform.eInheritRrSs:
        nodeDict['transformInheritance'] = "RrSs"
    elif lInheritType == FbxTransform.eInheritRSrs:
        nodeDict['transformInheritance'] = "RSrs"
    elif lInheritType == FbxTransform.eInheritRrs:
        nodeDict['transformInheritance'] = "Rrs"

    peParserContext.logger.AddLine("        Transformation Inheritance: %s" % (nodeDict['transformInheritance'],), False)
def ConstructRotationMatrix(x, y, z, order):
    rotX = FbxMatrix()
    rotX.SetTRS(FbxVector4(0, 0, 0), FbxVector4(x, 0, 0), FbxVector4(1, 1, 1))
    rotY = FbxMatrix()
    rotY.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, y, 0), FbxVector4(1, 1, 1))
    rotZ = FbxMatrix()
    rotZ.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, 0, z), FbxVector4(1, 1, 1))
    
    if 'xyz' in order.lower(): print "using xyz"; return rotZ * rotY * rotX
    if 'xzy' in order.lower(): print "using xzy"; return rotY * rotZ * rotX
    if 'yxz' in order.lower(): print "using yxz"; return rotZ * rotX * rotY
    if 'yzx' in order.lower(): print "using yzx"; return rotX * rotZ * rotY
    if 'zxy' in order.lower(): print "using zxy"; return rotY * rotX * rotZ
    if 'zyx' in order.lower(): print "using zyx"; return rotX * rotY * rotZ
    print "using xyz"; 
    return rotZ * rotY * rotX
    
def DisplayGeometricTransform(pNode, peParserCtx, nodeDict):
    lTmpVector = pNode.GetGeometricTranslation(FbxNode.eSourcePivot)
    nodeDict['pivotTranslation'] = lTmpVector

    lTmpVector = pNode.GetGeometricRotation(FbxNode.eSourcePivot)
    nodeDict['geometricRotation'] = lTmpVector

    lTmpVector = pNode.GetGeometricScaling(FbxNode.eSourcePivot)
    nodeDict['geometricScale'] = lTmpVector

    translation = pNode.LclTranslation.Get()
    nodeDict['translation'] = translation
    translationMatrix = FbxMatrix()
    translationMatrix.SetTRS(FbxVector4(translation[0], translation[1], translation[2]), FbxVector4(0, 0, 0), FbxVector4(1, 1, 1))
    nodeDict['translationMatrix'] = translationMatrix
    
    rotation = pNode.LclRotation.Get()
    nodeDict['rotation'] = rotation
    rotationMatrix = ConstructRotationMatrix(rotation[0], rotation[1], rotation[2], nodeDict['rotOrder'])
    nodeDict['rotationMatrix'] = rotationMatrix
    rotationMatrixNoOrder = ConstructRotationMatrix(rotation[0], rotation[1], rotation[2], 'xyz')
    
    scale = pNode.LclScaling.Get()
    nodeDict['scale'] = scale
    scaleMatrix = FbxMatrix()
    scaleMatrix.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, 0, 0), FbxVector4(scale[0], scale[1], scale[2]))
    nodeDict['scaleMatrix'] = scaleMatrix
    
    preRot = pNode.GetPreRotation(FbxNode.eSourcePivot)
    nodeDict['preRotation'] = preRot
    preRotMatrix = FbxMatrix()
    preRotMatrix.SetTRS(FbxVector4(0, 0, 0), preRot, FbxVector4(1, 1, 1))
    nodeDict['preRotationMatrix'] = preRotMatrix
    
    postRot = pNode.GetPostRotation(FbxNode.eSourcePivot)
    nodeDict['postRotation'] = postRot
    postRotMatrix = FbxMatrix()
    postRotMatrix.SetTRS(FbxVector4(0, 0, 0), postRot, FbxVector4(1, 1, 1))
    nodeDict['postRotationMatrix'] = postRotMatrix
    
    nodeDict['postRotationMatrixInverse'] = postRotMatrix.Inverse() #for some reason, fbx stores maya node's "Rotate Axis" as Inverse of Matrix("Rotate Axis")
    
    #rotation pivot
    rotationPivot = pNode.GetRotationPivot(FbxNode.eSourcePivot)
    nodeDict['rotationPivot'] = rotationPivot
    rotationPivotMatrix = FbxMatrix()
    rotationPivotMatrix.SetTRS(rotationPivot, FbxVector4(0, 0, 0), FbxVector4(1, 1, 1))
    nodeDict['rotationPivotMatrix'] = rotationPivotMatrix
    
    #rotation offset
    rotationOffset = pNode.GetRotationOffset(FbxNode.eSourcePivot)
    nodeDict['rotationOffset'] = rotationOffset
    rotationOffsetMatrix = FbxMatrix()
    rotationOffsetMatrix.SetTRS(rotationOffset, FbxVector4(0, 0, 0), FbxVector4(1, 1, 1))
    nodeDict['rotationOffsetMatrix'] = rotationOffsetMatrix
    
    
    scalingPivot = pNode.GetScalingPivot(FbxNode.eSourcePivot)
    nodeDict['scalingPivot'] = scalingPivot
    scalingPivotPartMatrix = FbxMatrix()
    scalingPivotPartMatrix.SetTRS(scalingPivot, FbxVector4(0, 0, 0), FbxVector4(1, 1, 1))
    
    scalingOffset = pNode.GetScalingOffset(FbxNode.eSourcePivot)
    nodeDict['scalingOffset'] = scalingOffset
    scalingOffsetMatrix = FbxMatrix()
    scalingOffsetMatrix.SetTRS(scalingOffset, FbxVector4(0, 0, 0), FbxVector4(1, 1, 1))
    nodeDict['scalingOffsetMatrix'] = scalingOffsetMatrix
    
    fixedUpScalingOffsetMatrix = rotationMatrix.Inverse() * rotationMatrixNoOrder * scalingOffsetMatrix
    fixedUpScalingOffsetMatrix.SetRow(0, FbxVector4(1.0, 0, 0, 0));
    fixedUpScalingOffsetMatrix.SetRow(1, FbxVector4(0, 1.0, 0, 0));
    fixedUpScalingOffsetMatrix.SetRow(2, FbxVector4(0, 0, 1.0, 0));
    
    fixedUpRotationOffsetMatrix = rotationMatrix.Inverse() * rotationMatrixNoOrder * rotationOffsetMatrix
    fixedUpRotationOffsetMatrix.SetRow(0, FbxVector4(1.0, 0, 0, 0));
    fixedUpRotationOffsetMatrix.SetRow(1, FbxVector4(0, 1.0, 0, 0));
    fixedUpRotationOffsetMatrix.SetRow(2, FbxVector4(0, 0, 1.0, 0));
    nodeDict['fixedUpRotationOffsetMatrix'] = fixedUpRotationOffsetMatrix
    
    
    #fixedUpScaleOffsetMatrix.Set(0,0,1.0); fixedUpScaleOffsetMatrix.Set(0,1,0.0); fixedUpScaleOffsetMatrix.Set(0,2,0.0)
    #rotatedScaleOffset.Set(1,0,0.0); rotatedScaleOffset.Set(1,1,1.0); rotatedScaleOffset.Set(1,2,0.0)
    #rotatedScaleOffset.Set(2,0,0.0); rotatedScaleOffset.Set(2,1,0.0); rotatedScaleOffset.Set(2,2,1.0)
    
    nodeDict['fixedUpScalingOffsetMatrix'] = fixedUpScalingOffsetMatrix
    
    nodeDict['rotationMatrixNoOrder'] = rotationMatrixNoOrder
    
    xform = translationMatrix \
        * rotationOffsetMatrix * rotationPivotMatrix * postRotMatrix * rotationMatrix * preRotMatrix * rotationPivotMatrix.Inverse() \
        * scalingOffsetMatrix * scalingPivotPartMatrix * scaleMatrix * scalingPivotPartMatrix.Inverse()
    
    otherVersion2 = translationMatrix \
        * fixedUpRotationOffsetMatrix * rotationPivotMatrix * postRotMatrix * rotationMatrix * preRotMatrix * rotationPivotMatrix.Inverse() \
        * fixedUpScalingOffsetMatrix * scalingPivotPartMatrix * scaleMatrix * scalingPivotPartMatrix.Inverse()
    
    otherVersion3 = translationMatrix \
        * fixedUpRotationOffsetMatrix * rotationPivotMatrix * postRotMatrix * rotationMatrix * preRotMatrix * rotationPivotMatrix.Inverse() \
        * scalingOffsetMatrix * scalingPivotPartMatrix * scaleMatrix * scalingPivotPartMatrix.Inverse()
    
    nodeDict['scalePivotFixup'] = otherVersion2
    
    nodeDict['finalLocalMatrix'] = otherVersion2
    nodeDict['finalWorldMatrix'] = otherVersion2
    
    nodeDict['xform'] = xform
    nodeDict['otherVersion3'] = otherVersion3
    
    if nodeDict['parent']:
        nodeDict['finalWorldMatrix'] = nodeDict['parent']['finalWorldMatrix'] * otherVersion2

class PEParserContext:
    def __init__(self, logger, assetType, package, flipAxis, logLevel):
        self.logger = logger
        self.logLevel = logLevel
        self.conversionFactor = 1.0
        self.directMap = {} #map node name -> node dictionary
        self.rootDict = {} # top dictionary of the node hierarchy
        self.meshNames = [] #names of meshes in scene
        self.flipAxis = flipAxis #name of axis to flip, 'x' or 'z'
        self.assetType = assetType #kind of asset we want to export 'mesh', 'animation', etc.
        self.package = package
if __name__ == "__main__":
    try:
        from FbxCommon import *
    except ImportError:
        import platform
        msg = 'You need to copy the content in compatible subfolder under /lib/python<version> into your python install folder such as '
        if platform.system() == 'Windows' or platform.system() == 'Microsoft':
            msg += '"Python26/Lib/site-packages"'
        elif platform.system() == 'Linux':
            msg += '"/usr/local/lib/python2.6/site-packages"'
        elif platform.system() == 'Darwin':
            msg += '"/Library/Frameworks/Python.framework/Versions/2.6/lib/python2.6/site-packages"'        
        msg += ' folder.'
        print(msg) 
        sys.exit(1)

    # Prepare the FBX SDK.
    lSdkManager, lScene = InitializeSdkObjects()
    # Load the scene.
    
    logger = Logger(sys.argv[0], "../ParserLogs", "../External/DownloadedLibraries/ListCollapse/listCollapse-pe.js")
    
    
    assetType = None
    try:
        # The example can take a FBX file as an argument.
        
        if len(sys.argv) > 2:
            assetType = sys.argv[2]
            if not assetType in ['skeleton', 'animation', 'mesh']:
                print "asset type must be skeleton, animation, or mesh"
                sys.exit(0)
            print("\n\nFile: %s\n" % sys.argv[1])
            print("\n\nAsset Type: %s\n" % sys.argv[2])
            lResult = LoadScene(lSdkManager, lScene, sys.argv[1])
        else :
            lResult = False

            print("\n\nUsage: ImportScene <FBX file name> <asset type>\n")

        if not lResult:
            print("\n\nAn error occurred while loading the scene...")
        else :
            package = 'Default'
            logLevel = 0
            animsToExport = []
            flipAxis = 'x'
            logger.StartScope("Parsing Arguments")
    
            if len(sys.argv) > 3:
                iarg = 3
                while iarg < len(sys.argv):
                    if sys.argv[iarg] == '-p' and iarg + 1 < len(sys.argv):
                        package = sys.argv[iarg+1]
                        dirPath = os.path.join(os.environ['PYENGINE_WORKSPACE_DIR'], "AssetsOut", package)
                        if not os.path.exists(dirPath):
                            print "Creating %s folder" % dirPath
                            os.mkdir(dirPath)
    
                        iarg += 2
                    elif sys.argv[iarg] == '-a' and iarg + 1 < len(sys.argv):
                        env = {}
                        print "Evaluating animation profile %s" % (sys.argv[iarg+1],)
                        execfile(sys.argv[iarg+1], env)
    
                        if env.has_key('d') and env['d'].has_key('sets'):
                            animsToExport = env['d']['sets']
                        print "Anims to export: %s" % (str(animsToExport),)
                        logger.AddLine("Anims to export: %s" % (str(animsToExport),))
                        iarg += 2
                    elif sys.argv[iarg] == '-l' and iarg + 1 < len(sys.argv):
                        logLevel = int(sys.argv[iarg+1])
                        iarg += 2
                    elif sys.argv[iarg] == '-flip' and iarg + 1 < len(sys.argv):
                        newAxis = sys.argv[iarg+1]
                        if not newAxis in ['x', 'z']:
                            print "\nOnly x and z axis are supported for flipping"
                            sys.exit(0)
                        else:
                            flipAxis = newAxis
                        
                        iarg += 2
            logger.AddLine("Flip Axis:" + flipAxis, True)
            logger.AddLine("Asset Type:" + assetType, True)
            logger.AddLine("Package:" + package, True)
            logger.EndScope()
            
            peParserContext = PEParserContext(logger, assetType, package, flipAxis, logLevel)
            DisplayMetaData(lScene)
            
            print("\n\n---------------------\nGlobal Light Settings\n---------------------\n")
            DisplayGlobalLightSettings(lScene)

            print("\n\n----------------------\nGlobal Camera Settings\n----------------------\n")
            DisplayGlobalCameraSettings(lScene)

            print("\n\n--------------------\nGlobal Time Settings\n--------------------\n")
            DisplayGlobalTimeSettings(lScene.GetGlobalSettings(), peParserContext)

            DisplayHierarchy(lScene, peParserContext)
            
            hierarchy, directMap = peParserContext.rootDict, peParserContext.directMap
            
            #print ("--------\nStep 2: Gathering Mesh Names")
            #meshNames = GatherMeshList(lScene)
            #print ("  GatherMeshList(): meshes found: %s\n---------" % str(meshNames)) 
    
    
            print("---------\nStep 3: Node Content (But will defer DisplayMesh()) \n------------")
            DisplayContent(lScene, None, None, None, peParserContext)
            
            peParserContext.logger.AddLine("meshNames after DisplayContent(): %s" % (str(peParserContext.meshNames)),)
            
            print("---------\nStep 4: Analyse joint nodes + hierarchy => skeletons")
            skelRoots, skelDirectMaps = AnalyzeSkeletonHierarchy(hierarchy, directMap, logger, logLevel)
            print("------------")
            
            print("---------\nStep 5: Analysing Poses (bind poses for skeletons)")
            meshToBindMatrixMap = DisplayPose(lScene, skelRoots, skelDirectMaps, peParserContext.meshNames, logLevel)
            print("---------")
            
            print("------------")
            print(            "Step 6: Call DisplayMesh() on gathered mesh names")
            
            for im in xrange(len(peParserContext.meshNames)):
                name = peParserContext.meshNames[im]
                DisplayMesh(lScene, assetType, peParserContext.directMap[name]['fbxNode'], hierarchy, skelRoots, skelDirectMaps, meshToBindMatrixMap, package, im, flipAxis, peParserContext)
            
            raw_input()
            if assetType != 'animation':
                print "Skipping DisplayAnimation() since asset type is not animation"
            else:
                print("\n\n---------\nStep 7: Export Animation\n---------\n")
                DisplayAnimation(lScene, skelRoots, skelDirectMaps, animsToExport, logLevel)

            StoreSkelAndSkelAnimsToFiles(lScene, assetType, skelRoots, package, flipAxis, logLevel)
            
            #now display generic information
            print("\n\n---------\nGeneric Information\n---------\n")
            print "Skipping DisplayGenericInfo.."
            #DisplayGenericInfo(lScene)
            
            logger.ProduceHtml()
            
            raw_input()
    except:
        print "Error hapenned during fbx parsing. Callstack:"
        print traceback.print_exc()
        raw_input()
    finally:
        # Destroy all objects created by the FBX SDK.
        lSdkManager.Destroy()
        sys.exit(0)
