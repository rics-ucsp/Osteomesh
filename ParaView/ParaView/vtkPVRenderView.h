/*=========================================================================

  Program:   ParaView
  Module:    vtkPVRenderView.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2000-2001 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific 
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
// .NAME vtkPVRenderView - For using styles
// .SECTION Description
// This is a render view that is a parallel object.  It needs to be cloned
// in all the processes to work correctly.  After cloning, the parallel
// nature of the object is transparent.
// Other features:
// I am going to try to divert the events to a vtkInteractorStyle object.
// I also have put compositing into this object.  I had to create a separate
// renderwindow and renderer for the off screen compositing (Hacks).
// Eventually I need to merge these back into the main renderer and renderer
// window.


#ifndef __vtkPVRenderView_h
#define __vtkPVRenderView_h

#include "vtkKWView.h"

class vtkKWLabel;
class vtkKWPushButton;
class vtkKWRadioButton;
class vtkKWScale;
class vtkKWSplitFrame;
class vtkLabeledFrame;
class vtkMultiProcessController;
class vtkPVApplication;
class vtkPVCameraIcon;
class vtkPVData;
class vtkPVInteractorStyleControl;
class vtkPVRenderViewObserver;
class vtkPVSource;
class vtkPVSourceList;
class vtkPVSourcesNavigationWindow;
class vtkPVTreeComposite;
class vtkPVWindow;

#define VTK_PV_VIEW_MENU_LABEL       " 3D View Properties"

class VTK_EXPORT vtkPVRenderView : public vtkKWView
{
public:
  static vtkPVRenderView* New();
  vtkTypeRevisionMacro(vtkPVRenderView,vtkKWView);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the application right after construction.
  void CreateRenderObjects(vtkPVApplication *pvApp);
  
  // Description:
  // Add widgets to vtkKWView's notebook
  virtual void CreateViewProperties();
  
  // Description:
  // Create the TK widgets associated with the view.
  virtual void Create(vtkKWApplication *app, const char *args);

  // Description:
  // Compute the bounding box of all the visibile props
  // Used in ResetCamera() and ResetCameraClippingRange()
  void ComputeVisiblePropBounds( float bounds[6] ); 
  
  // Description:
  // Method called by the toolbar reset camera button.
  void ResetCamera();
  
  // Description:
  // Specify the position, focal point, and view up of the camera.
  void SetCameraState(float p0, float p1, float p2,
                      float fp0, float fp1, float fp2,
                      float up0, float up1, float up2);

  // Description:
  // Reset the camera clipping range based on the bounds of the
  // visible actors. This ensures that no props are cut off
  void ResetCameraClippingRange();

  // Description:
  // Callback to set the view up or down one of the three axes.
  void StandardViewCallback(float x, float y, float z);

  // Description:
  // This method is executed in all processes.
  void AddPVSource(vtkPVSource *pvs);
  void RemovePVSource(vtkPVSource *pvs);

  // Description:
  // Make snapshot of the render window.
  virtual void SaveAsImage() { this->Superclass::SaveAsImage(); }
  virtual void SaveAsImage(const char* filename) ;
  
  // Description:
  // Casts to vtkPVApplication.
  vtkPVApplication *GetPVApplication();

  // Description:
  // Computes the reduction factor to use in compositing.
  void StartRender();
  
  // Description:
  // Composites
  void Render();
  void EventuallyRender();
  void EventuallyRenderCallBack();

  // Description:
  // Tcl "update" has to be called for various reasons (packing).
  // This calls update without triggering EventuallyRenderCallback.
  // I was having problems with multiple renders.
  void UpdateTclButAvoidRendering();
  
  // Description:
  // Update all the actors.
  void Update();

  // Description:
  // Callback method bound to expose or configure events.
  void Exposed();
  void Configured();
  
  // Description:
  // Are we currently in interactive mode?
  int IsInteractive() { return this->Interactive; }
  
  // Description:
  // My version.
  vtkRenderer *GetRenderer();
  vtkRenderWindow *GetRenderWindow();
  const char *GetRenderWindowTclName() {return this->RenderWindowTclName;}

  // Description:
  // The center of rotation picker needs the compositers zbuffer.
  vtkPVTreeComposite *GetComposite() {return this->Composite;}

  // Description:
  // Update the navigation window for a particular source
  void UpdateNavigationWindow(vtkPVSource *currentSource, int nobind);

  // Description:
  // Get the frame for the navigation window
  vtkGetObjectMacro(NavigationFrame, vtkKWLabeledFrame);

  // Description:
  // Show either navigation window with a fragment of pipeline or a
  // source window with a list of sources. If the argument registery
  // is 1, then the value will be stored in the registery.
  void ShowNavigationWindowCallback(int registery);
  void ShowSelectionWindowCallback(int registery);
  
  void AddBindings();
  
  // Description:
  // Export the renderer and render window to a file.
  void SaveInBatchScript(ofstream *file);
  void SaveState(ofstream *file);

  // Description:
  // Change the background color.
  void SetBackgroundColor(float r, float g, float b);
  virtual void SetBackgroundColor(float *c) {this->SetBackgroundColor(c[0],c[1],c[2]);}

  // Description:
  // Close the view - called from the vtkkwwindow. This default method
  // will simply call Close() for all the composites. Can be overridden.
  virtual void Close();

  // Description:
  // This method Sets all IVars to NULL and unregisters
  // vtk objects.  This should eliminate circular references.
  void PrepareForDelete();
  
  // Description:
  // Get the tcl name of the renderer.
  vtkGetStringMacro(RendererTclName);
  
  // Description:
  // Set this flag to indicate whether to calculate the reduction factor for
  // use in tree composite.
  vtkSetMacro(UseReductionFactor, int);
  vtkGetMacro(UseReductionFactor, int);
  vtkBooleanMacro(UseReductionFactor, int);
  
  // Description:
  // The render view keeps track of these times but does not use them.
  vtkGetMacro(StillRenderTime, double);
  vtkGetMacro(InteractiveRenderTime, double);
  vtkGetMacro(StillCompositeTime, double);
  vtkGetMacro(InteractiveCompositeTime, double);

  // Description:
  // Callback for toggling between parallel and perspective.
  void ParallelProjectionCallback();
  
  // Description:
  // Callback for the triangle strips check button
  void TriangleStripsCallback();
  
  // Description:
  // Callback for the immediate mode rendering check button
  void ImmediateModeCallback();
  
  // Description:
  // Callback for the interrupt render check button
  void InterruptRenderCallback();
  
  // Description:
  // Callback for the use char check button.  
  // These are only public because they are callbacks.
  // Cannot be used from a script because they do not 
  // change the state of the check.
  void CompositeWithFloatCallback();
  void CompositeWithFloatCallback(int val);
  void CompositeWithRGBACallback();
  void CompositeWithRGBACallback(int val);
  void CompositeCompressionCallback();
  void CompositeCompressionCallback(int val);
  
  // Description:
  // Get the triangle strips check button.
  vtkGetObjectMacro(TriangleStripsCheck, vtkKWCheckButton);
  
  // Description:
  // Get the immediate mode rendering check button.
  vtkGetObjectMacro(ImmediateModeCheck, vtkKWCheckButton);

  // Description:
  // A convience method to get the PVWindow.
  vtkPVWindow *GetPVWindow();

  // Description:
  // Used to temporarily disable rendering. Useful for collecting a few
  // renders and flusing them out at the end with one render
  vtkSetMacro(DisableRenderingFlag, int);
  vtkGetMacro(DisableRenderingFlag, int);
  vtkBooleanMacro(DisableRenderingFlag, int);

  // Description:
  // Get the widget that controls the interactor styles.
  vtkGetObjectMacro(ManipulatorControl3D, vtkPVInteractorStyleControl);
  vtkGetObjectMacro(ManipulatorControl2D, vtkPVInteractorStyleControl);

  // Description:
  // Get the size of the render window.
  int* GetRenderWindowSize();

  // Description:
  // Setup camera manipulators by populating the control and setting
  // initial values.
  void SetupCameraManipulators();

  // Description:
  // Update manipulators after they were added to control.
  void UpdateCameraManipulators();

  // Description:
  // Here so that sources get packed in the second frame.
  virtual vtkKWWidget *GetSourceParent();
  vtkKWSplitFrame *GetSplitFrame() {return this->SplitFrame;}

  // Description:
  // Threshold for individual actors as number of points.
  void LODThresholdScaleCallback();

  // Description:
  // This method sets the threshold without tracing or
  // changing the UI scale.
  void SetLODThresholdInternal(float threshold);

  // Description:
  // This methods can be used from a script.  
  // "Set" sets the value of the scale, and adds an entry to the trace.
  void SetLODThreshold(float);
  vtkGetMacro(LODThreshold, float);
  vtkBooleanMacro(LODThreshold, float);


  // Description:
  // LOD resolution determines how many cells are in decimated model.
  void LODResolutionScaleCallback();
  void LODResolutionLabelCallback();

  // Description:
  // This method sets the resolution without tracing or
  // changing the UI scale.
  void SetLODResolutionInternal(int threshold);

  // Description:
  // This methods can be used from a script.  
  // "Set" sets the value of the scale, and adds an entry to the trace.
  void SetLODResolution(int);
  vtkGetMacro(LODResolution, int);
  vtkBooleanMacro(LODResolution, int);

  // Description:
  // This method is called when an event is called that PVRenderView
  // is interested in.
  void ExecuteEvent(vtkObject* wdg, unsigned long event, void* calldata);

  // Description:
  // Threshold for collecting data to a single process (MBytes).
  void CollectThresholdScaleCallback();

  // Description:
  // This method sets the threshold without tracing or
  // changing the UI scale.
  void SetCollectThresholdInternal(float threshold);

  // Description:
  // This methods can be used from a script.  
  // "Set" sets the value of the scale, and adds an entry to the trace.
  void SetCollectThreshold(float);
  vtkGetMacro(CollectThreshold, float);
  vtkBooleanMacro(CollectThreshold, float);

  // Description:
  // Store current camera at a specified position. This stores all the
  // camera parameters and generates a small icon.
  void StoreCurrentCamera(int position);
  
  // Description:
  // Restore current camera from a specified position.
  void RestoreCurrentCamera(int position);  

  // Description:
  // Set the global variable for always displaying 3D widgets.
  void Display3DWidgetsCallback();
  void SetDisplay3DWidgets(int s);

  // Description:
  // Show the names in sources browser.
  void SetSourcesBrowserAlwaysShowName(int s);

  // Description:
  // Switch to the View Properties menu back and forth
  void SwitchBackAndForthToViewProperties();

protected:
  vtkPVRenderView();
  ~vtkPVRenderView();

  virtual void SerializeRevision(ostream& os, vtkIndent indent);
  virtual void SerializeSelf(ostream& os, vtkIndent indent);
  virtual void SerializeToken(istream& is, const char token[1024]);

  void CalculateBBox(char* name, int bbox[4]);

  // This is used before a render to make sure all visible sources
  // have been updated.  It returns 1 if all the data has been collected
  // and the render should be local.
  int UpdateAllPVData(int interactive);
 
  int Interactive;

  int UseReductionFactor;
  
  vtkPVTreeComposite *Composite;
  char *CompositeTclName;
  vtkSetStringMacro(CompositeTclName);

  char *RendererTclName;
  vtkSetStringMacro(RendererTclName);  
   
  char *RenderWindowTclName;
  vtkSetStringMacro(RenderWindowTclName);  
    
  vtkKWLabeledFrame *StandardViewsFrame;
  vtkKWPushButton   *XMaxViewButton; 
  vtkKWPushButton   *XMinViewButton; 
  vtkKWPushButton   *YMaxViewButton; 
  vtkKWPushButton   *YMinViewButton; 
  vtkKWPushButton   *ZMaxViewButton; 
  vtkKWPushButton   *ZMinViewButton; 

  vtkKWLabeledFrame *LODFrame;
  vtkKWLabeledFrame *RenderParametersFrame;
  vtkKWLabeledFrame *ParallelRenderParametersFrame;
  vtkKWCheckButton *TriangleStripsCheck;
  vtkKWCheckButton *ParallelProjectionCheck;
  vtkKWCheckButton *ImmediateModeCheck;
  vtkKWCheckButton *InterruptRenderCheck;
  vtkKWCheckButton *CompositeWithFloatCheck;
  vtkKWCheckButton *CompositeWithRGBACheck;
  vtkKWCheckButton *CompositeCompressionCheck;

  vtkKWLabeledFrame *InterfaceSettingsFrame;
  vtkKWCheckButton *Display3DWidgets;

  vtkKWWidget *LODScalesFrame;
  vtkKWLabel *LODResolutionLabel;
  vtkKWScale *LODResolutionScale;
  vtkKWLabel *LODResolutionValue;
  vtkKWLabel *LODThresholdLabel;
  vtkKWScale *LODThresholdScale;
  vtkKWLabel *LODThresholdValue;

  vtkKWLabel *CollectThresholdLabel;
  vtkKWScale *CollectThresholdScale;
  vtkKWLabel *CollectThresholdValue;

  vtkKWSplitFrame *SplitFrame;

  vtkKWLabeledFrame* NavigationFrame;
  vtkPVSourcesNavigationWindow* NavigationWindow;
  vtkPVSourcesNavigationWindow* SelectionWindow;
  vtkKWRadioButton *NavigationWindowButton;
  vtkKWRadioButton *SelectionWindowButton;
  
  int EventuallyRenderFlag;
  char* RenderPending;
  vtkSetStringMacro(RenderPending);

  double StillRenderTime;
  double InteractiveRenderTime;
  double StillCompositeTime;
  double InteractiveCompositeTime;

  int ShowSelectionWindow;
  int ShowNavigationWindow;

  // For the renderer in a separate toplevel window.
  vtkKWWidget *TopLevelRenderWindow;

  vtkPVInteractorStyleControl *ManipulatorControl2D;
  vtkPVInteractorStyleControl *ManipulatorControl3D;

  int DisableRenderingFlag;

  float LODThreshold;
  int LODResolution;
  float CollectThreshold;

  // Camera icons
  vtkKWLabeledFrame* CameraIconsFrame;
  vtkPVCameraIcon* CameraIcons[6];

  vtkPVRenderViewObserver* Observer;

  vtkKWPushButton *PropertiesButton;

  char *MenuLabelSwitchBackAndForthToViewProperties;
  vtkSetStringMacro(MenuLabelSwitchBackAndForthToViewProperties);

  vtkPVRenderView(const vtkPVRenderView&); // Not implemented
  void operator=(const vtkPVRenderView&); // Not implemented
};


#endif
