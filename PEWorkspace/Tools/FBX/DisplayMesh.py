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
from StoreMeshToFile import *

from fbx import FbxLayerElement
from fbx import FbxSurfaceMaterial
from fbx import FbxLayeredTexture
from fbx import FbxTexture
from fbx import FbxVector4, FbxMatrix
from DisplayMaterial import DisplayMaterial
from DisplayTexture  import DisplayTexture, DisplayTextureInfo  
from DisplayLink     import DisplayLink
from DisplayShape    import DisplayShape
import sys

showInfo = False
def DisplayMesh(lScene, assetType, pNode, hierarchy, skelRoots, skelDirectMaps, meshToBindMatrixMap, package, meshIndex, flipAxis, peParserContext):
    lMesh = pNode.GetNodeAttribute ()

    peParserContext.logger.StartScope("Step 6.%d Analyzing Mesh %s" % (meshIndex, pNode.GetName()), True)

    bindMatrix = meshToBindMatrixMap.get(pNode.GetName(), None)
    if bindMatrix:
        peParserContext.logger.AddLine("This mesh is associated with bindpose so all control points will be transformed by bind pose matrix")
        peParserContext.logger.AddLine("The bindpose usually has the scaling needed to convert to meters")
        
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(0,0),bindMatrix.Get(0,1),bindMatrix.Get(0,2),bindMatrix.Get(0,3),))
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(1,0),bindMatrix.Get(1,1),bindMatrix.Get(1,2),bindMatrix.Get(1,3),))
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(2,0),bindMatrix.Get(2,1),bindMatrix.Get(2,2),bindMatrix.Get(2,3),))
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(3,0),bindMatrix.Get(3,1),bindMatrix.Get(3,2),bindMatrix.Get(3,3),))
    else:
        peParserContext.logger.AddLine("Creating a matrix to scale mesh")
        bindMatrix = FbxMatrix()
        bindMatrix.SetTRS(FbxVector4(0, 0, 0), FbxVector4(0, 0, 0), FbxVector4(peParserContext.conversionFactor, peParserContext.conversionFactor, peParserContext.conversionFactor))
        
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(0,0),bindMatrix.Get(0,1),bindMatrix.Get(0,2),bindMatrix.Get(0,3),))
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(1,0),bindMatrix.Get(1,1),bindMatrix.Get(1,2),bindMatrix.Get(1,3),))
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(2,0),bindMatrix.Get(2,1),bindMatrix.Get(2,2),bindMatrix.Get(2,3),))
        peParserContext.logger.AddLine("%f %f %f %f" % (bindMatrix.Get(3,0),bindMatrix.Get(3,1),bindMatrix.Get(3,2),bindMatrix.Get(3,3),))
    
    # 6.mesh.1
    polyToMaterialMap = DisplayMaterialMapping(lMesh, meshIndex, peParserContext)

    # 6.mesh.2
    materials = DisplayMaterial(lMesh, meshIndex, peParserContext.logger, peParserContext.logLevel)
    #print polyToMaterialMap
    if len(materials) != max(polyToMaterialMap) + 1:
        print "  ERROR: len(materials) != max material index referenced + 1:", len(materials), "!= ", max(polyToMaterialMap), "+1"
        if len(materials) > max(polyToMaterialMap) + 1:
            print "  There are more materials than referenced by polys. This is ok."
        else:
            print "  Polys reference material out of range. This will crash"
    
    #print materials

    # 6.mesh.3
    DisplayMaterialConnections(lMesh, materials, meshIndex, peParserContext)
    #print "Materials:", materials
    print "  Now materials list has full information of materials"

    # 6.mesh.4
    finalVertices = DisplayControlsPoints(lMesh, bindMatrix, meshIndex, peParserContext)
    if peParserContext.logLevel > 1:
        print "  Final Vertices After control point pass:"
        for f in finalVertices:
            f.printToScreen()
    else:
        print "  Have %d Final Vertices after simple control point pass" % len(finalVertices)
    
    # 6.mesh.5
    finalVertices, originalControlPointToNewVerticesMapping, polysSortedByMaterial, trisSortedByMaterial = DisplayPolygons(lMesh, finalVertices, bindMatrix, polyToMaterialMap, materials, meshIndex, peParserContext)
    print "  Num of Final Vertices (control points) after polygon pass: %d" % (len(finalVertices),)
    triSummary = "  trisSortedByMaterial: %d materials with tri counts: " % len(trisSortedByMaterial)
    for mat in trisSortedByMaterial:
        triSummary += " %d" % (len(mat)/3)

    #print "  trisSortedByMaterial: %d materials", trisSortedByMaterial
    #DisplayTexture(lMesh)

    # 6.mesh.6
    skelRoot, skelDirectMap, unusedJointsInSkinBind = DisplayLink(lMesh, finalVertices, originalControlPointToNewVerticesMapping, skelRoots, skelDirectMaps, meshIndex, peParserContext.logLevel)
    
    
    print "  Gathered skin bind info Mesh is skinned to skelRoot %s . unused joints in skin bind:" % (skelRoot['name'] if skelRoot else '<none>'), unusedJointsInSkinBind
    print "  Checking that all used skeleton joints have bind transform"
    
    if skelDirectMap:
        for jointName in skelDirectMap.keys():
            if not skelDirectMap[jointName].has_key('matrix'):
                if not jointName in unusedJointsInSkinBind:
                    print "    ERROR: joint %s has skin bound to it but no bind pose transform!"
                skelDirectMap[jointName]['matrix'] = [1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1]
        
    #if showInfo:
    peParserContext.logger.StartScope("  Final Vertices:", showInfo, newFileName=pNode.GetName()+'_final_verts.html')
    iv = 0
    for f in finalVertices:
        peParserContext.logger.AddLine("Vertex[%d]"%iv, showInfo)
        f.logToScreen(peParserContext.logger, showInfo)
        iv += 1
    peParserContext.logger.EndScope("  Ended up with %d final vertices" % len(finalVertices), True)
    
    #6.mesh.7
    DisplayString("Step 6.%d.7 Splitting Material Patches Into Bone Segments" % (meshIndex,))

    materialJointSegments = []
    matIndex = 0
    for mat in trisSortedByMaterial:
        materialJointSegments.append(SplitMaterialIntoBoneSegments(mat, materials[matIndex], finalVertices, matIndex, meshIndex, peParserContext.logLevel))
        matIndex += 1
    print "  Ended up with %d final vertices after bone segment split" % len(finalVertices)
    StoreMeshToFiles(lScene, pNode.GetName(), skelRoot, finalVertices, trisSortedByMaterial, materialJointSegments, materials, package, flipAxis)
    raw_input()
    peParserContext.logger.EndScope()
    return pNode.GetName()
    
    DisplayShape(lMesh)



def GetVertexAttrByControlPointMapping(layers, bindMatrix, attrName, lControlPointsCount, vertexList, setFunc, cloneFunc):

    '''
            leUV = pMesh.GetLayer(l).GetUVs()
     
            header = "            Texture UV (on layer %d): " % l 

            if leUV.GetMappingMode() == FbxLayerElement.eByControlPoint:
                if leUV.GetReferenceMode() == FbxLayerElement.eDirect:
                    Display2DVector(header, leUV.GetDirectArray().GetAt(lControlPointIndex))
                elif leUV.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                    id = leUV.GetIndexArray().GetAt(lControlPointIndex)
                    Display2DVector(header, leUV.GetDirectArray().GetAt(id))
            elif leUV.GetMappingMode() ==  FbxLayerElement.eByPolygonVertex:
                lTextureUVIndex = pMesh.GetTextureUVIndex(i, j)
                if leUV.GetReferenceMode() == FbxLayerElement.eDirect or \
                   leUV.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                    Display2DVector(header, leUV.GetDirectArray().GetAt(lTextureUVIndex))
            elif leUV.GetMappingMode() == FbxLayerElement.eByPolygon or \
                 leUV.GetMappingMode() == FbxLayerElement.eAllSame or \
                 leUV.GetMappingMode() ==  FbxLayerElement.eNone:
                 # doesn't make much sense for UVs
                pass
    '''
    numVertsGathered = 0
    if len(layers) > 0:
        for attrs, fileIndex in layers:
            print "      Gathering", attrName, "for each control point in layer[%d]" % fileIndex
            if attrs.GetMappingMode() == FbxLayerElement.eByControlPoint:
                #header = "            Normal Vector (on layer %d): " % j 
                if attrs.GetReferenceMode() == FbxLayerElement.eDirect:
                    for i in range(lControlPointsCount):
                        vec = attrs.GetDirectArray().GetAt(i)
                        if bindMatrix:
                            vec = bindMatrix.MultNormalize(vec)
                            vec.Normalize()
        
                        setFunc(vertexList[i], fileIndex, cloneFunc(vec))
                        numVertsGathered += 1
                elif attrs.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                    for i in range(lControlPointsCount):
                        id = attrs.GetIndexArray().GetAt(i)
                        vec = attrs.GetDirectArray().GetAt(id)
                        if bindMatrix:
                            vec = bindMatrix.MultNormalize(vec)
                            vec.Normalize()
                        setFunc(vertexList[id], fileIndex, cloneFunc(vec))
                        numVertsGathered += 1
                else:
                    print ("PE: Error: Unrecognized GetReferenceMode: ", attrs.GetReferenceMode())
                    #for i in range(lControlPointsCount):
                    #    writeFunc(nbf, [0, 0, 1, 0])
            else:
                print ("PE: Error: Unrecognized Mapping Mode: ", attrs.GetMappingMode(), "while exporting attribute", attrName, '[', fileIndex, ']')
                #for i in range(lControlPointsCount):
                #    writeFunc(nbf, [0, 0, 1, 0])
            #nbf.close()
    else:
        pass
        #print ("PE: Error: Mesh doesnt have", attrName, "layer (vertex attr)")
        #for i in range(lControlPointsCount):
        #    nbf.write("0 0 1\n")
def GetVertexAttrByPolygonVertexMapping(layers, bindMatrix, attrName, pMesh, vertexIndexInMesh, polyIndex, polyControlPoints, lPolygonSize, writeFunc, vertexList, originalControlPointToNewVerticesMapping,
    setFunc, getPolygonVertexFunc, cloneFunc):

    '''
            leUV = pMesh.GetLayer(l).GetUVs()
     
            header = "            Texture UV (on layer %d): " % l 

            if leUV.GetMappingMode() == FbxLayerElement.eByControlPoint:
                if leUV.GetReferenceMode() == FbxLayerElement.eDirect:
                    Display2DVector(header, leUV.GetDirectArray().GetAt(lControlPointIndex))
                elif leUV.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                    id = leUV.GetIndexArray().GetAt(lControlPointIndex)
                    Display2DVector(header, leUV.GetDirectArray().GetAt(id))
            elif leUV.GetMappingMode() ==  FbxLayerElement.eByPolygonVertex:
                lTextureUVIndex = pMesh.GetTextureUVIndex(i, j)
                if leUV.GetReferenceMode() == FbxLayerElement.eDirect or \
                   leUV.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                    Display2DVector(header, leUV.GetDirectArray().GetAt(lTextureUVIndex))
            elif leUV.GetMappingMode() == FbxLayerElement.eByPolygon or \
                 leUV.GetMappingMode() == FbxLayerElement.eAllSame or \
                 leUV.GetMappingMode() ==  FbxLayerElement.eNone:
                 # doesn't make much sense for UVs
                pass
    '''
    numPointsModified = 0
    numPointsAdded = 0
    if showInfo: print 'Poly control points:', polyControlPoints
    if len(layers) > 0:
        for attrs, fileIndex in layers:
            #print "      Gathering", attrName, "for each polygon in layer[%d]" % fileIndex
            
            if attrs.GetMappingMode() == FbxLayerElement.eByPolygonVertex:
                #in this mapping mode each vertex has its own value, index of the vertex is whatever the index of the vertex in continuous polygon list
                for iVertInPoly in range(lPolygonSize):
                    controlPointIndex = polyControlPoints[iVertInPoly] # getPolygonVertexFunc(polyIndex, iVertInPoly) #index to control point. Polygon Vertex == index of control point
                    originalControlPointIndex = pMesh.GetPolygonVertex(polyIndex, iVertInPoly)
                    if showInfo: print "index is", controlPointIndex, "for poly[", polyIndex, "] vert [", iVertInPoly, "]"
                    #if vertexIndexInMesh + iVertInPoly != controlPointIndex:
                    #    print "Error", str(vertexIndexInMesh + iVertInPoly) + "!= " + str(controlPointIndex)
                    #    raw_input()
                    
                    if attrs.GetReferenceMode() == FbxLayerElement.eDirect:
                        id = vertexIndexInMesh + iVertInPoly
                    elif attrs.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                        id = attrs.GetIndexArray().GetAt(vertexIndexInMesh + iVertInPoly)
                    else:
                        print ("PE: Error: Unrecognized GetReferenceMode: ", attrs.GetReferenceMode())
                        setFunc(vertexList[controlPointIndex], fileIndex, [0, 0, 1, 0])
                        id = -1
                    if id >= 0:
                        #if showInfo: 
                        #print "Direct id:", id, "/", attrs.GetDirectArray().GetCount(), "Control point index:", controlPointIndex, "/", len(vertexList)
                        vec = attrs.GetDirectArray().GetAt(id)
                        if bindMatrix:
                            vec = bindMatrix.MultNormalize(vec)
                            vec.Normalize()
                        
                        res = setFunc(vertexList[controlPointIndex], fileIndex, cloneFunc(vec))
                        #need to clone final vertex to accomodate new value
                        if res == False:
                            if showInfo:
                                print "Adding new control point because poly[%d]vert[%d] tries using control point[%d] with %s value of %s while the control point value is already:" % (polyIndex, iVertInPoly, controlPointIndex, attrName, str(cloneFunc(attrs.GetDirectArray().GetAt(id))))
                                vertexList[controlPointIndex].printToScreen()
                            vertexList[controlPointIndex].useCount -= 1
                            newVertex = FinalVertex(vertexList[controlPointIndex])
                            setFunc(newVertex, fileIndex, cloneFunc(attrs.GetDirectArray().GetAt(id)), True) # force new value in new vertex
                            vertexList.append(newVertex)
                            polyControlPoints[iVertInPoly] = len(vertexList)-1 #make sure that this poly vertex points to new control point
                            originalControlPointToNewVerticesMapping[originalControlPointIndex].append(len(vertexList)-1)
                            newVertex.useCount = 1
                            if showInfo:
                                print "New referenced control point is:"
                                vertexList[polyControlPoints[iVertInPoly]].printToScreen()
                            numPointsAdded += 1
                        else:
                            numPointsModified += 1
            else:
                print ("PE: Error: Unrecognized Mapping Mode: ", attrs.GetMappingMode(), "while exporting attribute", attrName, '[', fileIndex, ']')
                for i in range(lControlPointsCount):
                    setFunc(vertexList[controlPointIndex], fileIndex, [0, 0, 1, 0])
    else:
        pass
        
        #print ("PE: Error: Mesh doesnt have", attrName, "layer (vertex attr)")
        #for i in range(lControlPointsCount):
        #    nbf.write("0 0 1\n")
    return numPointsModified, numPointsAdded
def DisplayControlsPoints(pMesh, bindMatrix, meshIndex, peParserContext):

    DisplayString("Step 6.%d.4 Check control points to make base FinalVertices list" % meshIndex)
    
    if peParserContext.logLevel > 0:
        print "  DisplayControlsPoints entry... Will make list fo final vertices based on control points"
    lControlPointsCount = pMesh.GetControlPointsCount()
    lControlPoints = pMesh.GetControlPoints()

    if peParserContext.logLevel > 0:
        print "    Mesh has [%d] Control Points" % lControlPointsCount
        print "    Mesh has [%d] Layers" % pMesh.GetLayerCount()
    
    finalVertices = []
    for i in range(lControlPointsCount):
        #DisplayInt("        Control Point ", i)
        #Display3DVector("            Coordinates: ", lControlPoints[i])
        v = FinalVertex()
        point = lControlPoints[i]
        if bindMatrix: point = bindMatrix.MultNormalize(point)
        v.setPosition(0, V3ToList(point))
        finalVertices.append(v)
    #first we construct a vertex list with a FinalVertex for every control point. this potetnially will be able to define whole mesh
    #however, some vertex attributes are stored per polygon, so they have potential to be different for same final vertex
    #for example poly0 and poly1 could use same control point but have different tangent, in which case we will duplicate FinalVertex corresponding to control point
    # and set a different tangent in it
    normalsLayers, texCoordLayers, tangentLayers = [], [], []
    for j in range(pMesh.GetLayerCount()):
        leNormals, leTexCoords, tangents = pMesh.GetLayer(j).GetNormals(), pMesh.GetLayer(j).GetUVs(), pMesh.GetLayer(j).GetTangents()
        if leNormals : normalsLayers.append((leNormals, len(normalsLayers)))
        if leTexCoords: texCoordLayers.append((leTexCoords, len(texCoordLayers)))
        if tangents: tangentLayers.append((tangents, len(tangentLayers)))
    def ControlPointMappingMode(x):
        return x[0].GetMappingMode() == FbxLayerElement.eByControlPoint
    GetVertexAttrByControlPointMapping(filter(ControlPointMappingMode, normalsLayers), bindMatrix, 'normal', lControlPointsCount, finalVertices, FinalVertex.setNormal, V3ToList)
    GetVertexAttrByControlPointMapping(filter(ControlPointMappingMode, texCoordLayers), None, 'texcoord', lControlPointsCount, finalVertices, FinalVertex.setTexCoord, V2ToList)
    GetVertexAttrByControlPointMapping(filter(ControlPointMappingMode, tangentLayers), bindMatrix, 'tangent', lControlPointsCount, finalVertices, FinalVertex.setTangent, V3ToList)
    DisplayString("")
    print "    Gathered %d normals %d tex coord %d tangent sets for base Controls Points" % (len(normalsLayers), len(texCoordLayers), len(tangentLayers))

    if peParserContext.logLevel > 0:
        print "  DisplayControlsPoints end."
    
    return finalVertices

def DisplayPolygons(pMesh, finalVertices, bindMatrix, polyToMaterialMap, materials, meshIndex, peParserContext):

    DisplayString("Step 6.%d.5 DisplayPolygons entry... will potentially expand list of Final vertices. will split vertices into materials" % meshIndex)
    

    lPolygonCount = pMesh.GetPolygonCount()
    lControlPoints = pMesh.GetControlPoints() 

    #DisplayString("    Polygons")
    
    polysSortedByMaterials = [ [] for x in range(len(materials)) ]
    trisSortedByMaterials = [ [] for x in range(len(materials)) ]
    
    normalsLayers, texCoordLayers, tangentLayers = [], [], []
    for j in range(pMesh.GetLayerCount()):
        leNormals, leTexCoords, tangents = pMesh.GetLayer(j).GetNormals(), pMesh.GetLayer(j).GetUVs(), pMesh.GetLayer(j).GetTangents()
        if leNormals : normalsLayers.append((leNormals, len(normalsLayers)))
        if leTexCoords: texCoordLayers.append((leTexCoords, len(texCoordLayers)))
        if tangents: tangentLayers.append((tangents, len(tangentLayers)))
    def ByPolygonVertexMappingMode(x):
        return x[0].GetMappingMode() == FbxLayerElement.eByPolygonVertex
    
    texCoordLayers = filter(ByPolygonVertexMappingMode, texCoordLayers)
    tangentLayers = filter(ByPolygonVertexMappingMode, tangentLayers)
    normalsLayers = filter(ByPolygonVertexMappingMode, normalsLayers)
    
    print "    Found %d tex coord layers %d normals layers %d tangent layers that are mapped by polygon vertex" % (len(texCoordLayers), len(normalsLayers), len(tangentLayers))
    #create list of final vertices that currently matches control points
    #going through the layers that have by polygon mapping could add more vertices
    for i in range(lPolygonCount):
        lPolygonSize = pMesh.GetPolygonSize(i)
        for j in range(lPolygonSize):
            iControlPoint = pMesh.GetPolygonVertex(i, j)
            finalVertices[iControlPoint].useCount += 1
    
    originalControlPointToNewVerticesMapping = []
    newFinalVertices = []
    print ("    Preliminary check of control points: removing unused ones")
    for i in range(len(finalVertices)):
        if finalVertices[i].useCount == 0:
            originalControlPointToNewVerticesMapping.append(None)
        else:
            newFinalVertices.append(finalVertices[i])
            originalControlPointToNewVerticesMapping.append([len(newFinalVertices)-1])
    print("    Vertex list length is %d was %d" % (len(newFinalVertices), len(finalVertices)))
    finalVertices = newFinalVertices
    
    vertexId = 0
    pointTracking = {
        'texcoord' : {'numPointsAdded' : 0, 'numPointsModified' : 0 },
        'tangent' : {'numPointsAdded' : 0, 'numPointsModified' : 0 },
        'normal' : {'numPointsAdded' : 0, 'numPointsModified' : 0 }
    }
    for i in range(lPolygonCount):
        #print "        Polygon ", i
        
        for l in range(pMesh.GetLayerCount()):
            lePolgrp = pMesh.GetLayer(l).GetPolygonGroups()
            if lePolgrp:
                if lePolgrp.GetMappingMode() == FbxLayerElement.eByPolygon:
                    if lePolgrp.GetReferenceMode() == FbxLayerElement.eIndex:
                        header = "        Assigned to group (on layer %d): " % l 
                        polyGroupId = lePolgrp.GetIndexArray().GetAt(i)
                        print header, polyGroupId
                        DisplayInt(header, polyGroupId)
                else:
                    # any other mapping modes don't make sense
                    print("        \"unsupported group assignment\"")
                    sys.exit(0)
        lPolygonSize = pMesh.GetPolygonSize(i)
        #create prestine poly to control points mapping
        #as we get attributes of vertices, new vertices might be generated, changing the mapping
        polyControlPoints = []
        for iVertInPoly in xrange(lPolygonSize):
            # list of control point indices for polygon, potentially will be different than what is stored in fbx, since per polygon vertex values outnumber number of contol points
            polyControlPoints.append(
                originalControlPointToNewVerticesMapping[pMesh.GetPolygonVertex(i, iVertInPoly)][0]
            ) 
        
        #lTextureUVIndex = pMesh.GetTextureUVIndex(polyIndex, iVertInPoly) #index to control point
        #lTextureUVIndex = pMesh.GetPolygonVertex(polyIndex, iVertInPoly) #index to control point
                    
        #GetVertexAttrByControlPointMapping(filter(ByPolygonVertexMappingMode, normalsLayers), 'normal', lControlPointsCount, Vector3DToFile, finalVertices, FinalVertex.setNormal, pMesh.GetPolygonVertex)
        numPointsModified, numPointsAdded = GetVertexAttrByPolygonVertexMapping(texCoordLayers, None, 'texcoord', pMesh, vertexId, i, polyControlPoints, lPolygonSize, Vector2DToFile, finalVertices, originalControlPointToNewVerticesMapping, FinalVertex.setTexCoord, pMesh.GetPolygonVertex, V2ToList)
        pointTracking['texcoord']['numPointsModified'] += numPointsModified
        pointTracking['texcoord']['numPointsAdded'] += numPointsAdded
        
        numPointsModified, numPointsAdded = GetVertexAttrByPolygonVertexMapping(tangentLayers, bindMatrix, 'tangent', pMesh, vertexId, i, polyControlPoints, lPolygonSize, Vector3DToFile, finalVertices, originalControlPointToNewVerticesMapping, FinalVertex.setTangent, pMesh.GetPolygonVertex, V3ToList)
        pointTracking['tangent']['numPointsModified'] += numPointsModified
        pointTracking['tangent']['numPointsAdded'] += numPointsAdded
        
        numPointsModified, numPointsAdded = GetVertexAttrByPolygonVertexMapping(normalsLayers, bindMatrix, 'normal', pMesh, vertexId, i, polyControlPoints, lPolygonSize, Vector3DToFile, finalVertices, originalControlPointToNewVerticesMapping, FinalVertex.setNormal, pMesh.GetPolygonVertex, V3ToList)
        pointTracking['normal']['numPointsModified'] += numPointsModified
        pointTracking['normal']['numPointsAdded'] += numPointsAdded
        
        matId = polyToMaterialMap[i]
        polysSortedByMaterials[matId].append(i)
        if lPolygonSize == 3:
            for j in range(lPolygonSize):
                lControlPointIndex = polyControlPoints[j] # pMesh.GetPolygonVertex(i, j)
                trisSortedByMaterials[matId].append(lControlPointIndex)
        else:
            print "Error: poly size is not 3, skipping"
        vertexId += lPolygonSize #this value tracks index of next vertex in polygon vertex reference mode
        '''
        for j in range(lPolygonSize):
            lControlPointIndex = polyControlPoints[j] # pMesh.GetPolygonVertex(i, j)
            
            Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex])

            for l in range(pMesh.GetLayerCount()):
                leVtxc = pMesh.GetLayer(l).GetVertexColors()
                if leVtxc:
                    header = "            Color vertex (on layer %d): " % l 

                    if leVtxc.GetMappingMode() == FbxLayerElement.eByControlPoint:
                        if leVtxc.GetReferenceMode() == FbxLayerElement.eDirect:
                            DisplayColor(header, leVtxc.GetDirectArray().GetAt(lControlPointIndex))
                        elif leVtxc.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                                id = leVtxc.GetIndexArray().GetAt(lControlPointIndex)
                                DisplayColor(header, leVtxc.GetDirectArray().GetAt(id))
                    elif leVtxc.GetMappingMode() == FbxLayerElement.eByPolygonVertex:
                            if leVtxc.GetReferenceMode() == FbxLayerElement.eDirect:
                                DisplayColor(header, leVtxc.GetDirectArray().GetAt(vertexId))
                            elif leVtxc.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                                id = leVtxc.GetIndexArray().GetAt(vertexId)
                                DisplayColor(header, leVtxc.GetDirectArray().GetAt(id))
                    elif leVtxc.GetMappingMode() == FbxLayerElement.eByPolygon or \
                         leVtxc.GetMappingMode() ==  FbxLayerElement.eAllSame or \
                         leVtxc.GetMappingMode() ==  FbxLayerElement.eNone:       
                         # doesn't make much sense for UVs
                        pass

                leUV = pMesh.GetLayer(l).GetUVs()
                if leUV:
                    header = "            Texture UV (on layer %d): " % l 

                    if leUV.GetMappingMode() == FbxLayerElement.eByControlPoint:
                        if leUV.GetReferenceMode() == FbxLayerElement.eDirect:
                            Display2DVector(header, leUV.GetDirectArray().GetAt(lControlPointIndex))
                        elif leUV.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                            id = leUV.GetIndexArray().GetAt(lControlPointIndex)
                            Display2DVector(header, leUV.GetDirectArray().GetAt(id))
                    elif leUV.GetMappingMode() ==  FbxLayerElement.eByPolygonVertex:
                        lTextureUVIndex = pMesh.GetTextureUVIndex(i, j)
                        if leUV.GetReferenceMode() == FbxLayerElement.eDirect or \
                           leUV.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                            Display2DVector(header, leUV.GetDirectArray().GetAt(lTextureUVIndex))
                    elif leUV.GetMappingMode() == FbxLayerElement.eByPolygon or \
                         leUV.GetMappingMode() == FbxLayerElement.eAllSame or \
                         leUV.GetMappingMode() ==  FbxLayerElement.eNone:
                         # doesn't make much sense for UVs
                        pass
            # # end for layer
            vertexId += 1
        # # end for polygonSize
        '''
    # # end for polygonCount
    #for v in finalVertices:
    #    print v.printToScreen()
    
    print("    TexCoords Modified: %d Added: %d" % (pointTracking['texcoord']['numPointsModified'], pointTracking['texcoord']['numPointsAdded']))
    print("    Tangents Modified: %d Added: %d" % (pointTracking['tangent']['numPointsModified'], pointTracking['tangent']['numPointsAdded']))
    print("    Normals Modified: %d Added: %d" % (pointTracking['normal']['numPointsModified'], pointTracking['normal']['numPointsAdded']))
    
    print "    Total points added: %d" % (pointTracking['texcoord']['numPointsAdded'] + pointTracking['tangent']['numPointsAdded'] + pointTracking['normal']['numPointsAdded'], )
    
    finalVertices[iControlPoint].useCount += 1
    
    print "  End display polygons"
        
    '''
    # check visibility for the edges of the mesh
    for l in range(pMesh.GetLayerCount()):
        leVisibility=pMesh.GetLayer(0).GetVisibility()
        if leVisibility:
            header = "    Edge Visibilty (on layer %d): " % l
            DisplayString(header)
            # should be eByEdge
            if leVisibility.GetMappingMode() == FbxLayerElement.eByEdge:
                # should be eDirect
                for j in range(pMesh.GetMeshEdgeCount()):
                    DisplayInt("        Edge ", j)
                    DisplayBool("              Edge visibilty: ", leVisibility.GetDirectArray().GetAt(j))

    DisplayString("")
    '''
    return finalVertices, originalControlPointToNewVerticesMapping, polysSortedByMaterials, trisSortedByMaterials

def DisplayTextureNames(pProperty, textures, logger):
    lTextureName = ""
    logger.StartScope("Looking up material property %s" % (pProperty.GetName().Buffer()[:]))
    lLayeredTextureCount = pProperty.GetSrcObjectCount(FbxLayeredTexture.ClassId)
    logger.AddLine("LayeredTextureCount: %d" % lLayeredTextureCount)
    if lLayeredTextureCount > 0:
        logger.AddLine("Looking at layered textures")
    
        for j in range(lLayeredTextureCount):
            lLayeredTexture = pProperty.GetSrcObject(FbxLayeredTexture.ClassId, j)
            lNbTextures = lLayeredTexture.GetSrcObjectCount(FbxTexture.ClassId)
            lTextureName = " Texture "
            logger.AddLine("Num textures: %d in texture layer %d" % (lNbTextures, j))
    
            for k in range(lNbTextures):
                lTexture = lLayeredTexture.GetSrcObject(FbxTexture.ClassId,k)
                if lTexture:
                     
                    lTextureName += "\""
                    lTextureName += lTexture.GetName()
                    lTextureName += "\""
                    lTextureName += " "
                    lBlendMode = lLayeredTexture.GetTextureBlendMode(k)
                    textureDict = DisplayTextureInfo(lTexture, lBlendMode)
                    textureDict['property'] = pProperty.GetName().Buffer()[:]
                    textureDict['layer'] = j
                    textures.append(textureDict)
                    
            lTextureName += "of "
            lTextureName += pProperty.GetName().Buffer()
            lTextureName += " on layer "
            lTextureName += str(j)
            
        lTextureName += " |"
    else:
        #no layered texture simply get on the property
        logger.AddLine("Looking at non-layered textures")
    
        lNbTextures = pProperty.GetSrcObjectCount(FbxTexture.ClassId)
        logger.AddLine("Num textures: %d" % lNbTextures)
    
        if lNbTextures > 0:
            lTextureName = " Texture "
            lTextureName += " "

            for j in range(lNbTextures):
                lTexture = pProperty.GetSrcObject(FbxTexture.ClassId,j)
                if lTexture:
                    lTextureName += "\""
                    lTextureName += lTexture.GetName()
                    lTextureName += "\""
                    lTextureName += " "
                    textureDict = DisplayTextureInfo(lTexture, -1)
                    textureDict['property'] = pProperty.GetName().Buffer()[:]
                    textures.append(textureDict)
                    
            lTextureName += "of "
            lTextureName += pProperty.GetName().Buffer()
            lTextureName += " |"
    logger.EndScope(lTextureName)
    return lTextureName

def DisplayMaterialTextureConnections(pMaterial, pMatId, l, materials, logger ):
    logger.StartScope("      DisplayMaterialTextureConnections entry... looking at Material " + str(pMatId) + " (on layer " + str(l) + ")", True)
    lConnectionString = "            Material " + str(pMatId) + " (on layer " + str(l) +") -- "
    #Show all the textures
    matDict = materials[pMatId]
    
    #Diffuse Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sDiffuse)
    matDict['diffuse_textures'] = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['diffuse_textures'], logger)

    #DiffuseFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sDiffuseFactor)
    matDict['diffusefactor_textures'] = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['diffusefactor_textures'], logger)

    #Emissive Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sEmissive)
    matDict['emissive_textures'] = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['emissive_textures'], logger)

    #EmissiveFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sEmissiveFactor)
    matDict['emissivefactor_textures'] = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['emissivefactor_textures'], logger)


    #Ambient Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sAmbient)
    matDict['ambient_textures'] = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['ambient_textures'], logger)

    #AmbientFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sAmbientFactor)
    matDict['ambientfactor_textures'] = []
   
    lConnectionString += DisplayTextureNames(lProperty, matDict['ambientfactor_textures'], logger )     

    #Specular Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sSpecular)
    matDict['specular_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['specular_textures'], logger)

    #SpecularFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sSpecularFactor)
    matDict['specularfactor_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['specularfactor_textures'], logger)

    #Shininess Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sShininess)
    matDict['shininess_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['shininess_textures'], logger)

    #Bump Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sBump)
    matDict['shininessfactor_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['shininessfactor_textures'], logger)

    #Normal Map Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sNormalMap)
    matDict['normal_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['normal_textures'], logger)

    #Transparent Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sTransparentColor)
    matDict['transparency_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['transparency_textures'], logger)

    #TransparencyFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sTransparencyFactor)
    matDict['transparencyfactor_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['transparencyfactor_textures'], logger)

    #Reflection Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sReflection)
    matDict['reflection_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['reflection_textures'], logger)

    #ReflectionFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sReflectionFactor)
    matDict['reflectionfactor_textures']  = []
    lConnectionString += DisplayTextureNames(lProperty, matDict['reflectionfactor_textures'], logger)

    #if(lMaterial != NULL)
    #DisplayString(lConnectionString)
    logger.EndScope()
def DisplayMaterialConnections(pMesh, materials, meshIndex, peParserContext):
    peParserContext.logger.StartScope("Step 6.%d.3 DisplayMaterialConnections entry... (will look at all subnodes of materials used by polygons)" % meshIndex, True)

    lPolygonCount = pMesh.GetPolygonCount()

    peParserContext.logger.AddLine("    Polygons Material Connections")

    #check whether the material maps with only one mesh
    peParserContext.logger.AddLine("Checking mapping modes for layer materials to mesh. If by polygon, will go through each polygon, if all same, then we can just assign material to whole mesh", True)
    lIsAllSame = True
    for l in range(pMesh.GetLayerCount()):
        lLayerMaterial = pMesh.GetLayer(l).GetMaterials()
        if lLayerMaterial:
            peParserContext.logger.AddLine("Found a material layer, mapping mode: %d" % (lLayerMaterial.GetMappingMode()))
            if lLayerMaterial.GetMappingMode() == FbxLayerElement.eByPolygon:
                peParserContext.logger.AddLine("Mapping by polygon, will have to go through all polys and assign materials that way")
                lIsAllSame = False
                break

    #For eAllSame mapping type, just out the material and texture mapping info once
    if lIsAllSame:
        for l in range(pMesh.GetLayerCount()):
            lLayerMaterial = pMesh.GetLayer(l).GetMaterials()
            if lLayerMaterial:
                if lLayerMaterial.GetMappingMode() == FbxLayerElement.eAllSame:
                    lMaterial = pMesh.GetNode().GetMaterial(lLayerMaterial.GetIndexArray().GetAt(0))    
                    lMatId = lLayerMaterial.GetIndexArray().GetAt(0)
                    if lMatId >=0:
                        peParserContext.logger.AddLine("        All polygons share the same material on layer %d" % l, True)
                        DisplayMaterialTextureConnections(lMaterial, lMatId, l, materials, peParserContext.logger)
            else:
                #layer 0 has no material
                if l == 0:
                    peParserContext.logger.AddLine("        no material applied", True)

    #For eByPolygon mapping type, just out the material and texture mapping info once
    else:
        checkedMaterials = set()
        
        for i in range(lPolygonCount): #we get to materials through poygons, making sure we only use materials that are used by polys
            #DisplayInt("        Polygon ", i)

            for l in range(pMesh.GetLayerCount()):
                lLayerMaterial = pMesh.GetLayer(l).GetMaterials()
                if lLayerMaterial:
                    lMatId = -1
                    lMaterial = pMesh.GetNode().GetMaterial(lLayerMaterial.GetIndexArray().GetAt(i))
                    lMatId = lLayerMaterial.GetIndexArray().GetAt(i)

                    if lMatId >= 0:
                        if lMatId in checkedMaterials:
                            continue
                        checkedMaterials.add(lMatId)
                        DisplayMaterialTextureConnections(lMaterial, lMatId, l, materials, peParserContext.logger)
    peParserContext.logger.EndScope("  DisplayMaterialConnections end.", True)

def DisplayMaterialMapping(pMesh, meshIndex, peParserContext):
    DisplayString("Step 6.%d.1 Creating mapping from poly index to material index" % meshIndex)
    
    lMappingTypes = [ "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" ]
    lReferenceMode = [ "Direct", "Index", "Index to Direct"]

    polyToMaterialMap = []
    lMtrlCount = 0
    lNode = None
    if pMesh:
        lNode = pMesh.GetNode()
        if lNode:
            lMtrlCount = lNode.GetMaterialCount()

    for l in range(pMesh.GetLayerCount()):
        leMat = pMesh.GetLayer(l).GetMaterials()
        if leMat:
            header = "    Material layer %d (describes mapping from polygon to material): " % l
            DisplayString(header)

            if peParserContext.logLevel > 0:
                print "      Mapping:", lMappingTypes[leMat.GetMappingMode()], "ReferenceMode:", lReferenceMode[leMat.GetReferenceMode()]

            lMaterialCount = 0

            needMaterialEntries = 1
            if leMat.GetMappingMode() == FbxLayerElement.eByPolygon:
                #this is expected, mesh has mutliple materials
                # this means that we need as many entries in this materials layer's array as there are polygons
                needMaterialEntries = pMesh.GetPolygonCount()
            elif leMat.GetMappingMode() == FbxLayerElement.eAllSame:
                #means whole mesh has one material, in whihc case we would expect one value in layer
                needMaterialEntries = 1
            else:
                print "Error: unsupported mapping mode"
                
            if leMat.GetReferenceMode() == FbxLayerElement.eDirect:
                #means that values are stored in direct array indexed by index of (polygon or whatever else based on mapping mode)
                pass
            if leMat.GetReferenceMode() == FbxLayerElement.eIndexToDirect or \
                leMat.GetReferenceMode() == FbxLayerElement.eIndex: #legacy enum entry, same as index to direct
                #means that values are stored in direct array but are refernced by index stored in index array. the index array in turn is indexed by index of (polygon or whatever else based on mapping mode)
                pass
            
            if leMat.GetReferenceMode() == FbxLayerElement.eDirect or \
                leMat.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                lMaterialCount = lMtrlCount
            
            if leMat.GetReferenceMode() == FbxLayerElement.eIndex or \
                leMat.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                #lString = "     Indices: "

                lIndexArrayCount = leMat.GetIndexArray().GetCount()
                print "      Material Index array size:", lIndexArrayCount, "Expected based on mapping mode:", needMaterialEntries 
                
                if leMat.GetMappingMode() == FbxLayerElement.eByPolygon:
                    for i in range(lIndexArrayCount):
                        #lString += str(leMat.GetIndexArray().GetAt(i))

                        #if i < lIndexArrayCount - 1:
                        #    lString += ", "
                        polyToMaterialMap.append(leMat.GetIndexArray().GetAt(i))
                else:
                    polyToMaterialMap = [leMat.GetIndexArray().GetAt(0) for i in range(pMesh.GetPolygonCount())]
                
                #DisplayString(lString)
    return polyToMaterialMap

