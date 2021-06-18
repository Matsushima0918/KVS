#  DON'T EDIT THIS FILE.
#  THIS IS GENERATED BY "Configuration/configure_BuildRule.py".


OBJECTS = \
$(OUTDIR)\.\Communicator.obj \
$(OUTDIR)\.\Environment.obj \
$(OUTDIR)\.\MPICall.obj \
$(OUTDIR)\.\Renderer\234Compositor\234compositor.obj \
$(OUTDIR)\.\Renderer\234Compositor\compress.obj \
$(OUTDIR)\.\Renderer\234Compositor\exchange.obj \
$(OUTDIR)\.\Renderer\234Compositor\merge.obj \
$(OUTDIR)\.\Renderer\234Compositor\misc.obj \
$(OUTDIR)\.\Renderer\ImageCompositor.obj \
$(OUTDIR)\.\Request.obj \
$(OUTDIR)\.\Window.obj \



{.\Renderer\234Compositor\}.cpp{$(OUTDIR)\.\Renderer\234Compositor\}.obj::
	IF NOT EXIST $(OUTDIR)\.\Renderer\234Compositor $(MKDIR) $(OUTDIR)\.\Renderer\234Compositor
	$(MPICPP) /c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) /Fo$(OUTDIR)\.\Renderer\234Compositor\ @<<
$<
<<

{.\Renderer\}.cpp{$(OUTDIR)\.\Renderer\}.obj::
	IF NOT EXIST $(OUTDIR)\.\Renderer $(MKDIR) $(OUTDIR)\.\Renderer
	$(MPICPP) /c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) /Fo$(OUTDIR)\.\Renderer\ @<<
$<
<<

{.\}.cpp{$(OUTDIR)\.\}.obj::
	IF NOT EXIST $(OUTDIR)\. $(MKDIR) $(OUTDIR)\.
	$(MPICPP) /c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) /Fo$(OUTDIR)\.\ @<<
$<
<<


install::
	IF NOT EXIST $(INSTALL_DIR)\include\SupportMPI\. $(MKDIR) $(INSTALL_DIR)\include\SupportMPI\.
	$(INSTALL) .\*.h $(INSTALL_DIR)\include\SupportMPI\.
	IF NOT EXIST $(INSTALL_DIR)\include\SupportMPI\.\Renderer $(MKDIR) $(INSTALL_DIR)\include\SupportMPI\.\Renderer
	$(INSTALL) .\Renderer\*.h $(INSTALL_DIR)\include\SupportMPI\.\Renderer
	IF NOT EXIST $(INSTALL_DIR)\include\SupportMPI\.\Renderer\234Compositor $(MKDIR) $(INSTALL_DIR)\include\SupportMPI\.\Renderer\234Compositor
	$(INSTALL) .\Renderer\234Compositor\*.h $(INSTALL_DIR)\include\SupportMPI\.\Renderer\234Compositor
