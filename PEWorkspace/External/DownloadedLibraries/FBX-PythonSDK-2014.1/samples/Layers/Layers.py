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

import sys

SAMPLE_FILENAME = "Layers.Fbx"
BACKGROUND_IMAGE_NAME = "Spotty"
BACKGROUND_IMAGE = "spotty.jpg"
LAYER1_IMAGE_NAME = "One"
LAYER1_IMAGE = "1.jpg"
LAYER2_IMAGE_NAME = "Waffle"
LAYER2_IMAGE = "waffle.jpg"

def CreateScene(pSdkManager, pScene):
    # Create the scene.
    lCube = CreateCube(lSdkManager, "Cube")

    # Build the node tree.
    lRootNode = lScene.GetRootNode()
    lRootNode.AddChild(lCube)
   
# Create a cube with materials and layered textures.
def CreateCube(pSdkManager, pName):
    
    # create the main structure.
    lMesh = FbxMesh.Create(pSdkManager, "")

    # create the node containing the mesh
    lNode = FbxNode.Create(pSdkManager, pName)
    lNode.SetNodeAttribute(lMesh)
    lNode.SetShadingMode(FbxNode.eTextureShading)
    
    # Create control points.
    lControlPoints = [
        FbxVector4(-50.0,  0.0,  50.0, 1.0), FbxVector4(50.0,  0.0,  50.0, 1.0),
        FbxVector4(50.0, 100.0, 50.0, 1.0), FbxVector4(-50.0, 100.0, 50.0, 1.0),
        FbxVector4(-50.0, 0.0, -50.0, 1.0), FbxVector4(50.0, 0.0, -50.0, 1.0),
        FbxVector4(50.0, 100.0, -50.0, 1.0), FbxVector4(-50.0, 100.0, -50.0, 1.0)
    ]
    lMesh.InitControlPoints(8)
    for index in range(8):
        lMesh.SetControlPointAt(lControlPoints[index], index)
    
    # NOTE: Layers must be filled sequentially. That is, elements in layer
    # 0 have to exist before using layer 1, 2, etc... In this example,
    # the reader will notice that normals, polygroups, color vertices,
    # materials and the first texture are all in layer 0. Then one more texture,
    # with its corresponding UVs, is added in layer 1 and another in layer 2.

    layer = lMesh.GetLayer(0)
    if not layer:
        lMesh.CreateLayer()
        layer = lMesh.GetLayer(0)
    
    # Create the materials.
    # Each polygon face will be assigned a unique material.
    matLayer = FbxLayerElementMaterial.Create(lMesh, "")
    matLayer.SetMappingMode(FbxLayerElement.eByPolygon)
    matLayer.SetReferenceMode(FbxLayerElement.eIndexToDirect)
    layer.SetMaterials(matLayer)
    
    # Create polygons. Assign material indices.
    # indices of the vertices per each polygon
    vtxId = [
        0,1,2,3, # front  face  (Z+)
        1,5,6,2, # right  side  (X+)
        5,4,7,6, # back   face  (Z-)
        4,0,3,7, # left   side  (X-)
        0,4,5,1, # bottom face  (Y-)
        3,2,6,7  # top    face  (Y+)
    ]
    for faceIndex in range(6):
        lMesh.BeginPolygon(faceIndex)
        for vertexIndex in range(4):
            lMesh.AddPolygon(vtxId[faceIndex * 4 + vertexIndex])
        lMesh.EndPolygon()
    
    # specify normals per control point.
    # For compatibility, we follow the rules stated in the 
    # layer class documentation: normals are defined on layer 0 and
    # are assigned by control point.
    normLayer = FbxLayerElementNormal.Create(lMesh, "")
    normLayer.SetMappingMode(FbxLayerElement.eByControlPoint)
    normLayer.SetReferenceMode(FbxLayerElement.eDirect)

    lNormals = [
        FbxVector4(-0.577350258827209,-0.577350258827209, 0.577350258827209, 1.0),
        FbxVector4(0.577350258827209,-0.577350258827209, 0.577350258827209, 1.0),
        FbxVector4(0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0),
        FbxVector4(-0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0),
        FbxVector4(-0.577350258827209,-0.577350258827209,-0.577350258827209, 1.0),
        FbxVector4(0.577350258827209,-0.577350258827209,-0.577350258827209, 1.0),
        FbxVector4(0.577350258827209, 0.577350258827209,-0.577350258827209, 1.0),
        FbxVector4(-0.577350258827209, 0.577350258827209,-0.577350258827209, 1.0)
    ]
    for normalIndex in range(len(lNormals)):
        normLayer.GetDirectArray().Add(lNormals[normalIndex])

    layer.SetNormals(normLayer)
    
    # Create color vertices
    # We choose to define one color per control point. The other choice would
    # have been to use the eByPolygonVertex mapping mode. In this second case,
    # the reference mode should become eIndexToDirect.
    vtxcLayer = FbxLayerElementVertexColor.Create(lMesh, "")
    vtxcLayer.SetMappingMode(FbxLayerElement.eByControlPoint)
    vtxcLayer.SetReferenceMode(FbxLayerElement.eDirect)

    lColors = [
        # colors used for the materials
        FbxColor(1.0, 1.0, 1.0, 1.0),
        FbxColor(1.0, 1.0, 0.0, 1.0),
        FbxColor(1.0, 0.0, 1.0, 1.0),
        FbxColor(0.0, 1.0, 1.0, 1.0),
        FbxColor(0.0, 0.0, 1.0, 1.0),
        FbxColor(1.0, 0.0, 0.0, 1.0),
        FbxColor(0.0, 1.0, 0.0, 1.0),
        FbxColor(0.0, 0.0, 0.0, 1.0),
    ]
    for colorIndex in range(len(lColors)):
        vtxcLayer.GetDirectArray().Add(lColors[colorIndex])

    layer.SetVertexColors(vtxcLayer)

    # create polygroups. 
    # We are going to make a first group with the 4 sides.
    # And a second group with the top and bottom sides.
    # NOTE that the only reference mode allowed is eINDEX
    pgrpLayer = FbxLayerElementPolygonGroup.Create(lMesh, "")
    pgrpLayer.SetMappingMode(FbxLayerElement.eByPolygon)
    pgrpLayer.SetReferenceMode(FbxLayerElement.eIndex)
    pgrpLayer.GetIndexArray().Add(0) # front face assigned to group 0
    pgrpLayer.GetIndexArray().Add(0) # right side assigned to group 0
    pgrpLayer.GetIndexArray().Add(0) # back face assigned to group 0
    pgrpLayer.GetIndexArray().Add(0) # left side assigned to group 0
    pgrpLayer.GetIndexArray().Add(1) # bottom face assigned to group 1
    pgrpLayer.GetIndexArray().Add(1) # top face assigned to group 1

    layer.SetPolygonGroups(pgrpLayer)
    
    # create the UV textures mapping.
    # On layer 0 all the faces have the same texture
    uvLayer = FbxLayerElementUV.Create(lMesh, "")
    uvLayer.SetMappingMode(FbxLayerElement.eByPolygon)
    uvLayer.SetReferenceMode(FbxLayerElement.eIndexToDirect)

    lUVs = [
        FbxVector2(0.0, 0.0), FbxVector2(1.0, 0.0), FbxVector2(0.0, 1.0), FbxVector2(1.0, 1.0),
        FbxVector2(0.0, 2.0), FbxVector2(1.0, 2.0), FbxVector2(0.0, 3.0), FbxVector2(1.0, 3.0),
        FbxVector2(0.0, 4.0), FbxVector2(1.0, 4.0), FbxVector2(2.0, 0.0), FbxVector2(2.0, 1.0),
        FbxVector2(-1.0, 0.0), FbxVector2(-1.0, 1.0),
    ]

    # indices of the uvs per each polygon
    uvsId = [0,1,3,2,2,3,5,4,4,5,7,6,6,7,9,8,1,10,11,3,12,0,2,13]
    
    for uvIndex in range(len(lUVs)):
        uvLayer.GetDirectArray().Add(lUVs[uvIndex])

    for uvIdIndex in range(len(uvsId)):
        uvLayer.GetIndexArray().Add(uvsId[uvIdIndex])
    layer.SetUVs(uvLayer)
    
    # Create texture.
    lTexture = CreateTexture(pSdkManager, BACKGROUND_IMAGE_NAME, BACKGROUND_IMAGE)

    texLayer =  FbxLayerElementTexture.Create(lMesh, "")
    texLayer.SetMappingMode(FbxLayerElement.eByPolygon)
    texLayer.SetReferenceMode(FbxLayerElement.eIndexToDirect)
    texLayer.GetDirectArray().Add(lTexture)
    for index in range(6):
        texLayer.GetIndexArray().Add(0)

    layer.SetTextures(FbxLayerElement.eTextureDiffuse,texLayer)
    
    # On layer 1 only one texture is mapped on the front face of the cube.
    lMesh.CreateLayer()
    layer = lMesh.GetLayer(1)

    uvLayer = FbxLayerElementUV.Create(lMesh, "")
    uvLayer.SetMappingMode(FbxLayerElement.eByPolygonVertex)
    uvLayer.SetReferenceMode(FbxLayerElement.eIndexToDirect)
    for index in range(4):
        uvLayer.GetDirectArray().Add(lUVs[index])

    for index in range(24):
        uvLayer.GetIndexArray().Add(uvsId[index%4])
    layer.SetUVs(uvLayer)

    lTexture = CreateTexture(pSdkManager, LAYER1_IMAGE_NAME, LAYER1_IMAGE)
    
    texLayer =  FbxLayerElementTexture.Create(lMesh, "")
    texLayer.SetBlendMode(FbxLayerElementTexture.eModulate)
    texLayer.SetMappingMode(FbxLayerElement.eByPolygon)
    texLayer.SetReferenceMode(FbxLayerElement.eIndexToDirect)
    texLayer.GetDirectArray().Add(lTexture)

    for index in range(6):
        texLayer.GetIndexArray().Add(0)

    layer.SetTextures(FbxLayerElement.eTextureDiffuse,texLayer)

    
    # Make a third layer where only one texture is mapped on the
    # front face of the cube. 

    # create layer 2
    lMesh.CreateLayer()
    layer = lMesh.GetLayer(2)

    uvLayer1 = FbxLayerElementUV.Create(lMesh, "")
    uvLayer1.SetMappingMode(FbxLayerElement.eByPolygonVertex)
    uvLayer1.SetReferenceMode(FbxLayerElement.eIndexToDirect)
    for index in range(4):
        uvLayer1.GetDirectArray().Add(lUVs[index])

    for index in range(24):
        uvLayer1.GetIndexArray().Add(uvsId[index%4])
    layer.SetUVs(uvLayer1)

    lTexture = CreateTexture(pSdkManager, LAYER2_IMAGE_NAME, LAYER2_IMAGE)

    texLayer =  FbxLayerElementTexture.Create(lMesh, "")
    texLayer.SetBlendMode(FbxLayerElementTexture.eModulate)
    texLayer.SetMappingMode(FbxLayerElement.eByPolygon)
    texLayer.SetReferenceMode(FbxLayerElement.eIndexToDirect)
    texLayer.GetDirectArray().Add(lTexture)

    texLayer.GetIndexArray().Add(0)
    for index in range(6):
        texLayer.GetIndexArray().Add(-1) # no texture

    layer.SetTextures(FbxLayerElement.eTextureDiffuse, texLayer)
    
    for index in range(6):
        lMaterialName = FbxString("material")
        lMaterialName += FbxString(index)

        lMaterial = FbxSurfacePhong.Create(pSdkManager,lMaterialName.Buffer())

        # Generate primary and secondary colors.
        lMaterial.Emissive.Set(FbxDouble3(0.0, 0.0, 0.0))
        lMaterial.Ambient.Set(FbxDouble3(lColors[index].mRed, lColors[index].mGreen, lColors[index].mGreen))
        lMaterial.Diffuse.Set(FbxDouble3(1.0, 1.0, 1.0))
        lMaterial.Specular.Set(FbxDouble3(0.0, 0.0, 0.0))
        lMaterial.TransparencyFactor.Set(0.0)
        lMaterial.Shininess.Set(0.5)
        lMaterial.ShadingModel.Set(FbxString("phong"))

        # get the node of mesh, add material for it.
        lMeshNode = lMesh.GetNode()
        if lMeshNode:
            lMeshNode.AddMaterial(lMaterial)
    
    return lNode

def CreateTexture(pSdkManager, pName, pFilename):
    lTexture = FbxFileTexture.Create(pSdkManager, pName)
    lTexture.SetFileName(pFilename)
    lTexture.SetTextureUse(FbxTexture.eStandard)
    lTexture.SetMappingType(FbxTexture.eUV)
    lTexture.SetMaterialUse(FbxFileTexture.eModelMaterial)
    lTexture.SetSwapUV(False)
    lTexture.SetTranslation(0.0, 0.0)
    lTexture.SetScale(1.0, 1.0)
    lTexture.SetRotation(0.0, 0.0)
    return lTexture

if __name__ == "__main__":
    try:
        import FbxCommon
        from fbx import *
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
    (lSdkManager, lScene) = FbxCommon.InitializeSdkObjects()

    # Create the scene.
    lResult = CreateScene(lSdkManager, lScene)

    if lResult == False:
        print("\n\nAn error occurred while creating the scene...\n")
        lSdkManager.Destroy()
        sys.exit(1)

    # Save the scene.
    # The example can take an output file name as an argument.
    if len(sys.argv) > 1:
        lResult = FbxCommon.SaveScene(lSdkManager, lScene, sys.argv[1])
    # A default output file name is given otherwise.
    else:
        lResult = FbxCommon.SaveScene(lSdkManager, lScene, SAMPLE_FILENAME)

    if lResult == False:
        print("\n\nAn error occurred while saving the scene...\n")
        lSdkManager.Destroy()
        sys.exit(1)

    # Destroy all objects created by the FBX SDK.
    lSdkManager.Destroy()
   
    sys.exit(0)
