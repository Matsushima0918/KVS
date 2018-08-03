#  DON'T EDIT THIS FILE.
#  THIS IS GENERATED BY "Configuration/configure_BuildRule.py".


OBJECTS = \
$(OUTDIR)\.\Renderer\Axis2D.obj \
$(OUTDIR)\.\Renderer\Axis2DMatrix.obj \
$(OUTDIR)\.\Renderer\Axis3D.obj \
$(OUTDIR)\.\Renderer\ParallelAxis.obj \
$(OUTDIR)\.\Viewer\Application.obj \
$(OUTDIR)\.\Viewer\Screen.obj \
$(OUTDIR)\.\Viewer\ScreenBase.obj \
$(OUTDIR)\.\Viewer\Text.obj \
$(OUTDIR)\.\Viewer\Timer.obj \
$(OUTDIR)\.\Widget\CheckBox.obj \
$(OUTDIR)\.\Widget\CheckBoxGroup.obj \
$(OUTDIR)\.\Widget\ColorMapPalette.obj \
$(OUTDIR)\.\Widget\ColorPalette.obj \
$(OUTDIR)\.\Widget\Histogram.obj \
$(OUTDIR)\.\Widget\Label.obj \
$(OUTDIR)\.\Widget\LegendBar.obj \
$(OUTDIR)\.\Widget\OpacityMapPalette.obj \
$(OUTDIR)\.\Widget\OrientationAxis.obj \
$(OUTDIR)\.\Widget\PushButton.obj \
$(OUTDIR)\.\Widget\RadioButton.obj \
$(OUTDIR)\.\Widget\RadioButtonGroup.obj \
$(OUTDIR)\.\Widget\Slider.obj \
$(OUTDIR)\.\Widget\TransferFunctionEditor.obj \
$(OUTDIR)\.\Widget\WidgetBase.obj \



{.\Widget\}.cpp{$(OUTDIR)\.\Widget\}.obj::
	IF NOT EXIST $(OUTDIR)\.\Widget $(MKDIR) $(OUTDIR)\.\Widget
	$(CPP) /c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) /Fo$(OUTDIR)\.\Widget\ @<<
$<
<<

{.\Viewer\}.cpp{$(OUTDIR)\.\Viewer\}.obj::
	IF NOT EXIST $(OUTDIR)\.\Viewer $(MKDIR) $(OUTDIR)\.\Viewer
	$(CPP) /c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) /Fo$(OUTDIR)\.\Viewer\ @<<
$<
<<

{.\Renderer\}.cpp{$(OUTDIR)\.\Renderer\}.obj::
	IF NOT EXIST $(OUTDIR)\.\Renderer $(MKDIR) $(OUTDIR)\.\Renderer
	$(CPP) /c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) /Fo$(OUTDIR)\.\Renderer\ @<<
$<
<<


install::
	IF NOT EXIST $(INSTALL_DIR)\include\SupportGLUT\. $(MKDIR) $(INSTALL_DIR)\include\SupportGLUT\.
	$(INSTALL) .\*.h $(INSTALL_DIR)\include\SupportGLUT\.
	IF NOT EXIST $(INSTALL_DIR)\include\SupportGLUT\.\Renderer $(MKDIR) $(INSTALL_DIR)\include\SupportGLUT\.\Renderer
	$(INSTALL) .\Renderer\*.h $(INSTALL_DIR)\include\SupportGLUT\.\Renderer
	IF NOT EXIST $(INSTALL_DIR)\include\SupportGLUT\.\Viewer $(MKDIR) $(INSTALL_DIR)\include\SupportGLUT\.\Viewer
	$(INSTALL) .\Viewer\*.h $(INSTALL_DIR)\include\SupportGLUT\.\Viewer
	IF NOT EXIST $(INSTALL_DIR)\include\SupportGLUT\.\Widget $(MKDIR) $(INSTALL_DIR)\include\SupportGLUT\.\Widget
	$(INSTALL) .\Widget\*.h $(INSTALL_DIR)\include\SupportGLUT\.\Widget
