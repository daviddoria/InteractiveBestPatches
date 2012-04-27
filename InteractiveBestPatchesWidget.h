/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef InteractiveBestPatchesWidget_H
#define InteractiveBestPatchesWidget_H

#include "ui_InteractiveBestPatchesWidget.h"

// VTK
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkImageSlice;
class vtkImageSliceMapper;

// ITK
#include "itkImage.h"

// Qt
#include <QMainWindow>
#include <QImage>

// Custom
#include "Types.h"
#include "SelfPatchCompare.h"

// Submodules
#include "Mask/Mask.h"

class SwitchBetweenStyle;

class InteractiveBestPatchesWidget : public QMainWindow, public Ui::InteractiveBestPatchesWidget
{
  Q_OBJECT
public:

  // Constructor/Destructor
  InteractiveBestPatchesWidget();
  InteractiveBestPatchesWidget(const std::string& imageFileName, const std::string& maskFileName);
  void SharedConstructor();
  ~InteractiveBestPatchesWidget() {};
  
  // These function deal with flipping the image
  void SetCameraPosition(const double leftToRight[3], const double bottomToTop[3]);
  void SetCameraPosition1();
  void SetCameraPosition2();
  
  void Refresh();
  
  const static unsigned int DisplayPatchSize = 50;
  
  void DisplaySourcePatches();
  
  void PositionTarget();
  
public slots:
  
  void PatchClickedSlot(const unsigned int);
  
  void on_chkShowMask_clicked();
  
  void on_actionOpenImage_activated();
  void on_actionOpenMask_activated();
  void on_actionOpenMaskInverted_activated();
  
  void on_actionHelp_activated();
  void on_actionQuit_activated();
  
  void on_actionFlipImage_activated();
  
  void on_txtPatchRadius_returnPressed();
  void on_txtNumberOfPatches_returnPressed();
  
  void on_txtTargetX_returnPressed();
  void on_txtTargetY_returnPressed();
  
  void on_btnCompute_clicked();
  void on_btnResort_clicked();

  void on_chkFillPatch_clicked();
  
  void RefreshSlot();
  
  
protected:
  
  QImage GetQImage(const itk::ImageRegion<2>& region);
  QImage GetTargetQImage(const itk::ImageRegion<2>& region);
  
  void SetMaskedPixelsToGreen(const itk::ImageRegion<2>& targetRegion, vtkImageData* image);

  void LoadMask(const std::string& filename);
  void LoadImage(const std::string& filename);
  
  static const unsigned char Green[3];
  static const unsigned char Red[3];
  
  void GetPatchSize();
  
  itk::ImageRegion<2> GetTargetRegion();
  
  void InitializePatch(vtkImageData* image, const unsigned char color[3]);
  
  void PatchesMoved();
  void SetupPatches();
  
  // Allow us to interact with the objects as we would like.
  vtkSmartPointer<SwitchBetweenStyle> InteractorStyle;
  
  // Track if the image has been flipped
  bool Flipped;

  vtkSmartPointer<vtkRenderer> Renderer;
  
  // Image display
  vtkSmartPointer<vtkImageData> VTKImage;
  vtkSmartPointer<vtkImageSlice> ImageSlice;
  vtkSmartPointer<vtkImageSliceMapper> ImageSliceMapper;
  
  // Mask image display
  vtkSmartPointer<vtkImageData> VTKMaskImage;
  vtkSmartPointer<vtkImageSlice> MaskImageSlice;
  vtkSmartPointer<vtkImageSliceMapper> MaskImageSliceMapper;
  
  // Movable target patch
  vtkSmartPointer<vtkImageData> TargetPatch;
  vtkSmartPointer<vtkImageSlice> TargetPatchSlice;
  vtkSmartPointer<vtkImageSliceMapper> TargetPatchSliceMapper;
  
  // Source patch (to indicate where one of the best matches came from)
  vtkSmartPointer<vtkImageData> SourcePatch;
  vtkSmartPointer<vtkImageSlice> SourcePatchSlice;
  vtkSmartPointer<vtkImageSliceMapper> SourcePatchSliceMapper;
    
  // The data that the user loads
  FloatVectorImageType::Pointer Image;
  Mask::Pointer MaskImage;
  
  itk::Size<2> PatchSize;
  unsigned int PatchScale;
  
  QGraphicsScene* SourcePatchesScene;
  QGraphicsScene* TargetPatchScene;
  
  SelfPatchCompare PatchCompare;

  unsigned int DisplayedSourcePatch;
};

#endif // InteractiveBestPatchesWidget_H
