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
from fbx import FbxSurfaceMaterial
from fbx import FbxLayerElement
from fbx import FbxLayeredTexture
from fbx import FbxTexture

showInfo = True
def DisplayTextureInfo(pTexture, pBlendMode):
    textureDict = {}
    if showInfo: DisplayString("            Name: \"", pTexture.GetName(), "\"")
    textureDict['name'] = pTexture.GetName()[:]
    if showInfo: DisplayString("            File Name: \"", pTexture.GetFileName(), "\"")
    textureDict['filename'] =  pTexture.GetFileName()[:]
    if showInfo: DisplayDouble("            Scale U: ", pTexture.GetScaleU())
    if showInfo: DisplayDouble("            Scale V: ", pTexture.GetScaleV())
    if showInfo: DisplayDouble("            Translation U: ", pTexture.GetTranslationU())
    if showInfo: DisplayDouble("            Translation V: ", pTexture.GetTranslationV())
    if showInfo: DisplayBool("            Swap UV: ", pTexture.GetSwapUV())
    if showInfo: DisplayDouble("            Rotation U: ", pTexture.GetRotationU())
    if showInfo: DisplayDouble("            Rotation V: ", pTexture.GetRotationV())
    if showInfo: DisplayDouble("            Rotation W: ", pTexture.GetRotationW())

    lAlphaSources = [ "None", "RGB Intensity", "Black" ]

    if showInfo: DisplayString("            Alpha Source: ", lAlphaSources[pTexture.GetAlphaSource()])
    if showInfo: DisplayDouble("            Cropping Left: ", pTexture.GetCroppingLeft())
    if showInfo: DisplayDouble("            Cropping Top: ", pTexture.GetCroppingTop())
    if showInfo: DisplayDouble("            Cropping Right: ", pTexture.GetCroppingRight())
    if showInfo: DisplayDouble("            Cropping Bottom: ", pTexture.GetCroppingBottom())

    lMappingTypes = [ "Null", "Planar", "Spherical", "Cylindrical", "Box", "Face", "UV", "Environment"]

    if showInfo: DisplayString("            Mapping Type: ", lMappingTypes[pTexture.GetMappingType() if pTexture.GetMappingType() else 0])

    if pTexture.GetMappingType() == FbxTexture.ePlanar:
        lPlanarMappingNormals = ["X", "Y", "Z" ]
        DisplayString("            Planar Mapping Normal: ", lPlanarMappingNormals[pTexture.GetPlanarMappingNormal()])

    lBlendModes   = ["Translucent", "Add", "Modulate", "Modulate2"]   
    if ((type(pBlendMode) is tuple) and pBlendMode[0]) or ((type(pBlendMode) is tuple) and  pBlendMode >= 0):
        DisplayString("            Blend Mode: ", lBlendModes[pBlendMode if not type(pBlendMode) is tuple else pBlendMode[1]])
    if showInfo: DisplayDouble("            Alpha: ", pTexture.GetDefaultAlpha())

    lMaterialUses = ["Model Material", "Default Material"]

    if showInfo: DisplayString("            Material Use: ", lMaterialUses[pTexture.GetMaterialUse()])

    pTextureUses = ["Standard", "Shadow Map", "Light Map", "Spherical Reflexion Map", "Sphere Reflexion Map"]

    if showInfo: DisplayString("            Texture Use: ", pTextureUses[pTexture.GetTextureUse()])
    if showInfo: DisplayString("")
    print textureDict
    return textureDict
def FindAndDisplayTextureInfoByProperty(pProperty, pDisplayHeader, pMaterialIndex):
    if pProperty.IsValid():
        #Here we have to check if it's layeredtextures, or just textures:
        lLayeredTextureCount = pProperty.GetSrcObjectCount(FbxLayeredTexture.ClassId)
        if lLayeredTextureCount > 0:
            for j in range(lLayeredTextureCount):
                DisplayInt("    Layered Texture: ", j)
                lLayeredTexture = pProperty.GetSrcObject(FbxLayeredTexture.ClassId, j)
                lNbTextures = lLayeredTexture.GetSrcObjectCount(FbxTexture.ClassId)
                for k in range(lNbTextures):
                    lTexture = lLayeredTexture.GetSrcObject(FbxTexture.ClassId,k)
                    if lTexture:
                        if pDisplayHeader:
                            DisplayInt("    Textures connected to Material ", pMaterialIndex)
                            pDisplayHeader = False

                        # NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
                        # Why is that?  because one texture can be shared on different layered textures and might
                        # have different blend modes.

                        lBlendMode = lLayeredTexture.GetTextureBlendMode(k)
                        DisplayString("    Textures for ", pProperty.GetName())
                        DisplayInt("        Texture ", k)  
                        DisplayTextureInfo(lTexture, lBlendMode)
        else:
            # no layered texture simply get on the property
            lNbTextures = pProperty.GetSrcObjectCount(FbxTexture.ClassId)
            for j in range(lNbTextures):
                lTexture = pProperty.GetSrcObject(FbxTexture.ClassId,j)
                if lTexture:
                    # display connectMareial header only at the first time
                    if pDisplayHeader:
                        DisplayInt("    Textures connected to Material ", pMaterialIndex)
                        pDisplayHeader = False
                    
                    DisplayString("    Textures for ", pProperty.GetName().Buffer())
                    DisplayInt("        Texture ", j)  
                    DisplayTextureInfo(lTexture, -1)

        lNbTex = pProperty.GetSrcObjectCount(FbxTexture.ClassId)
        for lTextureIndex in range(lNbTex):
            lTexture = pProperty.GetSrcObject(FbxTexture.ClassId, lTextureIndex) 


def DisplayTexture(pGeometry):
    lNbMat = pGeometry.GetNode().GetSrcObjectCount(FbxSurfaceMaterial.ClassId)
    textures = {}
    for lMaterialIndex in range(lNbMat):
        lMaterial = pGeometry.GetNode().GetSrcObject(FbxSurfaceMaterial.ClassId, lMaterialIndex)
        lDisplayHeader = True

        #go through all the possible textures
        if lMaterial:            
            for lTextureIndex in range(FbxLayerElement.sTypeTextureCount()):
                lProperty = lMaterial.FindProperty(FbxLayerElement.sTextureChannelNames(lTextureIndex))
                FindAndDisplayTextureInfoByProperty(lProperty, lDisplayHeader, lMaterialIndex, textures) 
