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

from FbxCommon import *
from fbx import FbxNode

def DisplayPivotsAndLimits(pNode, peParserCtx):
    # Pivots
    peParserCtx.logger.AddLine("    Pivot Information", False)

    lPivotState = pNode.GetPivotState(FbxNode.eSourcePivot)
    if lPivotState == FbxNode.ePivotActive:
        peParserCtx.logger.AddLine("        Pivot State: Active", False)
    else:
        peParserCtx.logger.AddLine("        Pivot State: Reference", False)

    lTmpVector = pNode.GetPreRotation(FbxNode.eSourcePivot)
    peParserCtx.logger.AddLine("        Pre-Rotation: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]), False)

    lTmpVector = pNode.GetPostRotation(FbxNode.eSourcePivot)
    peParserCtx.logger.AddLine("        Post-Rotation: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]), False)

    lTmpVector = pNode.GetRotationPivot(FbxNode.eSourcePivot)
    peParserCtx.logger.AddLine("        Rotation Pivot: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]), False)

    lTmpVector = pNode.GetRotationOffset(FbxNode.eSourcePivot)
    peParserCtx.logger.AddLine("        Rotation Offset: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]), False)

    lTmpVector = pNode.GetScalingPivot(FbxNode.eSourcePivot)
    peParserCtx.logger.AddLine("        Scaling Pivot: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]), False)

    lTmpVector = pNode.GetScalingOffset(FbxNode.eSourcePivot)
    peParserCtx.logger.AddLine("        Scaling Offset: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]), False)

    peParserCtx.logger.AddLine("    Limits Information", False)

    lIsActive = pNode.TranslationActive
    lMinXActive = pNode.TranslationMinX
    lMinYActive = pNode.TranslationMinY
    lMinZActive = pNode.TranslationMinZ
    lMaxXActive = pNode.TranslationMaxX
    lMaxYActive = pNode.TranslationMaxY
    lMaxZActive = pNode.TranslationMaxZ
    lMinValues = pNode.TranslationMin
    lMaxValues = pNode.TranslationMax

    peParserCtx.logger.StartScope("Limits Information")
    
    if lIsActive:
        peParserCtx.logger.AddLine("        Translation limits: Active", False)
    else:
        peParserCtx.logger.AddLine("        Translation limits: Inactive", False)
    peParserCtx.logger.AddLine("            X", False)
    if lMinXActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f" % lMinValues.Get()[0], False)
    if lMaxXActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[0], False)
    
    peParserCtx.logger.AddLine("            Y", False)
    if lMinYActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f" % lMinValues.Get()[1], False)
    if lMaxYActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[1], False)
    
    peParserCtx.logger.AddLine("            Z", False)
    if lMinZActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f"% lMinValues.Get()[2], False)
    if lMaxZActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[2], False)

    lIsActive = pNode.RotationActive
    lMinXActive = pNode.RotationMinX
    lMinYActive = pNode.RotationMinY
    lMinZActive = pNode.RotationMinZ
    lMaxXActive = pNode.RotationMaxX
    lMaxYActive = pNode.RotationMaxY
    lMaxZActive = pNode.RotationMaxZ
    lMinValues = pNode.RotationMin
    lMaxValues = pNode.RotationMax

    if lIsActive:
        peParserCtx.logger.AddLine("        Rotation limits: Active", False)
    else:
        peParserCtx.logger.AddLine("        Rotation limits: Inactive", False)    
    peParserCtx.logger.AddLine("            X", False)
    if lMinXActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f" % lMinValues.Get()[0], False)
    if lMaxXActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[0], False)
    
    peParserCtx.logger.AddLine("            Y", False)
    if lMinYActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f" % lMinValues.Get()[1], False)
    if lMaxYActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[1], False)
    
    peParserCtx.logger.AddLine("            Z", False)
    if lMinZActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f"% lMinValues.Get()[2], False)
    if lMaxZActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[2], False)

    lIsActive = pNode.ScalingActive
    lMinXActive = pNode.ScalingMinX
    lMinYActive = pNode.ScalingMinY
    lMinZActive = pNode.ScalingMinZ
    lMaxXActive = pNode.ScalingMaxX
    lMaxYActive = pNode.ScalingMaxY
    lMaxZActive = pNode.ScalingMaxZ
    lMinValues = pNode.ScalingMin
    lMaxValues = pNode.ScalingMax

    if lIsActive:
        peParserCtx.logger.AddLine("        Scaling limits: Active", False)
    else:
        peParserCtx.logger.AddLine("        Scaling limits: Inactive", False)    
    peParserCtx.logger.AddLine("            X", False)
    if lMinXActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f" % lMinValues.Get()[0], False)
    if lMaxXActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[0], False)
    
    peParserCtx.logger.AddLine("            Y", False)
    if lMinYActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f" % lMinValues.Get()[1], False)
    if lMaxYActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[1], False)
    
    peParserCtx.logger.AddLine("            Z", False)
    if lMinZActive:
        peParserCtx.logger.AddLine("                Min Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Min Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Min Limit Value: %f"% lMinValues.Get()[2], False)
    if lMaxZActive:
        peParserCtx.logger.AddLine("                Max Limit: Active", False)
    else:
        peParserCtx.logger.AddLine("                Max Limit: Inactive", False)
    peParserCtx.logger.AddLine("                Max Limit Value: %f" % lMaxValues.Get()[2], False)

    peParserCtx.logger.EndScope()