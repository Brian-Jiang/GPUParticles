"""
This code has been modified by Artem Kovalovs, for PrimeEngine

for maya command docs refer to:
http://download.autodesk.com/us/maya/2009help/CommandsPython/cat_Windows.html

cvxporter

Exports .x files from Maya from versions 8.5+.

Author: Chad Vernon - www.chadvernon.com - chadvernon@gmail.com

Copyright (c) 2008 Chad Vernon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

"""
import maya.cmds as cmds
import maya.mel as mel
import maya.OpenMaya as OpenMaya
import maya.OpenMayaAnim as OpenMayaAnim
import ctypes
import os
import sys
import shutil
from math import fabs
import xparser
import subprocess
import threading
sys.path.append(os.path.normpath(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "Tools")))
import PyClient
import PyClient.assetmanagercommon
import PyClient.pyclientcommon

# scriptTable command calls MEL procedures internally so we have to specify these in MEL
mel.eval( 'global proc string getCellMel( int $row, int $column ) { return python( "cvxporter.getCell( " + $row + ", " + $column + ")" ); }' )
mel.eval( 'global proc cellChangedMel( int $row, int $column, string $value ) { python( "cvxporter.cellChanged( " + $row + ", " + $column + ", \\\"" + $value + "\\\" )" ); }' )

sets = [["","",""]]
exportedText = ""

#startDirectory = cmds.workspace( query=True, rootDirectory=True )
startDirectory = os.environ["PYENGINE_WORKSPACE_DIR"] + "\\\\AssetsIn\\\\XFiles\\\\"
profilesDirectory = os.environ["PYENGINE_WORKSPACE_DIR"] + "\\\\AssetsIn\\\\ExportProfiles\\\\"
execDir = os.environ["PYENGINE_WORKSPACE_DIR"] + "\\\\Debug_DX11"
execName = "Arkane.exe"
execLaunchDir = os.environ["PYENGINE_WORKSPACE_DIR"] + "\\\\Arkane"
matUICallBackDict = None
def Materials_Continue(args):
    print 'Materials_Continue'
    
    matsToExport = matUICallBackDict['args']['matsToExport']
    matCheckBoxes = matUICallBackDict['args']['matCheckBoxes']
    
    l = len(matsToExport)
    for i in xrange(l):
        matname = matsToExport[i]
        chkBx = matCheckBoxes[i]
        
        exportThisMat = cmds.checkBox( chkBx, query=True, value=True )
        if not exportThisMat: matsToExport[i] = None
    
    meshesToExport = matUICallBackDict['args']['meshesToExport']
    meshCheckBoxes = matUICallBackDict['args']['meshCheckBoxes']
    meshDestinations = matUICallBackDict['args']['meshDestinations']
    
    l = len(meshesToExport)
    for i in xrange(l):
        matname = meshesToExport[i]
        chkBx = meshCheckBoxes[i]
        
        exportThisMesh = cmds.checkBox( chkBx, query=True, value=True )
        if not exportThisMesh: meshesToExport[i] = None
    
    skeletonsToExport = matUICallBackDict['args']['skeletonsToExport']
    skeletonCheckBoxes = matUICallBackDict['args']['skeletonCheckBoxes']
    
    l = len(skeletonsToExport)
    for i in xrange(l):
        skelname = skeletonsToExport[i]
        chkBx = skeletonCheckBoxes[i]
        
        exportThisSkel = cmds.checkBox( chkBx, query=True, value=True )
        if not exportThisSkel: skeletonsToExport[i] = None
    
    matUICallBackDict['call'](matUICallBackDict['args'])
    
    cmds.deleteUI( "pyengineMatSelectorUI", window=True )
def Materials(cbDict):
    if cmds.window( "pyengineMatSelectorUI", exists=True ):
        cmds.deleteUI( "pyengineMatSelectorUI", window=True )
    if cmds.windowPref( "pyengineMatSelectorUI", exists=True ):
        cmds.windowPref( "pyengineMatSelectorUI", remove=True )

    global matUICallBackDict
    matUICallBackDict = cbDict
    matsToExport = cbDict['args']['matsToExport']
    matCheckBoxes = cbDict['args']['matCheckBoxes']
    matDestinations = cbDict['args']['destinations']
    
    
    meshesToExport = cbDict['args']['meshesToExport']
    meshCheckBoxes = cbDict['args']['meshCheckBoxes']
    meshDestinations = cbDict['args']['meshDestinations']
    
    skeletonsToExport = cbDict['args']['skeletonsToExport']
    skeletonCheckBoxes = cbDict['args']['skeletonCheckBoxes']
    skeletonDestinations = cbDict['args']['skeletonDestinations']
    
    skinsToExport = cbDict['args']['skinsToExport']
    skinDestinations = cbDict['args']['skinDestinations']
    skinCheckBoxes = cbDict['args']['skinCheckBoxes']
    
    animNames = cbDict['args']['animNames']
    
    isMaya2011orNewer = True
    if (os.environ["MAYA_DIR"].lower().find("maya2009") != -1) or (os.environ["MAYA_DIR"].lower().find("maya2010") != -1):
        #Maya 2009 or 2010
        isMaya2011orNewer = False
    
    
    wwidth = 640
    maxWHeight = 400
    wheight = maxWHeight #min(112 + 42 * len(matsToExport), maxWHeight)

    window = cmds.window( "pyengineMatSelectorUI", title="PrimeEngine Material Selector", widthHeight=(wwidth, wheight) , topLeftCorner=[100, 100])
    
    scroll = cmds.scrollLayout("mainScroll", height = 300)#height = 470  )
    
    cmds.columnLayout( adjustableColumn=False, columnOffset=("both", 4), height = 50 )
    cmds.iconTextStaticLabel(style="textOnly", align="left", label = "PrimeEngine Exporter has detected these assets:",height = 20, width = wwidth)
    cmds.iconTextStaticLabel(style="textOnly", align="left", label = "Do you want all of them exported? Please check", height=20,width = wwidth)
    if isMaya2011orNewer: #Maya 2009&2010 UI is a little buggy and will not work with this call
        cmds.setParent( ".." )

    cmds.columnLayout( adjustableColumn=False, columnOffset=("both", 4) , height = 20)
    
    cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
    cmds.text( 'name_label_def_', label="", width = 20)
    cmds.text( 'name_label_def', label=" Material in Maya", align = "left", width = 200)
    cmds.text( 'name_dest_label_def', label="Destination PrimeEngine Material", align="left", width=300)
    cmds.setParent( ".." )
    cmds.setParent( ".." )
    i = 0
    for matname in matsToExport:
        print 'mat: ', matname
        cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
        matCheckBoxes.append(cmds.checkBox( matname, label='', value=True, enable = True, align="left", width = 20 ))
        
        cmds.textField( 'name_label_' + matname, text=matname, editable=False, width = 200)
        
        cmds.textField( 'name_dest_label_' + matname, editable=False, width=300, insertionPosition=0, text=matDestinations[i])
        
        cmds.setFocus('name_dest_label_' + matname)
        cmds.setParent( ".." )
        i += 1
    #cmds.setParent( ".." )
    
    cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
    cmds.text( 'mesh_name_label_def_', label="", align = "left", width = 20)
    cmds.text( 'mesh_name_label_def', label=" Mesh node in Maya", align = "left", width = 200)
    cmds.text( 'mes_name_dest_label_def', label="Destination PrimeEngine Mesh", align = "left", width=300)
    cmds.setParent( ".." )
    i = 0
    for meshname in meshesToExport:
        print 'mesh: ', meshname
        cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
        meshCheckBoxes.append(cmds.checkBox( meshname, label='', value=True, enable = True, align="left", width = 20 ))
        
        cmds.textField( 'mesh_name_label_' + meshname, text=meshname, editable=False, width = 200)
        
        cmds.textField( 'mesh_name_dest_label_' + meshname, editable=False, width=300, insertionPosition=0, text=meshDestinations[i])
        
        cmds.setFocus('mesh_name_dest_label_' + meshname)
        cmds.setParent( ".." )
        i += 1
    
    cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
    cmds.text( 'skin_name_label_def_', label="", align = "left",width = 20)
    cmds.text( 'skin_name_label_def', label=" Skin node in Maya", align = "left",width = 200)
    cmds.text( 'skin_name_dest_label_def', label="Destination PrimeEngine Skin", align = "left",width=300)
    cmds.setParent( ".." )
    i = 0
    for skinname in skinsToExport:
        print 'skin: ', skinname
        cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
        skinCheckBoxes.append(cmds.checkBox( skinname, label='', value=True, enable = True, align="left", width = 20 ))
        
        cmds.textField( 'skin_name_label_' + skinname, text=skinname, editable=False, width = 200)
        
        cmds.textField( 'skin_name_dest_label_' + skinname, editable=False, width=300, insertionPosition=0, text=skinDestinations[i])
        
        cmds.setFocus('skin_name_dest_label_' + skinname)
        cmds.setParent( ".." )
        i += 1
    
    
    # skeletons
    cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
    cmds.text( 'skel_name_label_def_', label="", align = "left",width = 20)
    cmds.text( 'skel_name_label_def', label=" Skel Root Joint node in Maya", align = "left",width = 200)
    cmds.text( 'skel_name_dest_label_def', label="Destination PrimeEngine Skeleton", align = "left", width=300)
    cmds.setParent( ".." )
    iskel = 0
    
    for skel in skeletonsToExport:
        print 'skel: ', skel
        cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
        skeletonCheckBoxes.append(cmds.checkBox( skel['name']+'checkbox', label='', value=True, enable = False, align="left", width = 20 ))
        cmds.textField( 'skel_name_label_' + skel['name'], text=skel['name'], editable=False, width = 200)
        cmds.textField( 'skel_name_dest_label_' + skel['name'], editable=False, width=300, insertionPosition=0, text = skeletonDestinations[iskel])
        cmds.setFocus('skel_name_dest_label_' + skel['name'])
        cmds.setParent( ".." )
        #skel is a hierarchy distionary
        JointUI(skel)
        iskel += 1
    
    #cmds.setParent( ".." )
    
    cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
    cmds.text( 'anims_name_label_def_', label="", align = "left", width = 20)
    cmds.text( 'anims_name_label_def', label=" Animations", align = "left", width = 200)
    cmds.text( 'anims_name_dest_label_def', label="", align = "left", width=300)
    cmds.setParent( ".." )
    i = 0
    for animName in animNames:
        cmds.rowLayout(numberOfColumns=3, columnWidth=[(1,20), (2, 210), (3, 300)], columnAlign2=('left', 'left'))
        skinCheckBoxes.append(cmds.checkBox( animName, label='', value=True, enable = False, align="left", width = 20 ))
        
        cmds.textField( 'skin_name_label_' + animName, text=animName, editable=False, width = 200)
        cmds.textField( 'skin_name_label__' + animName, text="", editable=False, width = 200)
        
        cmds.setFocus('skin_name_label_' + animName)
        cmds.setParent( ".." )
        i += 1
    
    cmds.columnLayout( adjustableColumn=False, columnOffset=("both", 4), height = 20 )
    cmds.button( "Continue_matselector", label="Continue", command=Materials_Continue, align="center", width = 100 )
    
    cmds.showWindow( "pyengineMatSelectorUI" )
def JointUI(joint, i = 0, offset = 0):
    #joint is a hierarchy distionary
    columnWidthT=[]
    for plusIndex in xrange(offset + 1):
        columnWidthT.append((plusIndex+1, 20))
    columnWidthT.append((offset + 2, 210))
    columnWidthT.append((offset + 3, 300))
    
    cmds.rowLayout(numberOfColumns=3 + offset, columnWidth=columnWidthT, columnAlign2=('left', 'left'))
    for plusIndex in xrange(offset + 1):
        icon = str(i)+':'
        if plusIndex > 0:
            if plusIndex == offset:
                icon = '+'
            else:
                icon =  ' |'
        
        cmds.text( str(plusIndex) + joint['name'] + '_joint_name_label_def_', label = icon, align = "left", width = 20)
    cmds.text( joint['name'] + '_joint_name_label_def', label=joint['name'], align = "left", width = 200)
    cmds.text( joint['name'] + '_joint_name_dest_label_def', label="", align = "left", width=300)
    cmds.setParent( ".." )
    
    for c in joint['children']:
        i += 1
        i = JointUI(c, i, offset+1)
    return i
def UI():
    """
    Creates the interface for cvxporter.
    """

    if cmds.window( "cvxporterUI", exists=True ):
        cmds.deleteUI( "cvxporterUI", window=True )
    if cmds.windowPref( "cvxporterUI", exists=True ):
        cmds.windowPref( "cvxporterUI", remove=True )

    window = cmds.window( "cvxporterUI", title="PrimeEngine Exporter", widthHeight=(550, 470) )
    cmds.columnLayout( adjustableColumn=True, columnOffset=("both", 4) )
    cmds.button( "exportAllButton", label="Export All", command=exportAll )
    cmds.button( "exportSelectedButton", label="Export Selected", command=exportSelected )
    cmds.button( "exportLevelButton", label="Export Level", command=exportLevel )
    cmds.button( "saveProfileButton", label="Save Export Profile", command=saveProfile )
    cmds.button( "loadProfileButton", label="Load Export Profile", command=loadProfile )
    cmds.text( label="" )
    cmds.rowColumnLayout( numberOfColumns=2, columnWidth=[(1, 250), (2,300)], columnOffset=(2, "left", 15) )
    cmds.columnLayout()
    cmds.text( 'label_export_select_package', label="Select Destination Package", width = 200)
    
    cmds.textScrollList( 'export_packages', width = 250, height = 75, 
        append = PyClient.assetmanagercommon.getListOfAvailablePackages() + ['New Package...'], 
        selectCommand = SelectExportPackage)
    
    cmds.textFieldGrp( "targetPackage", label="Destination Package:", text="Default", enable = False, columnWidth=[(1, 115), (2, 135)], columnAlign=(1, "left") )
    cmds.checkBox( "adjacencies", label="Adjacencies", value=False, enable = False )
    cmds.checkBox( "materials", label="Materials", value=True )
    cmds.checkBox( "normals", label="Normals", value=True )
    cmds.checkBox( "skinning", label="Skinning", value=True )
    cmds.checkBox( "uvs", label="UVs", value=True )
    cmds.checkBox( "tangents", label="Tangents", value=True )
    cmds.checkBox( "binormals", label="Binormals", value=False, enable = False )
    cmds.setParent( ".." )
    cmds.columnLayout()
    cmds.checkBox( "useDDS", label="Convert textures to DDS (requires DX SDK)" )
    cmds.text( label='Conversion runs cmd: dxtex "inputFile" -m "outputFile"' )
    cmds.checkBox( "absolutePath", label="Absolute Texture Path", onCommand=enableTextureText, offCommand=enableTextureText )
    cmds.checkBox( "copyTexture", label="Copy texture to relative directory" )
    cmds.textFieldGrp( "texturePath", label="Texture Path:", text="./", columnWidth=[(1, 75), (2, 150)], columnAlign=(1, "left") )
    cmds.checkBox( "vertexColors", label="Vertex Colors", value=False, enable=False, onCommand=enableVertexColors, offCommand=enableVertexColors )
    cmds.radioButtonGrp( "vertexColor", label="Colorless vertices:", numberOfRadioButtons=2, select=1, enable=False, label1="White", label2="Black", columnAlign3=("left", "left", "left"), columnWidth3=(95, 55, 55) )
    cmds.setParent( ".." )
    cmds.setParent( ".." )
    cmds.text( label="" )
    cmds.rowColumnLayout( numberOfColumns=4 )
    cmds.checkBox( "animation", label="Animation", value=False, onCommand=enableAnimation, offCommand=enableAnimation )
    cmds.button( "addSetButton", label="Add Set", command=addSet, enable=False )
    cmds.button( "removeSetButton", label="Remove Set", command=removeSet, enable=False )
    cmds.button( "clearSetsButton", label="Clear Sets", command=clearSets, enable=False )
    cmds.setParent( ".." )
    cmds.radioButtonGrp( "animationFormat1", label="Animation format:", edit=False, numberOfRadioButtons=2, select=2, label1="SRT", label2="Matrix", columnAlign3=("left", "left", "left"), columnWidth3=(90, 55, 55), enable=False )
    
    if os.environ["MAYA_DIR"].lower().find("maya2009") != -1:
        #Maya 2009
        cmds.scriptTable( "animationSets", rows=1, columns=3, label=[(1, "Name"), (2, "Start Frame"), (3, "End Frame")], enable=False, width=400, height=200, columnWidth=[(1,180),(2,70),(3,70)], getCellCmd="getCellMel", cellChangedCmd="cellChangedMel" )
    else:
        #Maya 2011
        cmds.scriptTable( "animationSets", rows=1, columns=3, label=[(1, "Name"), (2, "Start Frame"), (3, "End Frame")], enable=False, width=400, height=200, columnWidth=[(1,180),(2,70),(3,70)], cellChangedCmd=cellChanged, getCellCmd=getCell)
    cmds.text( label="" )
    cmds.showWindow( "cvxporterUI" )
def SelectExportPackage():
    s = cmds.textScrollList('export_packages', query = True, selectItem = True)
    print " Selected Package: ", s
    
    if s[0] == u'New Package...':
        cmds.textFieldGrp( "targetPackage", edit = True, label="New Package:", enable = True, text='EnterNameHere')
    else:
        cmds.textFieldGrp( "targetPackage", edit = True, label="Destination Package:", enable = False, text=s[0])
    
    
def tmpCellChanged(row, column, value):
    print "tmpCellChanged:", row, column, value

def enableTextureText( value ):
    """
    Enables and disables the texturePath textFieldGrp.
    """
    result = False
    if value == "false":
        result = True
    cmds.textFieldGrp( "texturePath", edit=True, enable=result )
    cmds.checkBox( "copyTexture", edit=True, enable=result )

def enableVertexColors( value ):
    """
    Enables and disables the vertexColor radioButtonGrp.
    """
    result = True
    if value == "false":
        result = False
    cmds.radioButtonGrp( "vertexColor", edit=True, enable=result )

def enableAnimation( value ):
    """
    Enables and disables the animation buttons and set table.
    """
    result = value != "false"
    cmds.button( "addSetButton", edit=True, enable=result )
    cmds.button( "removeSetButton", edit=True, enable=result )
    cmds.button( "clearSetsButton", edit=True, enable=result )
    cmds.scriptTable( "animationSets", 
        edit=True, 
        enable=result )
    # cmds.radioButtonGrp( "animationFormat1", 
        # edit=True, 
        # enable=result )

def addSet( arg ):
    """
    Adds a new row to the animation sets table.
    """
    global sets
    sets.append( ["","",""] )
    print "addSet(): len(sets):", len(sets)
    cmds.scriptTable( "animationSets", edit=True, insertRow=len(sets) )
    cmds.scriptTable( "animationSets", edit=True, clearTable=True )
    
def addSetWithValues( vals, i ):
    """
    Adds a new row to the animation sets table.
    """
    global sets
    sets.append( vals)
    cmds.scriptTable( "animationSets", edit=True, insertRow=i )
    cmds.scriptTable( "animationSets", edit=True, clearTable=True )

def removeSet( arg ):
    """
    Removes the selected row from the animation sets table.
    """
    global sets
    if len( sets ) == 1:
        # Leave a minimum of 1 row
        clearSets( None )
        return None
    selectedRow = cmds.scriptTable( "animationSets", query=True, selectedRow=True )
    if selectedRow:
        sets.pop( selectedRow - 1 )
        cmds.scriptTable( "animationSets", edit=True, deleteRow=selectedRow )
        rows = cmds.scriptTable( "animationSets", query=True, rows=True )
        cmds.scriptTable( "animationSets", edit=True, clearTable=True )

def clearSets( arg ):
    """
    Clears all sets from the animation sets table.
    """
    global sets
    sets = [["","",""]]
    cmds.scriptTable( "animationSets", edit=True, rows=1 )
    cmds.scriptTable( "animationSets", edit=True, clearTable=True )

def clearSetsComplete( arg ):
    """
    Clears all sets from the animation sets table.
    """
    global sets
    sets = []
    cmds.scriptTable( "animationSets", edit=True, rows=0 )
    cmds.scriptTable( "animationSets", edit=True, clearTable=True )

def getCell( row, column ):
    """
    Returns the value for the given cell.
    """
    global sets
    try:
        result = sets[row - 1][column - 1]
        return result
    except:
        return ""

def cellChanged( row, column, value ):
    """
    Stores the new value for the given cell.
    """
    global sets
    sets[row - 1][column - 1] = value
    cmds.scriptTable( "animationSets", edit=True, clearRow=row )
    print "cellChanged( row = %d column = %d value = %s): len(sets): %d" % (int(row), int(column), value, len(sets))
    

def exportAll( arg ):
    """
    Called by the UI to export all objects.
    """
    doExport( True )

def exportSelected( arg ):
    """
    Called by the UI to export selected objects.
    """
    doExport( False )

def exportLevel( arg ):
    """
    Called by the UI to export all objects as level.
    """
    doExport( True, True )
def saveProfile( arg ):
    """
    Called by the UI to save export profile.
    """
    mask = profilesDirectory + '*.profile'
    filename = cmds.fileDialog( mode=1, directoryMask=mask )
    if not filename.endswith('.profile'):
        filename = filename + '.profile'
    global sets
    f = open(filename, 'w')
    f.write('d={\n')
    f.write("'sets':[\n")
    for set in sets:
        f.write('["%s","%s","%s"],\n' % (set[0], set[1], set[2]))
    f.write('],\n')
    f.write('}')
    f.close()
def loadProfile( arg ):
    """
    Called by the UI to load export profile.
    """
    #doExport( False )
    mask = profilesDirectory + '*.profile'
    filename = cmds.fileDialog( mode=0, directoryMask=mask )
    if not filename.endswith('.profile'):
        filename = filename + '.profile'
    clearSetsComplete({})
    g = {}
    l = {}
    execfile(filename, g, l)
    print 'l:', l
    d = l.get('d', {'sets':[]})
    for s in d['sets']:
        addSetWithValues(s, 1)
def doExport( all, isLevel = False ):
    """
    Gathers the export options and exports the x file.
    """
    global sets
    adjacencies = cmds.checkBox( "adjacencies", query=True, value=True )
    materials = cmds.checkBox( "materials", query=True, value=True )
    normals = cmds.checkBox( "normals", query=True, value=True )
    skinning = cmds.checkBox( "skinning", query=True, value=True )
    uvs = cmds.checkBox( "uvs", query=True, value=True )
    tangents = cmds.checkBox( "tangents", query=True, value=True )
    binormals = cmds.checkBox( "binormals", query=True, value=True )
    targetPackage = cmds.textFieldGrp( "targetPackage", query=True, text=True )
    useDDS = cmds.checkBox( "useDDS", query=True, value=True )
    absolutePath = cmds.checkBox( "absolutePath", query=True, value=True )
    copyTexture = cmds.checkBox( "copyTexture", query=True, value=True )
    texturePath = cmds.textFieldGrp( "texturePath", query=True, text=True )
    vertexColors = cmds.checkBox( "vertexColors", query=True, value=True )
    vertexColor = cmds.radioButtonGrp( "vertexColor", query=True, select=True )
    animation = cmds.checkBox( "animation", query=True, value=True )
    animationFormat = cmds.radioButtonGrp( "animationFormat1", query=True, select=True )
    #print 'animationFormat', animationFormat
    #raw_input()
    animationSets = sets

    if not texturePath.endswith( '/' ):
        texturePath += '/'
    mask = startDirectory + '*.x'
    fileName = cmds.fileDialog( mode=1, directoryMask=mask )
    if fileName == '':
        return None
    if not fileName.endswith( '.x' ):
        fileName += '.x'

    args = { 
        "level" : isLevel, 
        "adjacencies" : adjacencies, "materials" : materials, "normals" :normals,
        "skinning" : skinning, "uvs" : uvs, "tangents" : tangents,
        "binormals" : binormals, "useDDS" : useDDS, "absolutePath" : absolutePath,
        "targetPackage" : targetPackage,
        "copyTexture" : copyTexture, "texturePath" : texturePath, "vertexColors" : vertexColors, 
        "vertexColor" : vertexColor, "animation" : animation, "animationSets" : animationSets,
        "fileName" : fileName, "animationFormat" : animationFormat 
    }

    exporter = XExporter( args )
    filenames = exporter.export( all )
    
class XMesh:
    """
    Holds all the information for a single mesh.
    """
    def __init__( self ):
        self.name = ''
        self.vertices = []
        self.verticesPerFace = OpenMaya.MIntArray()
        self.faceVertexIndices = []
        self.vertsForAdjacency = []
        self.normals = OpenMaya.MFloatVectorArray()
        self.normalsPerFace = OpenMaya.MIntArray()
        self.normalIndices = OpenMaya.MIntArray()
        self.uArray = []
        self.vArray = []
        self.hasUVs = True
        self.vertexColors = []
        self.materials = []
        self.faceMaterialIndices = OpenMaya.MIntArray()
        self.tangents = OpenMaya.MFloatVectorArray()
        self.binormals = OpenMaya.MFloatVectorArray()
        self.fnMesh = OpenMaya.MFnMesh()
        self.weights = []

class XJoint:
    """
    Holds the information for a skinned joint.
    """
    def __init__( self, name='', worldBindPoseMatrix=OpenMaya.MMatrix.identity ):
        self.name = name
        self.worldBindPoseMatrix = worldBindPoseMatrix

class XMaterial:
    """
    Holds the information for a material.
    """
    def __init__( self, color=[1.0, 1.0, 1.0], alpha=1.0, specular=[0.0, 0.0, 0.0], emissive=[0.0, 0.0, 0.0], diffuseCoefficient=1.0, power=0.0, texture='' ):
        self.color = color
        self.alpha = alpha
        self.specular = specular
        self.emissive = emissive
        self.diffuseCoefficient = diffuseCoefficient
        self.power = power
        self.texture = texture

class XAnimationSet:
    """
    Holds the information for an animation set.
    """
    def __init__( self, name, start, end ):
        self.name = name
        self.start = start
        self.end = end
        self.animations = []

class XAnimation:
    """
    Holds the animation data for an animation set.
    """
    def __init__( self, name, rotations, scales, translations, transforms ):
        self.frame = name
        self.rotations = rotations
        self.scales = scales
        self.translations = translations
        self.transforms = transforms


class XExporter:
    """
    Handles all the file exporting and data gathering.
    """
    def __init__( self, args ):
        self.indents = 0
        self.args = args
        self.nodesOutput = []
        self.animationSets = []
        self.joints = []
        for set in self.args['animationSets']:
            if set[0] and set[1] and set[2]:
                self.animationSets.append( XAnimationSet( set[0], int( set[1] ), int( set[2] ) ) )

    def export( self, all ):
        """
        Begins the export process.
        """
        # Autokeyframe causes crash
        keyMode = cmds.autoKeyframe( query=True, state=True )
        cmds.autoKeyframe( state=False )

        # Open file
        self.xfile = open( self.args['fileName'], 'w' )

        # Write header
        self.write( 'xof 0303txt 0032\n\n' )

        # Open progress window
        cmds.progressWindow( endProgress=True )
        cmds.progressWindow( title="Exporting File", progress=0, status="" )

        self.write( 'LevelProperties {\n', )
        self.write('%d;\n' % self.args['level'], 1)
        self.write('}\n', -1)

        if not self.args['level'] and self.args['animation']:
            self.outputAnimTicksPerSecond()

        # Since we're using material references, output all materials first
        if not self.args['level'] and self.args['materials']:
            self.outputAllMaterials( all )

        if self.args['skinning']:
            # First build joint hierarchy
            itDag = OpenMaya.MItDag( OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kJoint )
            while not itDag.isDone():
                path = OpenMaya.MDagPath()
                itDag.getPath( path )
                fnJoint = OpenMayaAnim.MFnIkJoint( path.node() )
                self.joints.append( XJoint( name=fnJoint.partialPathName() ) )
                itDag.next()

            # Find bind poses matrices of all joints
            noSkinCluster = False
            try:
                # Iterate over all skin cluster nodes
                itDn = OpenMaya.MItDependencyNodes( OpenMaya.MFn.kSkinClusterFilter )
            except:
                # No skin cluster found
                noSkinCluster = True
            if not noSkinCluster:
                while not itDn.isDone():
                    oNode = itDn.thisNode()
                    fnSkinCluster = OpenMayaAnim.MFnSkinCluster( oNode )
                    influenceObjects = OpenMaya.MDagPathArray()
                    numInfluences = fnSkinCluster.influenceObjects( influenceObjects )
                    plugBindPreMatrixArray = fnSkinCluster.findPlug( 'bindPreMatrix' )
                    self.calculateWorldBindPoses( influenceObjects, plugBindPreMatrixArray )
                    itDn.next()
        try:
            if all:
                self.exportAll()
            else:
                self.exportSelection()
        except:
            cmds.progressWindow( endProgress=True )
            from traceback import print_tb
            print sys.exc_info()[0]
            print_tb(sys.exc_info()[2])
            return None

        if not self.args['level'] and self.args['animation']:
            self.exportAnimation()

        self.xfile.flush()
        self.xfile.close()
        cmds.autoKeyframe( state=keyMode )
        OpenMaya.MGlobal.displayInfo( 'File exported successfully!' )
        filenames = xparser.parseXFile(self.args['fileName'], self.args['targetPackage'], {'materialUI' : Materials,})
        print 'Filenames returned by XParser:', filenames
        return filenames

    def write( self, string, indentDelta=0, indents=True ):
        """
        Writes text to the file with proper indentation.
        """
        if indents:
            self.indents += indentDelta
            for i in range( self.indents ):
                self.xfile.write( '\t' )
        self.xfile.write( string )

    def outputAnimTicksPerSecond( self ):
        """
        Outputs the AnimTicksPerSecond template.
        """
        fps = 0
        timeUnit = OpenMaya.MTime.uiUnit()
        if timeUnit == OpenMaya.MTime.kFilm:
            fps = 24
        elif timeUnit == OpenMaya.MTime.kGames:
            fps = 15
        elif timeUnit == OpenMaya.MTime.kPALFrame:
            fps = 25
        else:
            fps = 30
        self.write( 'AnimTicksPerSecond {\n' )
        self.write( '%d;\n' % fps, 1 )
        self.write( '}\n', -1 )

    def outputAllMaterials( self, exportAll ):
        originalSelection = cmds.ls( sl=True )

        # Build list of output meshes
        meshes = []
        if exportAll:
            # Check all meshes
            itDag = OpenMaya.MItDag( OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kTransform )
            while not itDag.isDone():
                path = OpenMaya.MDagPath()
                itDag.getPath( path )
                if self.isExportable( path ):
                    fnDagNode = OpenMaya.MFnDagNode( path )
                    numChildren = fnDagNode.childCount()
                    for i in range( numChildren ):
                        child = fnDagNode.child( i )
                        path.push( child )
                        if path.node().apiType() == OpenMaya.MFn.kMesh:
                            fnMesh = OpenMaya.MFnMesh( path )
                            meshes.append( fnMesh.partialPathName() )
                        path.pop()
                itDag.next()
        else:
            # Check selection
            selectionList = OpenMaya.MSelectionList()
            OpenMaya.MGlobal.getActiveSelectionList( selectionList )
            itList = OpenMaya.MItSelectionList( selectionList, OpenMaya.MFn.kTransform )
            while not itList.isDone():
                path = OpenMaya.MDagPath()
                itList.getDagPath( path )
                if self.isExportable( path ):
                    fnDagNode = OpenMaya.MFnDagNode( path )
                    numChildren = fnDagNode.childCount()
                    for i in range( numChildren ):
                        child = fnDagNode.child( i )
                        path.push( child )
                        if path.node().apiType() == OpenMaya.MFn.kMesh:
                            fnMesh = OpenMaya.MFnMesh( path )
                            meshes.append( fnMesh.partialPathName() )
                        path.pop()
                itList.next()

        # Iterate through all lambert inherited shaders
        try:
            itDN = OpenMaya.MItDependencyNodes( OpenMaya.MFn.kDependencyNode  )
            numMaterials = 0
            while not itDN.isDone():
                oNode = itDN.thisNode()
                if oNode.apiType() == OpenMaya.MFn.kPhong or oNode.apiType() == OpenMaya.MFn.kBlinn or oNode.apiType() == OpenMaya.MFn.kLambert or oNode.apiType() == OpenMaya.MFn.kSurfaceShader:
                    numMaterials += 1
                itDN.next()

            cmds.progressWindow( edit=True, progress=0, status="Writing Materials...", maxValue=numMaterials )
            itDN.reset()
            while not itDN.isDone():
                oNode = itDN.thisNode()
                material = XMaterial()
                if oNode.apiType() == OpenMaya.MFn.kPhong or oNode.apiType() == OpenMaya.MFn.kBlinn or oNode.apiType() == OpenMaya.MFn.kLambert or oNode.apiType() == OpenMaya.MFn.kSurfaceShader:
                    if oNode.apiType() == OpenMaya.MFn.kPhong:
                        shader = OpenMaya.MFnPhongShader( oNode )
                    elif oNode.apiType() == OpenMaya.MFn.kBlinn:
                        shader = OpenMaya.MFnBlinnShader( oNode )
                    elif oNode.apiType() == OpenMaya.MFn.kSurfaceShader:
                        shader = OpenMaya.MFnDependencyNode( oNode )
                    else:
                        shader = OpenMaya.MFnLambertShader( oNode )


                    if oNode.apiType() != OpenMaya.MFn.kSurfaceShader:
                        color = [shader.color().r, shader.color().g, shader.color().b]
                        material.color = map( lambda x: x * shader.diffuseCoeff(), color )
                        material.alpha = 1.0 - shader.transparency().r
                        material.diffuseCoefficient = shader.diffuseCoeff()
                        #TODO: ad support for other shader types
                        if oNode.apiType() == OpenMaya.MFn.kLambert:
                            material.specular = [0.0, 0.0, 0.0]
                        else:
                            spec = [shader.specularColor().r, shader.specularColor().g, shader.specularColor().b]
                            material.specular = map( lambda x: x * shader.reflectivity(), spec )
                        material.emissive = [shader.incandescence().r, shader.incandescence().g, shader.incandescence().b]
                    else:
                        color = [1.0, 1.0, 1.0]
                        material.color = map( lambda x: x, color )
                        material.alpha = 1.0
                        material.diffuseCoefficient = 1.0
                        material.specular = [0.0, 0.0, 0.0]
                        material.emissive = [0.0, 0.0, 0.0]

                    # Check if material is attached to any objects being exported
                    cmds.hyperShade( objects=shader.name() )
                    selected = cmds.ls( sl=True )
                    if selected:
                        for s in selected:
                            indexOfPeriod = s.find( '.f' )
                            if indexOfPeriod != -1:
                                # Face selected, get mesh
                                s = s[0:indexOfPeriod]
                                children = cmds.listRelatives( s, shapes=True )
                                s = children[0]
                            if s in meshes:
                                break
                        else:
                            itDN.next()
                            continue

                    # Get texture
                    if oNode.apiType() != OpenMaya.MFn.kSurfaceShader:
                        plugColor = shader.findPlug( 'color' )
                    else:
                        plugColor = shader.findPlug( 'outColor' )
                    plugsToColor = OpenMaya.MPlugArray()
                    plugColor.connectedTo( plugsToColor, True, False )

                    #
                    plugsToIncandescence = OpenMaya.MPlugArray()
                    if oNode.apiType() != OpenMaya.MFn.kSurfaceShader:
                        plugIncandescence = shader.findPlug( 'incandescence' )
                        plugIncandescence.connectedTo( plugsToIncandescence, True, False )

                        if plugsToColor.length() == 0 and plugsToIncandescence.length() > 0:
                            plugsToColor = plugsToIncandescence

                    if plugsToColor.length():
                        oSource = plugsToColor[0].node()
                        if oSource.hasFn( OpenMaya.MFn.kFileTexture ):
                            fnDepNodeFile = OpenMaya.MFnDependencyNode( oSource )
                            plugTexture = fnDepNodeFile.findPlug( 'fileTextureName' )
                            fileTextureName = plugTexture.asString()
                            if fileTextureName.startswith('lightMap'):
                                fileTextureName = startDirectory + 'renderData\\\\mentalray\\\\' + fileTextureName
                            if not self.args['absolutePath']:
                                # Relative path
                                material.texture = self.args['texturePath'] + os.path.basename( fileTextureName )
                            else:
                                # Absolute path
                                material.texture = fileTextureName
                            # Create new directories if needed
                            if not self.args['absolutePath']:
                                # Test if directory exists first
                                dir = os.path.dirname( material.texture )
                                if not os.path.exists( dir ):
                                    try:
                                        print 'Creating new directories for textures.'
                                        os.makedirs( dir )
                                    except:
                                        print 'ERROR: Unable to create new directories for textures. Check file permissions. Using absolute path instead.'
                                        material.texture = fileTextureName
                                        self.args['absolutePath'] = True
                            needToCopy = self.args['copyTexture']
                            if self.args['useDDS']:
                                ddsFile = os.path.splitext( material.texture )[0] + '.dds'
                                print 'Converting texture %s to dds texture %s.' % (fileTextureName, ddsFile)
                                if os.system( 'dxtex "%s" -m "%s"' % (fileTextureName, ddsFile ) ) == 0:
                                    # Change extension to dds
                                    material.texture = ddsFile
                                    # DDS conversion moves file automatically
                                    needToCopy = False
                                else:
                                    print 'Conversion failed. Using original texture.'
                                    needToCopy = self.args['copyTexture']
                            if self.args['copyTexture'] and not self.args['absolutePath'] and needToCopy:
                                print 'Copying %s to %s.' % (fileTextureName, material.texture)
                                try:
                                    shutil.copy( fileTextureName, material.texture )
                                except:
                                    print 'ERROR: Failed to copy texture. Check write permissions.'
                            material.texture = material.texture.replace( '/', '\\\\' )

                    name = shader.name().replace( '|', '_' )
                    name = name.replace( ':', '_' )
                    self.write( 'Material %s {\n' % name )
                    # Texture turns color black
                    if material.texture and plugsToIncandescence.length() == 0:
                        self.write( '% .6f;% .6f;% .6f;% .6f;;\n' % (material.diffuseCoefficient, material.diffuseCoefficient, material.diffuseCoefficient, material.alpha), 1 )
                    elif material.texture and plugsToIncandescence.length() > 0:
                        self.write( '1.0;1.0;1.0;% .6f;;\n' % (material.alpha), 1 )
                    else:
                        self.write( '% .6f;% .6f;% .6f;% .6f;;\n' % (material.color[0], material.color[1], material.color[2], material.alpha), 1 )
                    self.write( '% .6f;\n' % material.power )
                    self.write( '% .6f;% .6f;% .6f;;\n' % (material.specular[0], material.specular[1], material.specular[2]) )
                    if plugsToIncandescence.length() == 0:
                        self.write( '% .6f;% .6f;% .6f;;\n' % (material.emissive[0], material.emissive[1], material.emissive[2]) )
                    else:
                        self.write( '1.0;1.0;1.0;;\n' )
                    if material.texture:
                        self.write( 'TextureFileName {"%s";}\n' % material.texture )
                    self.write( '}\n', -1 )
                    cmds.progressWindow( edit=True, step=1 )
                itDN.next()
        except:
            print 'No materials found'
            from traceback import print_tb
            print sys.exc_info()[0]
            print_tb(sys.exc_info()[2])
            pass

        # Restore selection
        if originalSelection:
            cmds.select( originalSelection )

    def exportAll( self ):
        """
        Exports all transforms.
        """
        # Iterate over all transform nodes in the scene
        itDag = OpenMaya.MItDag( OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kTransform )
        while not itDag.isDone():
            path = OpenMaya.MDagPath()
            itDag.getPath( path )
            self.outputTransform( path, 0 )
            itDag.next()

        # Get animation
        if self.args['animation']:
            itDag = OpenMaya.MItDag( OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kTransform )
            while not itDag.isDone():
                path = OpenMaya.MDagPath()
                itDag.getPath( path )
                fnTransform = OpenMaya.MFnTransform( path )
                if fnTransform.fullPathName() in self.nodesOutput:
                    self.getFrameAnimation( path )
                itDag.next()

        # Close progress window
        cmds.progressWindow( endProgress=True )

    def exportSelection( self ):
        """
        Exports the selected transforms.
        """
        selectionList = OpenMaya.MSelectionList()
        OpenMaya.MGlobal.getActiveSelectionList( selectionList )

        itList = OpenMaya.MItSelectionList( selectionList, OpenMaya.MFn.kTransform )
        while not itList.isDone():
            path = OpenMaya.MDagPath()
            itList.getDagPath( path )
            self.outputTransform( path, 0 )
            itList.next()

        # Close progress window
        cmds.progressWindow( endProgress=True )

        # Get animation
        if self.args['animation']:
            itList = OpenMaya.MItSelectionList( selectionList, OpenMaya.MFn.kTransform )
            while not itList.isDone():
                path = OpenMaya.MDagPath()
                itList.getDagPath( path )
                fnTransform = OpenMaya.MFnTransform( path )
                if fnTransform.fullPathName() in self.nodesOutput:
                    self.getFrameAnimation( path )
                itList.next()

        # Close progress window
        cmds.progressWindow( endProgress=True )

    def isExportable( self, path ):
        """
        Tests if an object should be exported or not.
        """
        fnDagNode = OpenMaya.MFnDagNode( path )
        if fnDagNode.isIntermediateObject():
            # Don't export intermediate objects
            return False

        name = fnDagNode.fullPathName()

        plugVisibility = fnDagNode.findPlug( 'visibility' )
        # Don't export hidden objects
        return plugVisibility.asBool()

    def outputTransform( self, path, depth ):
        """
        Outputs Frame template.
        """

        if not self.isExportable( path ):
            return None

        fnTransform = OpenMaya.MFnTransform( path )

        # Check if node was already output
        if fnTransform.fullPathName() in self.nodesOutput:
            return None

        # Only output transform if it has a child transform or shape
        numChildren = fnTransform.childCount()
        for i in range( numChildren ):
            child = fnTransform.child( i )
            path.push( child )
            if path.node().apiType() == OpenMaya.MFn.kJoint or path.node().apiType() == OpenMaya.MFn.kTransform or path.node().apiType() == OpenMaya.MFn.kMesh:
                path.pop()
                break
            path.pop()
        else:
            # Joints can be leaf nodes
            if path.node().apiType() != OpenMaya.MFn.kJoint:
                return None
        self.nodesOutput.append( fnTransform.fullPathName() )

        isLevel = self.args['level']
            
        # Open Frame
        name = fnTransform.partialPathName().replace( '|', '_' )
        name = name.replace( ':', '_' )
        
        if (depth == 0 and isLevel) or (not isLevel):
            # process frame only if it is level export and we are on top nodes.
            # or we are not exporting level at all
            self.write( 'Frame %s {\n' % name )

            generateMetaScript = True
            if not isLevel: generateMetaScript = False
            if path.node().apiType() == OpenMaya.MFn.kJoint: generateMetaScript = False
            if path.node().apiType() == OpenMaya.MFn.kMesh: generateMetaScript = False
            if generateMetaScript: # in case of level, generate metaScript
                print 'Processing node %s' % name
                metaScript = None
                try:
                    fnDagNode = OpenMaya.MFnDagNode( path )
                    print fnDagNode
                    plugNotes = fnDagNode.findPlug( 'metaScript' )
                    metaScript = plugNotes.asString()
                except:
                    print 'no metaScript, will use default empty script'
                    metaScript = 'function run_notes(t)\nend\n'

                if metaScript:
                    name = name[:min(16, len(name))]
                    notesFilename = self.args['fileName'] + '_' + name + '.lua'
                    notesFilename = os.path.normpath(notesFilename)
                    notesTrimmedFilename = os.path.basename(notesFilename)
                    print 'creating metaScript file', notesFilename
                    notesFile = open(notesFilename, 'w')
                    
                    lines = metaScript.splitlines()
                    for l in lines:
                        notesFile.write('%s\n' % l)
                    notesFile.close()
                    self.write( 'MetaScript {\n', 1 )
                    self.write('"%s"\n' % notesTrimmedFilename, 1)
                    self.write('}\n', -1)
                    self.write('', -1)
                    
                peuuid = None
                try:
                    fnDagNode = OpenMaya.MFnDagNode( path )
                    print fnDagNode
                    plugNotes = fnDagNode.findPlug( 'peuuidStr' )
                    peuuid = plugNotes.asString()
                except:
                    print 'no peuuid, will use default empty peuuid'
                    peuuid = '0,0,0,0'

                if metaScript:
                    self.write( 'PEUUID {\n', 1 )
                    self.write('%s\n' % peuuid, 1)
                    self.write('}\n', -1)
                    self.write('', -1)
                    
            # END if isLevel -- generate metaScript    
            
            # FrameTransformMatrix
            self.write( 'FrameTransformMatrix {\n', 1 )
            mMatrix = fnTransform.transformationMatrix()

            # Flip for left-handed coordinate system
            matrix = XExporter.flipMatrix( mMatrix )

            self.write( '', 1 )
            for i in range( 4 ):
                for j in range( 4 ):
                    self.write( '%.6f' % matrix[i][j], indents=False )
                    if i == 3 and j == 3:
                        self.write( ';;\n', indents=False )
                    else:
                        self.write( ',', indents=False )
            self.write( '}\n', -1 )

            # Output children
            for i in range( numChildren ):
                child = fnTransform.child( i )
                path.push( child )
                if not isLevel and path.node().apiType() == OpenMaya.MFn.kJoint:
                    self.outputTransform( path, depth + 1 )
                if path.node().apiType() == OpenMaya.MFn.kTransform:
                    self.outputTransform( path, depth + 1 )
                elif not isLevel and path.node().apiType() == OpenMaya.MFn.kMesh:
                    self.outputMesh( path, mMatrix )
                path.pop()

            # Close Frame
            self.write( '}\n', -1 )
        # end of if depth == 0
    @staticmethod
    def flipMatrix( mMatrix ):
        A = [[],[],[],[]]
        for i in range( 4 ):
            for j in range( 4 ):
                A[i].append( mMatrix( i, j ) )
        A[3][2] *= -1
        A[1][2] *= -1
        A[2][1] *= -1
        A[0][2] *= -1
        A[2][0] *= -1
        return A

    def outputMesh( self, path, matrix ):
        """
        Outputs Mesh template.
        """
        xmesh = XMesh()
        if self.getMeshInfo( path, xmesh ):
            numVertices = xmesh.fnMesh.numFaceVertices()
            numFaces = xmesh.fnMesh.numPolygons()
            cmds.progressWindow( edit=True, progress=0, status="Writing Mesh...", maxValue=(numFaces + numFaces) )
            # Open Mesh
            name = xmesh.name.replace( '|', '_' )
            name = name.replace( ':', '_' )
            self.write( 'Mesh %s {\n' % xmesh.name )
            # Mesh.nVertices
            self.write( '%d;\n' % numVertices, 1 )
            # Mesh.vertices
            for i in range( numFaces ):
                for j in range( xmesh.vertices[i].length() ):
                    if not (i == 0 and j == 0):
                        self.write( ',\n', indents=False )
                    # Invert z for left-handed coordinate system
                    self.write( '% .6f; % .6f; % .6f;' % (xmesh.vertices[i][j].x, xmesh.vertices[i][j].y, -xmesh.vertices[i][j].z) )
                    if self.args['adjacencies']:
                        xmesh.vertsForAdjacency.append( xmesh.faceVertexIndices[i][j] )
                cmds.progressWindow( edit=True, step=1 )
            self.write( ';\n', indents=False )
            # Mesh.nFaces
            #self.write( '%d;\n' % len( xmesh.faceMaterialIndex ) )
            self.write( '%d;\n' % numFaces )
            # Mesh.faces (MeshFace)
            offset = 0
            for i in range( numFaces ):
                # MeshFace.nFaceVertexIndices
                numVertsForCurrentFace = xmesh.faceVertexIndices[i].length()
                self.write( '%d;' % numVertsForCurrentFace )
                # MeshFace.faceVertexIndices
                # Go in reverse for left-handed coordinate system
                for j in range( numVertsForCurrentFace - 1, -1, -1 ):
                    self.write( '%d' % (j + offset), indents=False )
                    if j == 0:
                        self.write( ';', indents=False )
                    else:
                        self.write( ',', indents=False )
                offset += numVertsForCurrentFace
                if i != numFaces - 1:
                    self.write( ',\n', indents=False )
                cmds.progressWindow( edit=True, step=1 )
            self.write( ';\n', indents=False )
            if self.args['normals']:
                self.outputNormals( xmesh )
            if self.args['uvs']:
                self.outputUVs( xmesh )
            if self.args['vertexColors']:
                self.outputVertexColors( xmesh )
            if self.args['materials']:
                self.outputMaterials( xmesh )
            if self.args['adjacencies']:
                self.outputAdjacencies( xmesh )
            if self.args['tangents'] or self.args['binormals']:
                self.outputDeclData( xmesh )
            if self.args['skinning']:
                self.outputSkinning( xmesh, matrix )

            # Close Mesh
            self.write( '}\n', -1 )

    def getMeshInfo( self, path, xmesh ):
        """
        Gets attributes for current mesh.
        """
        xmesh.fnMesh.setObject( path )
        if xmesh.fnMesh.isIntermediateObject():
            return False

        instanceNumber = 0
        if path.isInstanced():
            instanceNumber = path.instanceNumber()

        xmesh.name = xmesh.fnMesh.name()

        weights = OpenMaya.MDoubleArray()
        numInfluences = 0
        if self.args['skinning']:
            # Get skin weights
            plugInMesh = xmesh.fnMesh.findPlug( 'inMesh' )
            try:
                itDg = OpenMaya.MItDependencyGraph( plugInMesh, OpenMaya.MFn.kSkinClusterFilter, OpenMaya.MItDependencyGraph.kUpstream, OpenMaya.MItDependencyGraph.kDepthFirst, OpenMaya.MItDependencyGraph.kNodeLevel )

                while not itDg.isDone():
                    oNode = itDg.currentItem()
                    fnSkinCluster = OpenMayaAnim.MFnSkinCluster( oNode )

                    # Get components effected by deformer
                    fnSet = OpenMaya.MFnSet( fnSkinCluster.deformerSet() )
                    members = OpenMaya.MSelectionList()
                    fnSet.getMembers( members, False )
                    dagPath = OpenMaya.MDagPath()
                    components = OpenMaya.MObject()
                    members.getDagPath( 0, dagPath, components )

                    # Get skin weights
                    util = OpenMaya.MScriptUtil()
                    util.createFromInt( 0 )
                    pNumInfluences = util.asUintPtr()
                    fnSkinCluster.getWeights( dagPath, components, weights, pNumInfluences )
                    util = OpenMaya.MScriptUtil( pNumInfluences )
                    numInfluences = util.asUint()

                    break
            except:
                # No skin cluster found
                pass

        status = 'Gathering mesh info for mesh ' + xmesh.name + '...'
        numFaces = xmesh.fnMesh.numPolygons()
        cmds.progressWindow( edit=True, progress=0, status=status, maxValue=numFaces )

        # Get shaders
        shaders = OpenMaya.MObjectArray()
        xmesh.fnMesh.getConnectedShaders( instanceNumber, shaders, xmesh.faceMaterialIndices )
        for i in range( shaders.length() ):
            fnShader = OpenMaya.MFnDependencyNode( shaders[i] )
            plugShader = fnShader.findPlug( 'surfaceShader' )
            materials = OpenMaya.MPlugArray()
            plugShader.connectedTo( materials, True, False )
            for j in range( materials.length() ):
                if materials[j].node().apiType() == OpenMaya.MFn.kPhong or materials[j].node().apiType() == OpenMaya.MFn.kBlinn or materials[j].node().apiType() == OpenMaya.MFn.kLambert or materials[j].node().apiType() == OpenMaya.MFn.kSurfaceShader:
                    material = OpenMaya.MFnDependencyNode( materials[j].node() )
                    xmesh.materials.append( material.name() )
                else:
                    OpenMaya.MGlobal.displayError( 'Unsupported shader type of %s is attached to mesh %s' % (materials[j].node().apiTypeStr(), xmesh.name) )
                    return False

        oDeformedMesh = xmesh.fnMesh.object()
        if self.args['skinning']:
            # Use orig mesh, possible future update to mesh going into skin cluster
            try:
                itDg = OpenMaya.MItDependencyGraph( plugInMesh, OpenMaya.MFn.kMesh, OpenMaya.MItDependencyGraph.kUpstream, OpenMaya.MItDependencyGraph.kDepthFirst, OpenMaya.MItDependencyGraph.kNodeLevel )
                while not itDg.isDone():
                    oNode = itDg.currentItem()
                    fnDagNode = OpenMaya.MFnDagNode( oNode )
                    if not fnDagNode.isIntermediateObject():
                        itDg.next()
                        continue
                    xmesh.fnMesh.setObject( oNode )
                    break
                    itDg.next()
            except:
                # No orig mesh
                pass

        itPoly = OpenMaya.MItMeshPolygon( xmesh.fnMesh.object() )
        xmesh.weights = [[] for i in range( xmesh.fnMesh.numPolygons() )]
        while not itPoly.isDone():
            points = OpenMaya.MPointArray()
            verts = OpenMaya.MIntArray()
            uArray = OpenMaya.MFloatArray()
            vArray = OpenMaya.MFloatArray()
            colors = OpenMaya.MColorArray()
            itPoly.getPoints( points )
            itPoly.getVertices( verts )
            try:
                itPoly.getUVs( uArray, vArray )
            except:
                # No UVs
                xmesh.hasUVs = False
                pass
            #~ itPoly.getColors( colors )
            for i in range( points.length() ):
                if itPoly.hasColor( i ):
                    color = OpenMaya.MColor()
                    itPoly.getColor( color, i )
                    colors.append( color )
                elif self.args['vertexColor'] == 1:
                    # Store white for colorless vertex
                    colors.append( OpenMaya.MColor( 1.0, 1.0, 1.0, 1.0 ) )
                else:
                    # Store black for colorless vertex
                    colors.append( OpenMaya.MColor( 0.0, 0.0, 0.0, 1.0 ) )
            xmesh.vertices.append( points )
            xmesh.faceVertexIndices.append( verts )
            xmesh.uArray.append( uArray )
            xmesh.vArray.append( vArray )
            xmesh.vertexColors.append( colors )
            if self.args['skinning'] and numInfluences > 0:
                faceIndex = itPoly.index()
                numVertsInFace = verts.length()
                xmesh.weights[faceIndex] = [[] for i in range( numVertsInFace )]
                for i in range( numVertsInFace ):
                    # Store skin weights per face vertex
                    xmesh.weights[faceIndex][i] = [weights[z] for z in range( verts[i] * numInfluences, verts[i] * numInfluences + numInfluences )]
            cmds.progressWindow( edit=True, step=1 )
            itPoly.next()

        xmesh.fnMesh.getNormals( xmesh.normals )
        xmesh.fnMesh.getNormalIds( xmesh.normalsPerFace, xmesh.normalIndices )
        if xmesh.hasUVs:
            xmesh.fnMesh.getTangents( xmesh.tangents )
            xmesh.fnMesh.getBinormals( xmesh.binormals )

        # Restore deformed mesh so we can get skin cluster later on
        if self.args['skinning']:
            xmesh.fnMesh.setObject( oDeformedMesh )

        return True

    def outputNormals( self, xmesh ):
        """
        Outputs MeshNormals template.
        """
        numNormals = xmesh.normals.length()
        numFaces = xmesh.fnMesh.numPolygons()
        cmds.progressWindow( edit=True, progress=0, status="Writing MeshNormals...", maxValue=(numNormals + numFaces) )

        self.write( 'MeshNormals {\n' )
        # MeshNormals.nNormals
        self.write( '%d;\n' % numNormals, 1 )
        # MeshNormals.normals
        lastIndex = numNormals - 1
        for i in range( numNormals ):
            # Invert z for left-handed coordinate system
            self.write( '% .6f,% .6f,% .6f;' % (xmesh.normals[i].x, xmesh.normals[i].y, -xmesh.normals[i].z) )
            if i != lastIndex:
                self.write( ',\n', indents=False )
            cmds.progressWindow( edit=True, step=1 )
        self.write( ';\n', indents=False )
        # MeshNormals.nFaceNormals
        #self.write( '%d;\n' % len( xmesh.faceMaterialIndex ) )
        self.write( '%d;\n' % numFaces )
        # MeshNormals.faceNormals (MeshFace)
        offset = 0
        for i in range( numFaces ):
            # MeshFace.nFaceVertexIndices
            numVertsForCurrentFace = xmesh.faceVertexIndices[i].length()
            self.write( '%d;' % numVertsForCurrentFace )
            # MeshFace.faceVertexIndices
            # Go in reverse for left-handed coordinate system
            for j in range( xmesh.normalsPerFace[i] - 1, -1, -1 ):
                self.write( '%d' % xmesh.normalIndices[offset + j], indents=False )
                if j == 0:
                    self.write( ';', indents=False )
                else:
                    self.write( ',', indents=False )
            offset += xmesh.normalsPerFace[i]
            if i != numFaces - 1:
                self.write( ',\n', indents=False )
            cmds.progressWindow( edit=True, step=1 )
        self.write( ';\n', indents=False )
        self.write( '}\n', -1 )

    def outputUVs( self, xmesh ):
        """
        Outputs MeshTextureCoords template.
        """
        if not xmesh.hasUVs:
            return None
        numFaces = xmesh.fnMesh.numPolygons()
        numVertices = xmesh.fnMesh.numFaceVertices()
        cmds.progressWindow( edit=True, progress=0, status="Writing MeshTextureCoords...", maxValue=numFaces )
        self.write( 'MeshTextureCoords {\n' )
        # MeshTextureCoords.nTextureCoords
        self.write( '%d;\n' % numVertices, 1 )
        lastIndex = numFaces - 1
        for i in range( numFaces ):
            lastIndexUV = xmesh.uArray[i].length() - 1
            for j in range( xmesh.uArray[i].length() ):
                self.write( '% .6f;% .6f;' % (xmesh.uArray[i][j], -xmesh.vArray[i][j]) )
                if not (i == lastIndex and j == lastIndexUV):
                    self.write( ',\n', indents=False )

            cmds.progressWindow( edit=True, step=1 )
        self.write( ';\n', indents=False )
        self.write( '}\n', -1 )

    def outputVertexColors( self, xmesh ):
        """
        Outputs MeshVertexColors template.
        """
        numFaces = xmesh.fnMesh.numPolygons()
        numVertices = xmesh.fnMesh.numFaceVertices()
        cmds.progressWindow( edit=True, progress=0, status="Writing MeshVertexColors...", maxValue=numFaces )
        self.write( 'MeshVertexColors {\n' )
        # MeshVertexColors.nVertexColors
        self.write( '%d;\n' % numVertices, 1 )
        lastIndex = numFaces - 1
        index = 0
        for i in range( numFaces ):
            lastIndex2 = xmesh.vertexColors[i].length() - 1
            for j in range( xmesh.vertexColors[i].length() ):
                self.write( '%d;% .6f;% .6f;% .6f;% .6f;;' % (index, xmesh.vertexColors[i][j].r, xmesh.vertexColors[i][j].g, xmesh.vertexColors[i][j].b, xmesh.vertexColors[i][j].a) )
                if not (i == lastIndex and j == lastIndex2):
                    self.write( ',\n', indents=False )
                index += 1
            cmds.progressWindow( edit=True, step=1 )
        self.write( ';\n', indents=False )
        self.write( '}\n', -1 )

    def outputMaterials( self, xmesh ):
        """
        Outputs MeshMaterialList template.
        """
        numFaces = xmesh.faceMaterialIndices.length()
        cmds.progressWindow( edit=True, progress=0, status="Writing MeshMaterialList...", maxValue=numFaces )
        self.write( 'MeshMaterialList {\n' )
        # MeshMaterialList.nMaterials
        self.write( '%d;\n' % len( xmesh.materials ), 1 )
        # MeshMaterialList.nFaceIndices
        self.write( '%d;\n' % numFaces )
        lastIndex = xmesh.faceMaterialIndices.length() - 1
        for i in range( xmesh.faceMaterialIndices.length() ):
            self.write( '%d' % xmesh.faceMaterialIndices[i] )
            if i != lastIndex:
                self.write( ',\n', indents=False )
            cmds.progressWindow( edit=True, step=1 )
        self.write( ';\n', indents=False )

        # Materials
        for material in xmesh.materials:
            name = material.replace( '|', '_' )
            name = name.replace( ':', '_' )
            self.write( '{%s}\n' % name )

        self.write( '}\n', -1 )

    def outputAdjacencies( self, xmesh ):
        """
        Outputs VertexDuplicationIndices template.
        """
        numFaceVertices = xmesh.fnMesh.numFaceVertices()
        numVertices = xmesh.fnMesh.numVertices()
        cmds.progressWindow( edit=True, progress=0, status="Writing VertexDuplicationIndices...", maxValue=numFaceVertices)
        self.write( 'VertexDuplicationIndices {\n' )
        # VertexDuplicationIndices.nIndices
        self.write( '%d;\n' % numFaceVertices, 1 )
        # VertexDuplicationIndices.nOriginalVertices
        self.write( '%d;\n' % numVertices )
        lastIndex = numFaceVertices - 1

        indicesInArray = {}
        for i in range( len( xmesh.vertsForAdjacency ) ):
            if not indicesInArray.has_key( xmesh.vertsForAdjacency[i] ):
                indicesInArray[xmesh.vertsForAdjacency[i]] = []
                indicesInArray[xmesh.vertsForAdjacency[i]].append( i )
            self.write( '%d' % indicesInArray[xmesh.vertsForAdjacency[i]][0] )
            if i != lastIndex:
                self.write( ',\n', indents=False )
            cmds.progressWindow( edit=True, step=1 )

        self.write( ';\n', indents=False )
        self.write( '}\n', -1 )

    def outputDeclData( self, xmesh ):
        """
        Outputs the DeclData template.
        """
        if not xmesh.hasUVs:
            return None
        numElements = 0
        numFloats = 0
        tangentElement = '2;0;6;0;'
        binormalElement = '2;0;7;0;;\n'

        if self.args['tangents']:
            numElements += 1
            numFloats += 3
        if self.args['binormals']:
            numElements += 1
            numFloats += 3
            tangentElement += ',\n'
        else:
            tangentElement += ';\n'
        self.write( 'DeclData {\n' )
        self.write( '%d;\n' % numElements, 1 )
        if self.args['tangents']:
            self.write( tangentElement )
        if self.args['binormals']:
            self.write( binormalElement )
        self.write( '%d;\n' % (xmesh.fnMesh.numFaceVertices() * numFloats) )

        numTangents = xmesh.tangents.length()
        cmds.progressWindow( edit=True, progress=0, status="Writing DeclData...", maxValue=numTangents )
        lastIndex = numTangents - 1
        # Flip z for left-handed coordinate system
        for i in range( numTangents ):
            cmds.progressWindow( edit=True, step=1 )
            if self.args['tangents']:
                tx = self.convertFloatToDWORD( xmesh.tangents[i].x )
                ty = self.convertFloatToDWORD( xmesh.tangents[i].y )
                tz = self.convertFloatToDWORD( -xmesh.tangents[i].z )
                self.write( '%d,\n' % tx )
                self.write( '%d,\n' % ty )
                self.write( '%d' % tz )
                if self.args['binormals']:
                    self.write( ',\n', indents=False )
            if self.args['binormals']:
                bx = self.convertFloatToDWORD( xmesh.binormals[i].x )
                by = self.convertFloatToDWORD( xmesh.binormals[i].y )
                bz = self.convertFloatToDWORD( -xmesh.binormals[i].z )
                self.write( '%d,\n' % bx )
                self.write( '%d,\n' % by )
                self.write( '%d' % bz )
            if i != lastIndex:
                self.write( ',\n', indents=False )
        self.write( ';\n', indents=False )
        self.write( '}\n', -1 )

    def convertFloatToDWORD( self, float ):
        """
        Reinterprets the bytes of a float into a DWORD. Requires the ctypes module.
        """
        pF = ctypes.pointer( ctypes.c_float( float ) )
        pDw = ctypes.cast( pF, ctypes.POINTER( ctypes.c_uint ) )
        return pDw[0]

    def getFrameAnimation( self, path ):
        """
        Get animation data for the current dag node.
        """
        fnTransform = OpenMaya.MFnTransform( path )
        amount = 0
        for set in self.animationSets:
            amount += set.end - set.start
        print 'amount :', amount
        cmds.progressWindow( edit=True, progress=0, status="Gathering animation data...", maxValue=amount )
        originalTime = OpenMayaAnim.MAnimControl.currentTime()
        for set in self.animationSets:
            translations = []
            scales = []
            rotations = []
            transforms = []
            for j in range( set.start, set.end + 1 ):
                cmds.progressWindow( edit=True, step=1 )
                # Set current frame
                OpenMayaAnim.MAnimControl.setCurrentTime( OpenMaya.MTime( j, OpenMaya.MTime.uiUnit() ) )

                if self.args['animationFormat'] == 2:
                    # Matrix animation
                    mMatrix = fnTransform.transformationMatrix()
                    # Flip for left-handed coordinate system
                    matrix = [[],[],[],[]]
                    for i in range( 4 ):
                        for j in range( 4 ):
                            matrix[i].append( mMatrix( i, j ) )
                    matrix[3][2] = -matrix[3][2]
                    matrix[1][2] = -matrix[1][2]
                    matrix[2][1] = -matrix[2][1]
                    matrix[0][2] = -matrix[0][2]
                    matrix[2][0] = -matrix[2][0]
                    transforms.append( matrix )
                else:
                    print 'ERROR 1'
                    raw_input()
                    # SRT animtion
                    # Get rotation
                    rotation = OpenMaya.MQuaternion()
                    fnTransform.getRotation( rotation )
                    # Account for rotateOrient (rotate axis)
                    rotateOrient = fnTransform.rotateOrientation( OpenMaya.MSpace.kTransform )
                    rotation = rotateOrient * rotation
                    if fnTransform.object().hasFn( OpenMaya.MFn.kJoint ):
                        # Joints need to account for jointOrient
                        fnJoint = OpenMayaAnim.MFnIkJoint( fnTransform.object() )
                        jointOrient = OpenMaya.MQuaternion()
                        fnJoint.getOrientation( jointOrient )
                        rotation *= jointOrient
                    # Flip for left-handed coordinate system
                    rotation.x = -rotation.x
                    rotation.y = -rotation.y
                    rotation.w = -rotation.w
                    rotations.append( rotation )

                    # Get scale
                    util = OpenMaya.MScriptUtil()
                    util.createFromDouble( 0.0, 0.0, 0.0 )
                    pScale = util.asDoublePtr()
                    fnTransform.getScale( pScale )
                    scale = [OpenMaya.MScriptUtil.getDoubleArrayItem( pScale, 0 ), OpenMaya.MScriptUtil.getDoubleArrayItem( pScale, 1 ), OpenMaya.MScriptUtil.getDoubleArrayItem( pScale, 2 ) ]
                    scales.append( scale )

                    # Get translation
                    mMatrix = fnTransform.transformationMatrix()
                    # Flip z for left-handed coordinate system
                    translation = OpenMaya.MVector()
                    translation.x = mMatrix( 3, 0 )
                    translation.y = mMatrix( 3, 1 )
                    translation.z = -mMatrix( 3, 2 )
                    translations.append( translation )
            noAnimationForNode = False
            if (self.args['animationFormat'] == 2):
                # Matrix format
                if len( transforms ) > 0 and transforms.count( transforms[0] ) == len( transforms ):
                    noAnimationForNode = True
            else:
                print 'ERROR 2'
                raw_input()
                #SRT animation
                if (len( rotations ) > 0 and rotations.count( rotations[0] ) == len( rotations )) and \
                    (len( scales ) > 0 and scales.count( scales[0] ) == len( scales )) and \
                    (len( translations ) > 0 and translations.count( translations[0] ) == len( translations )):
                    noAnimationForNode = True
            #if (((len( rotations ) > 0 and rotations.count( rotations[0] ) == len( rotations )) and
            #            (len( scales ) > 0 and scales.count( scales[0] ) == len( scales )) and
            #     (len( translations ) > 0 and translations.count( translations[0] ) == len( translations ))) or
            #     (len( transforms ) > 0 and transforms.count( transforms[0] ) == len( transforms ))):
                # No animation on this frame
            #    pass
            #else:
            #    set.animations.append( XAnimation( fnTransform.partialPathName(), rotations, scales, translations, transforms ) )
            #potentially could look at noAnimationForNode to not export animation
            set.animations.append( XAnimation( fnTransform.partialPathName(), rotations, scales, translations, transforms ) )
        # Reset timeline
        OpenMayaAnim.MAnimControl.setCurrentTime( originalTime )

    def exportAnimation( self ):
        """
        Writes the Animation templates.
        """
        for set in self.animationSets:

            self.write( 'AnimationSet %s {\n' % set.name )
            numKeys = set.end - set.start + 1
            first = 1

            amount = len( set.animations )
            if amount != 0:
                status = 'Writing animation set %s...' % set.name
                cmds.progressWindow( edit=True, progress=0, status=status, maxValue=amount )

            for animation in set.animations:
                cmds.progressWindow( edit=True, step=1 )

                self.write( 'Animation {\n', first )
                first = 0
                name = animation.frame.replace( '|', '_' )
                name = name.replace( ':', '_' )
                self.write( '{ %s }\n' % name, 1 )

                if self.args['animationFormat'] == 2:
                    # Matrix AnimationKey
                    self.write( 'AnimationKey {\n' )
                    # AnimationKey.keyType
                    self.write( '4;\n', 1 )
                    # AnimationKey.nKeys
                    self.write( '%d;\n' % numKeys )
                    for i in range( 1, numKeys + 1 ):
                        self.write( '%d;16;' % i )
                        for j in range( 4 ):
                            for k in range( 4 ):
                                self.write( '% .6f;' % animation.transforms[i - 1][j][k], indents=False )
                        self.write( ';;\n', indents=False )
                    self.write( '}\n', -1 )
                else:
                    print 'ERROR 3'
                    raw_input()
                    # SRT AnimationKeys
                    # Rotate
                    self.write( 'AnimationKey {\n' )
                    # AnimationKey.keyType
                    self.write( '0;\n', 1 )
                    # AnimationKey.nKeys
                    self.write( '%d;\n' % numKeys )
                    for i in range( 1, numKeys + 1 ):
                        self.write( '%d;4;% .6f,% .6f,% .6f,% .6f;;' % (i, animation.rotations[i - 1].w, animation.rotations[i - 1].x, animation.rotations[i - 1].y, animation.rotations[i - 1].z) )
                        if i != numKeys:
                            self.write( ',\n', indents=False )
                        else:
                            self.write( ';\n', indents=False )
                    self.write( '}\n', -1 )

                    # Scale
                    self.write( 'AnimationKey {\n' )
                    # AnimationKey.keyType
                    self.write( '1;\n', 1 )
                    # AnimationKey.nKeys
                    self.write( '%d;\n' % numKeys )
                    for i in range( 1, numKeys + 1 ):
                        self.write( '%d;4;% .6f,% .6f,% .6f;;' % (i, animation.scales[i - 1][0], animation.scales[i - 1][1], animation.scales[i - 1][2]) )
                        if i != numKeys:
                            self.write( ',\n', indents=False )
                        else:
                            self.write( ';\n', indents=False )
                    self.write( '}\n', -1 )

                    # Translate
                    self.write( 'AnimationKey {\n' )
                    # AnimationKey.keyType
                    self.write( '2;\n', 1 )
                    # AnimationKey.nKeys
                    self.write( '%d;\n' % numKeys )
                    for i in range( 1, numKeys + 1 ):
                        self.write( '%d;4;% .6f,% .6f,% .6f;;' % (i, animation.translations[i - 1][0], animation.translations[i - 1][1], animation.translations[i - 1][2]) )
                        if i != numKeys:
                            self.write( ',\n', indents=False )
                        else:
                            self.write( ';\n', indents=False )
                    self.write( '}\n', -1 )

                self.write( '}\n', -1 )
            self.write( '}\n', -1 )


    def outputSkinning( self, xmesh, matrix ):
        """
        Outputs skinning information.
        """
        pathSkin = OpenMaya.MDagPath()
        xmesh.fnMesh.getPath( pathSkin )
        pathSkin.pop()
        fnTransform = OpenMaya.MFnTransform( pathSkin.node() )
        meshMatrix = fnTransform.transformationMatrix()
        # meshMatrix is in object space.  Multiply by all parent matrices to get world space
        newPath = OpenMaya.MDagPath( pathSkin )
        while True:
            try:
                newPath.pop()
                fnTransform = OpenMaya.MFnTransform( newPath.node() )
                meshMatrix *= fnTransform.transformationMatrix()
            except:
                break
        meshMatrix = meshMatrix.inverse()
        plugInMesh = xmesh.fnMesh.findPlug( 'inMesh' )
        itDg = None
        try:
            itDg = OpenMaya.MItDependencyGraph( plugInMesh, OpenMaya.MFn.kSkinClusterFilter, OpenMaya.MItDependencyGraph.kUpstream, OpenMaya.MItDependencyGraph.kDepthFirst, OpenMaya.MItDependencyGraph.kNodeLevel )
        except:
            # No skin cluster found
            return None
        numFaces = xmesh.fnMesh.numPolygons()
        while not itDg.isDone():
            oNode = itDg.currentItem()
            fnSkinCluster = OpenMayaAnim.MFnSkinCluster( oNode )
            influenceObjects = OpenMaya.MDagPathArray()
            numInfluences = fnSkinCluster.influenceObjects( influenceObjects )
            # XSkinMeshHeader
            self.write( 'XSkinMeshHeader {\n' )
            # XSkinMeshHeader.nMaxSkinWeightsPerVertex
            self.write( '%d;\n' % numInfluences, 1 )
            # XSkinMeshHeader.nMaxSkinWeightsPerFace
            self.write( '%d;\n' % numInfluences )
            # XSkinMeshHeader.nBones
            self.write( '%d;\n' % numInfluences )
            self.write( '}\n', -1 )

            # SkinWeights
            cmds.progressWindow( edit=True, progress=0, status="Writing SkinWeights...", maxValue=numInfluences )
            for i in range( numInfluences ):
                cmds.progressWindow( edit=True, step=1 )

                self.write( 'SkinWeights {\n' )
                # SkinWeights.transformNodeName
                name = influenceObjects[i].partialPathName().replace( '|', '_' )
                name = name.replace( ':', '_' )
                self.write( '"%s";\n' % name, 1 )

                vertexIndices = []
                nonZeroWeights = []
                offset = 0
                for j in range( numFaces ):
                    numVertsForCurrentFace = len( xmesh.weights[j] )
                    for k in range( numVertsForCurrentFace ):
                        if xmesh.weights[j][k][i] > 0.0001:
                            vertexIndices.append( k + offset )
                            nonZeroWeights.append( xmesh.weights[j][k][i] )
                    offset += numVertsForCurrentFace
                numNonZeroWeights = len( nonZeroWeights )
                # SkinWeights.nWeights
                self.write( '%d;\n' % numNonZeroWeights )
                # SkinWeights.vertexIndices
                haveOutput = False
                for index in vertexIndices:
                        if haveOutput:
                            self.write( ',\n', indents=False )
                        else:
                            haveOutput = True
                        self.write( '%d' % index )
                self.write( ';\n', indents=False )
                # SkinWeights.weights
                haveOutput = False
                for weight in nonZeroWeights:
                    if haveOutput:
                        self.write( ',\n', indents=False )
                    else:
                        haveOutput = True
                    self.write( '%.6f' % weight )

                self.write( ';\n', indents=False )

                # Get bone offset matrix
                boneOffsetMMatrix = None
                for joint in self.joints:
                    if joint.name == influenceObjects[i].partialPathName():
                        boneOffsetMMatrix = (joint.worldBindPoseMatrix * meshMatrix).inverse()
                        break
                else:
                    OpenMaya.MGlobal.displayError( 'Could not find boneOffsetMatrix' )
                    raise
                # Adjust for left-handed coordinate system
                boneOffsetMatrix = XExporter.flipMatrix( boneOffsetMMatrix )

                self.write( '' )
                for j in range( 4 ):
                    for k in range( 4 ):
                        self.write( '%.6f' % boneOffsetMatrix[j][k], indents=False )
                        if j == 3 and k == 3:
                            self.write( ';;\n', indents=False )
                        else:
                            self.write( ',', indents=False )
                self.write( '}\n', -1 )
            break

    def calculateWorldBindPoses( self, influenceObjects, plugBindPreMatrixArray ):
        """
        Calculates the local bind pose matrices for the joints affecting xmesh.
        """
        amount = influenceObjects.length()
        cmds.progressWindow( edit=True, progress=0, status="Calculating bind pose matrices...", maxValue=amount )
        for i in range( influenceObjects.length() ):
            cmds.progressWindow( edit=True, step=1 )
            fnJoint = None
            if influenceObjects[i].node().hasFn( OpenMaya.MFn.kJoint ):
                fnJoint = OpenMayaAnim.MFnIkJoint( influenceObjects[i].node() )
            else:
                OpenMaya.MGlobal.displayError( 'Exporter does not support non-joint influence objects.' )
                raise

            # See if this joint is already stored
            for j in self.joints:
                if j.name == fnJoint.partialPathName():
                    continue

            pathJoint = OpenMaya.MDagPath()
            fnJoint.getPath( pathJoint )
            if pathJoint.instanceNumber() != 0:
                # Only do first instance
                continue

            # Get bind pose matrix
            xjoint = None
            for joint in self.joints:
                if joint.name == fnJoint.partialPathName():
                    xjoint = joint
                    break
            else:
                OpenMaya.MGlobal.displayError( 'Could not find stored joint' )
                raise

            xjoint.worldBindPoseMatrix = self.getWorldBindPoseMatrix( plugBindPreMatrixArray.elementByPhysicalIndex( i ) )

    def getWorldBindPoseMatrix( self, plugBindPreMatrix ):
        fnMatrixData = OpenMaya.MFnMatrixData( plugBindPreMatrix.asMObject() )
        matrix = fnMatrixData.matrix()
        return matrix.inverse()



