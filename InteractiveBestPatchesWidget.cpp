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

// #include "ui_InteractiveBestPatchesWidget.h"

#include "InteractiveBestPatchesWidget.h"

// ITK
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkVector.h"

// Qt
#include <QFileDialog>
#include <QIcon>
#include <QTextEdit>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>

// VTK
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkProperty2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkImageSliceMapper.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLImageDataWriter.h> // For debugging only

// Custom
#include "ClickableLabel.h"
#include "Helpers.h"
#include "Mask.h"
#include "SwitchBetweenStyle.h"
//#include "MyGraphicsItem.h"
#include "Types.h"

const unsigned char InteractiveBestPatchesWidget::Green[3] = {0,255,0};
const unsigned char InteractiveBestPatchesWidget::Red[3] = {255,0,0};

void InteractiveBestPatchesWidget::on_actionHelp_activated()
{
  QTextEdit* help=new QTextEdit();
  
  help->setReadOnly(true);
  help->append("<h1>Interactive Patch Comparison</h1>\
  Position the two patches. <br/>\
  Their difference will be displayed.<br/> <p/>"
  );
  help->show();
}

InteractiveBestPatchesWidget::InteractiveBestPatchesWidget(const std::string& imageFileName, const std::string& maskFileName)
{
  SharedConstructor();
  LoadImage(imageFileName);
  LoadMask(maskFileName);
}

// Constructor
InteractiveBestPatchesWidget::InteractiveBestPatchesWidget()
{
  SharedConstructor();
}

void InteractiveBestPatchesWidget::SharedConstructor()
{
  this->setupUi(this);
  
  this->TargetPatchScene = new QGraphicsScene();
  this->gfxTarget->setScene(TargetPatchScene);
  
  this->PatchScale = 5;
  
  // Setup icons
  QIcon openIcon = QIcon::fromTheme("document-open");
  QIcon saveIcon = QIcon::fromTheme("document-save");
  
  // Setup toolbar
  actionOpenImage->setIcon(openIcon);
  this->toolBar->addAction(actionOpenImage);

  actionOpenMask->setIcon(openIcon);
  this->toolBar->addAction(actionOpenMask);
  actionOpenMask->setEnabled(false);

  actionSaveResult->setIcon(saveIcon);
  this->toolBar->addAction(actionSaveResult);

  this->Flipped = false;
  
  this->InteractorStyle = vtkSmartPointer<SwitchBetweenStyle>::New();
  
  // Initialize and link the image display objects
  this->VTKImage = vtkSmartPointer<vtkImageData>::New();
  this->ImageSlice = vtkSmartPointer<vtkImageSlice>::New();
  this->ImageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
  this->ImageSliceMapper->BorderOn();
  this->ImageSlice->PickableOff();
  this->ImageSliceMapper->SetInputData(this->VTKImage);
  this->ImageSlice->SetMapper(this->ImageSliceMapper);
  this->ImageSlice->GetProperty()->SetInterpolationTypeToNearest();
  
  // Initialize and link the mask image display objects
  this->VTKMaskImage = vtkSmartPointer<vtkImageData>::New();
  this->MaskImageSlice = vtkSmartPointer<vtkImageSlice>::New();
  this->MaskImageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
  this->MaskImageSlice->PickableOff();
  this->MaskImageSliceMapper->BorderOn();
  this->MaskImageSliceMapper->SetInputData(this->VTKMaskImage);
  this->MaskImageSlice->SetMapper(this->MaskImageSliceMapper);
  this->MaskImageSlice->GetProperty()->SetInterpolationTypeToNearest();
  
  // Initialize patches
  this->SourcePatch = vtkSmartPointer<vtkImageData>::New();
  this->SourcePatchSlice = vtkSmartPointer<vtkImageSlice>::New();
  this->SourcePatchSlice->PickableOff();
  this->SourcePatchSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
  this->SourcePatchSliceMapper->BorderOn();
  this->SourcePatchSliceMapper->SetInputData(this->SourcePatch);
  this->SourcePatchSlice->SetMapper(this->SourcePatchSliceMapper);
  this->SourcePatchSlice->GetProperty()->SetInterpolationTypeToNearest();
  
  this->TargetPatch = vtkSmartPointer<vtkImageData>::New();
  this->TargetPatchSlice = vtkSmartPointer<vtkImageSlice>::New();
  this->TargetPatchSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
  this->TargetPatchSliceMapper->BorderOn();
  this->TargetPatchSliceMapper->SetInputData(this->TargetPatch);
  this->TargetPatchSlice->SetMapper(this->TargetPatchSliceMapper);
  this->TargetPatchSlice->GetProperty()->SetInterpolationTypeToNearest();
  
  // Add objects to the renderer
  this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->qvtkWidget->GetRenderWindow()->AddRenderer(this->Renderer);
  
  this->Renderer->AddViewProp(this->ImageSlice);
  this->Renderer->AddViewProp(this->MaskImageSlice);
  this->Renderer->AddViewProp(this->SourcePatchSlice);
  this->Renderer->AddViewProp(this->TargetPatchSlice);

  this->InteractorStyle->SetCurrentRenderer(this->Renderer);
  this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->InteractorStyle);
  this->InteractorStyle->Init();

  this->Image = NULL;
  this->MaskImage = NULL;
  
  this->InteractorStyle->TrackballStyle->AddObserver(CustomTrackballStyle::PatchesMovedEvent,
                                                     this, &InteractiveBestPatchesWidget::PatchesMoved);
  
  this->tableWidget->resizeColumnsToContents();
};

void InteractiveBestPatchesWidget::on_btnResort_clicked()
{
  if(this->radTotalAbsolute->isChecked())
    {
    std::sort(this->PatchCompare.SourcePatches.begin(), this->PatchCompare.SourcePatches.end(), SortByTotalAbsoluteScore);
    }
  else if(this->radAverageAbsolute->isChecked())
    {
    std::sort(this->PatchCompare.SourcePatches.begin(), this->PatchCompare.SourcePatches.end(), SortByAverageAbsoluteScore);
    }
  else if(this->radTotalSquared->isChecked())
    {
    std::sort(this->PatchCompare.SourcePatches.begin(), this->PatchCompare.SourcePatches.end(), SortByTotalSquaredScore);
    }
  else if(this->radAverageSquared->isChecked())
    {
    std::sort(this->PatchCompare.SourcePatches.begin(), this->PatchCompare.SourcePatches.end(), SortByAverageSquaredScore);
    }
    
  DisplaySourcePatches();
}

void InteractiveBestPatchesWidget::on_actionQuit_activated()
{
  exit(0);
}

void InteractiveBestPatchesWidget::LoadImage(const std::string& fileName)
{
  // Set the working directory
  QFileInfo fileInfo(fileName.c_str());
  std::string workingDirectory = fileInfo.absoluteDir().absolutePath().toStdString() + "/";
  std::cout << "Working directory set to: " << workingDirectory << std::endl;
  QDir::setCurrent(QString(workingDirectory.c_str()));

  typedef itk::ImageFileReader<FloatVectorImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();

  //this->Image = reader->GetOutput();
  this->Image = FloatVectorImageType::New();
  Helpers::DeepCopyVectorImage<FloatVectorImageType>(reader->GetOutput(), this->Image);

  Helpers::ITKImagetoVTKImage(this->Image, this->VTKImage);

  this->statusBar()->showMessage("Opened image.");
  actionOpenMask->setEnabled(true);

  GetPatchSize();

  // Initialize (Have to cast these because a negative unsigned int is undefined)
  this->SourcePatchSlice->SetPosition(-1.*static_cast<float>(this->PatchSize[0]), -1.*static_cast<float>(this->PatchSize[0]), 0); 

  this->TargetPatchSlice->SetPosition(this->Image->GetLargestPossibleRegion().GetSize()[0]/2 + this->PatchSize[0],
                                      this->Image->GetLargestPossibleRegion().GetSize()[1]/2, 0);

  SetupPatches();

  PatchesMoved();

  this->Renderer->ResetCamera();

  Refresh();
}

void InteractiveBestPatchesWidget::on_actionOpenImage_activated()
{
  // Get a filename to open
  QString fileName = QFileDialog::getOpenFileName(this, "Open File", ".",
                                                  "Image Files (*.jpg *.jpeg *.bmp *.png *.mha);;PNG Files (*.png)");

  std::cout << "Got filename: " << fileName.toStdString() << std::endl;
  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  LoadImage(fileName.toStdString());
}

void InteractiveBestPatchesWidget::on_txtPatchRadius_returnPressed()
{
  SetupPatches();
}

void InteractiveBestPatchesWidget::PositionTarget()
{
  double position[3];
  this->TargetPatchSlice->GetPosition(position);
  position[0] = txtTargetX->text().toUInt();
  position[1] = txtTargetY->text().toUInt();
  this->TargetPatchSlice->SetPosition(position);
  
  PatchesMoved();
}

void InteractiveBestPatchesWidget::on_txtTargetX_returnPressed()
{
  PositionTarget();  
}

void InteractiveBestPatchesWidget::on_txtTargetY_returnPressed()
{
  PositionTarget();
}

void InteractiveBestPatchesWidget::GetPatchSize()
{
  // The edge length of the patch is the (radius*2) + 1
  this->PatchSize[0] = this->txtPatchRadius->text().toUInt() * 2 + 1;
  this->PatchSize[1] = this->txtPatchRadius->text().toUInt() * 2 + 1;
}

void InteractiveBestPatchesWidget::SetupPatches()
{
  GetPatchSize();

  InitializePatch(this->SourcePatch, this->Green);
  
  InitializePatch(this->TargetPatch, this->Red);
  
  PatchesMoved();
  Refresh();
}

void InteractiveBestPatchesWidget::InitializePatch(vtkImageData* image, const unsigned char color[3])
{
  // Setup and allocate the image data
  image->SetDimensions(this->PatchSize[0], this->PatchSize[1], 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);
  
  Helpers::BlankAndOutlineImage(image,color);
}

void InteractiveBestPatchesWidget::on_actionOpenMaskInverted_activated()
{
  std::cout << "on_actionOpenMaskInverted_activated()" << std::endl;
  on_actionOpenMask_activated();
  this->MaskImage->Invert();
  this->MaskImage->Cleanup();
  }

void InteractiveBestPatchesWidget::LoadMask(const std::string& fileName)
{

  typedef itk::ImageFileReader<Mask> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();

  if(this->Image->GetLargestPossibleRegion() != reader->GetOutput()->GetLargestPossibleRegion())
    {
    std::cerr << "Image and mask must be the same size!" << std::endl;
    return;
    }
  this->MaskImage = Mask::New();
  Helpers::DeepCopy<Mask>(reader->GetOutput(), this->MaskImage);

  // For this program, we ALWAYS assume the hole to be filled is white, and the valid/source region is black.
  // This is not simply reversible because of some subtle erosion operations that are performed.
  // For this reason, we provide an "load inverted mask" action in the file menu.
  this->MaskImage->SetValidValue(0);
  this->MaskImage->SetHoleValue(255);

  this->MaskImage->Cleanup();

  Helpers::SetMaskTransparency(this->MaskImage, this->VTKMaskImage);

}

void InteractiveBestPatchesWidget::on_actionOpenMask_activated()
{
  // Get a filename to open
  QString fileName = QFileDialog::getOpenFileName(this, "Open File", ".", "Image Files (*.png *.bmp);;Image Files (*.mha)");

  std::cout << "Got filename: " << fileName.toStdString() << std::endl;
  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  LoadMask(fileName.toStdString());

  this->statusBar()->showMessage("Opened mask.");
}

void InteractiveBestPatchesWidget::RefreshSlot()
{
  Refresh();
}

void InteractiveBestPatchesWidget::Refresh()
{
  //std::cout << "Refresh()" << std::endl;
  
  this->MaskImageSlice->SetVisibility(this->chkShowMask->isChecked());
  
  this->qvtkWidget->GetRenderWindow()->Render();
  
}

void InteractiveBestPatchesWidget::SetCameraPosition1()
{
  double leftToRight[3] = {-1,0,0};
  double bottomToTop[3] = {0,1,0};
  SetCameraPosition(leftToRight, bottomToTop);
}

void InteractiveBestPatchesWidget::SetCameraPosition2()
{
  double leftToRight[3] = {-1,0,0};
  double bottomToTop[3] = {0,-1,0};

  SetCameraPosition(leftToRight, bottomToTop);
}

void InteractiveBestPatchesWidget::SetCameraPosition(const double leftToRight[3], const double bottomToTop[3])
{
  this->InteractorStyle->SetImageOrientation(leftToRight, bottomToTop);

  this->Renderer->ResetCamera();
  this->Renderer->ResetCameraClippingRange();
  this->qvtkWidget->GetRenderWindow()->Render();
}


void InteractiveBestPatchesWidget::on_actionFlipImage_activated()
{
  if(this->Flipped)
    {
    SetCameraPosition1();
    }
  else
    {
    SetCameraPosition2();
    }
  this->Flipped = !this->Flipped;
}

void InteractiveBestPatchesWidget::PatchesMoved()
{
  //std::cout << "Patches moved." << std::endl;

  
  double targetPosition[3];
  this->TargetPatchSlice->GetPosition(targetPosition);
      
  itk::Index<2> targetIndex;
  targetIndex[0] = targetPosition[0];
  targetIndex[1] = targetPosition[1];
  
  // Snap to grid
  targetPosition[0] = targetIndex[0];
  targetPosition[1] = targetIndex[1];
  this->TargetPatchSlice->SetPosition(targetPosition);
  
  this->txtTargetX->setText(QString::number(targetIndex[0]));
  this->txtTargetY->setText(QString::number(targetIndex[1]));
  
  itk::ImageRegion<2> targetRegion(targetIndex, this->PatchSize);
  
  QImage targetImage = GetTargetQImage(targetRegion);
  
  this->TargetPatchScene->addPixmap(QPixmap::fromImage(targetImage));

  Refresh();

}

QImage InteractiveBestPatchesWidget::GetQImage(const itk::ImageRegion<2>& region)
{
  
  QImage qimage(region.GetSize()[0], region.GetSize()[1], QImage::Format_RGB888); // Should probably be Format_RGB888
  itk::ImageRegionIterator<FloatVectorImageType> imageIterator(this->Image, region);

  while(!imageIterator.IsAtEnd())
    {
    FloatVectorImageType::PixelType pixel = imageIterator.Get();
    
    itk::Index<2> index;
    index[0] = imageIterator.GetIndex()[0] - region.GetIndex()[0];
    index[1] = imageIterator.GetIndex()[1] - region.GetIndex()[1];

    QColor pixelColor(static_cast<int>(pixel[0]), static_cast<int>(pixel[1]), static_cast<int>(pixel[2]));
    qimage.setPixel(index[0], index[1], QColor(static_cast<int>(pixel[0]), static_cast<int>(pixel[1]), static_cast<int>(pixel[2])).rgb());
    
    ++imageIterator;
    }

  //return qimage; // The actual image region
  //return qimage.mirrored(false, true); // The flipped image region
  return qimage.mirrored(false, true).scaledToHeight(DisplayPatchSize); // The flipped image region forced to 50x50 (because our patches are always square
}

QImage InteractiveBestPatchesWidget::GetTargetQImage(const itk::ImageRegion<2>& region)
{
  
  QImage qimage(region.GetSize()[0], region.GetSize()[1], QImage::Format_RGB888); // Should probably be Format_RGB888
  itk::ImageRegionIterator<FloatVectorImageType> imageIterator(this->Image, region);

  while(!imageIterator.IsAtEnd())
    {
    FloatVectorImageType::PixelType pixel = imageIterator.Get();
    
    itk::Index<2> index;
    index[0] = imageIterator.GetIndex()[0] - region.GetIndex()[0];
    index[1] = imageIterator.GetIndex()[1] - region.GetIndex()[1];

    if(this->MaskImage && this->MaskImage->IsHole(imageIterator.GetIndex()))
      {
      QColor pixelColor(0, 255, 0);
      qimage.setPixel(index[0], index[1], pixelColor.rgb());
      }
    else
      {
      QColor pixelColor(static_cast<int>(pixel[0]), static_cast<int>(pixel[1]), static_cast<int>(pixel[2]));
      qimage.setPixel(index[0], index[1], pixelColor.rgb());
      }
    
    ++imageIterator;
    }

  //return qimage; // The actual image region
  //return qimage.mirrored(false, true); // The flipped image region
  return qimage.mirrored(false, true).scaledToHeight(DisplayPatchSize); // The flipped image region forced to 50x50 (because our patches are always square
}

void InteractiveBestPatchesWidget::SetMaskedPixelsToGreen(const itk::ImageRegion<2>& targetRegion, vtkImageData* image)
{
  itk::ImageRegionIterator<Mask> maskIterator(this->MaskImage, targetRegion);

  while(!maskIterator.IsAtEnd())
    {
    if(this->MaskImage->IsHole(maskIterator.GetIndex()))
      {
      itk::Index<2> index = maskIterator.GetIndex();
      index[0] -= targetRegion.GetIndex()[0];
      index[1] -= targetRegion.GetIndex()[1];
      unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(index[0], index[1],0));
      pixel[0] = 0;
      pixel[1] = 255;
      pixel[2] = 0;
      }
    ++maskIterator;
    }  
}

void InteractiveBestPatchesWidget::on_txtNumberOfPatches_returnPressed()
{
  DisplaySourcePatches();
  Refresh();
}

void InteractiveBestPatchesWidget::DisplaySourcePatches()
{
  
  unsigned int numberOfPatches = this->txtNumberOfPatches->text().toUInt();
  
  if(numberOfPatches > this->PatchCompare.SourcePatches.size())
    {
    std::cout << "You have requested more patches (" << numberOfPatches << ") than have been computed (" << this->PatchCompare.SourcePatches.size() << ")" << std::endl;
    return;
    }
    
  // Clear the table
  this->tableWidget->setRowCount(0);
  
  for(unsigned int i = 0; i < numberOfPatches; ++i)
    {
    this->tableWidget->insertRow(this->tableWidget->rowCount());
  
    Patch currentPatch = this->PatchCompare.SourcePatches[i];
    std::cout << "Generating table row for " << currentPatch.Region << std::endl;
  
    QImage sourceImage = GetQImage(currentPatch.Region);
  
    ClickableLabel* imageLabel = new ClickableLabel;
    imageLabel->setPixmap(QPixmap::fromImage(sourceImage));
    imageLabel->Id = i; // This is the ith best match
    imageLabel->setScaledContents(false);
    this->tableWidget->setCellWidget(i,0,imageLabel);
            
    connect( imageLabel, SIGNAL( ClickedSignal(unsigned int) ), this, SLOT(PatchClickedSlot(unsigned int)) );
    
    std::stringstream ssLabel;
    ssLabel << "( " << currentPatch.Region.GetIndex()[0] << ", " << currentPatch.Region.GetIndex()[1] << ")";
    
    QTableWidgetItem* indexLabel = new QTableWidgetItem;
    indexLabel->setText(ssLabel.str().c_str());
    this->tableWidget->setItem(i,1,indexLabel);

    // Total absolute score
    QTableWidgetItem* totalAbsoluteScoreLabel = new QTableWidgetItem;
    totalAbsoluteScoreLabel->setData(Qt::DisplayRole, currentPatch.TotalAbsoluteScore);
    this->tableWidget->setItem(i,2,totalAbsoluteScoreLabel);

    // Average absolute score
    QTableWidgetItem* averageAbsoluteScoreLabel = new QTableWidgetItem;
    averageAbsoluteScoreLabel->setData(Qt::DisplayRole, currentPatch.AverageAbsoluteScore);
    this->tableWidget->setItem(i,3,averageAbsoluteScoreLabel);

    // Total squared score
    QTableWidgetItem* totalSquaredScoreLabel = new QTableWidgetItem;
    totalSquaredScoreLabel->setData(Qt::DisplayRole, currentPatch.TotalSquaredScore);
    this->tableWidget->setItem(i,4,totalSquaredScoreLabel);

    // Average squared score
    QTableWidgetItem* averageSquaredScoreLabel = new QTableWidgetItem;
    averageSquaredScoreLabel->setData(Qt::DisplayRole, currentPatch.AverageSquaredScore);
    this->tableWidget->setItem(i,5,averageSquaredScoreLabel);
    }
    
  this->tableWidget->resizeRowsToContents();
  this->tableWidget->resizeColumnsToContents();
  
}

void InteractiveBestPatchesWidget::on_btnCompute_clicked()
{
  PositionTarget();
  
  this->PatchCompare.SetNumberOfComponentsPerPixel(this->Image->GetNumberOfComponentsPerPixel());
  this->PatchCompare.SetImage(this->Image);
  this->PatchCompare.SetMask(this->MaskImage);
  this->PatchCompare.SetTargetRegion(GetTargetRegion());
  
  // This checks to see if both the image and mask have been set to something non-NULL
  if(!this->PatchCompare.IsReady())
    {
    std::cout << "Not ready to compute! Image, MaskImage or NumberOfComponentsPerPixel\
                  may not be set on the PatchCompare object!" << std::endl;
    return;
    }

  this->PatchCompare.ComputePatchScores();
  
  DisplaySourcePatches();

  // Automatically display the best patch
  PatchClickedSlot(0);
}

itk::ImageRegion<2> InteractiveBestPatchesWidget::GetTargetRegion()
{
  double targetPosition[3];
  this->TargetPatchSlice->GetPosition(targetPosition);
      
  itk::Index<2> targetIndex;
  targetIndex[0] = targetPosition[0];
  targetIndex[1] = targetPosition[1];
  
  itk::ImageRegion<2> region(targetIndex, this->PatchSize);
  return region;
}

void InteractiveBestPatchesWidget::on_chkFillPatch_clicked()
{
  PatchClickedSlot(this->DisplayedSourcePatch);
}

void InteractiveBestPatchesWidget::PatchClickedSlot(const unsigned int value)
{
  // 'value' here is the "ith best match". E.g. the third best match would have value=2.
  this->DisplayedSourcePatch = value;
  
  std::cout << "PatchClickedSlot " << value << std::endl;
  
  Patch patch = this->PatchCompare.SourcePatches[value];
  
  std::cout << "Region: " << patch.Region << std::endl;
  
  this->SourcePatchSlice->SetPosition(patch.Region.GetIndex()[0], patch.Region.GetIndex()[1], 0);


  if(this->chkFillPatch->isChecked())
    {
    Helpers::ITKRegionToVTKImage(this->Image, patch.Region, this->TargetPatch);
    Helpers::OutlineImage(this->TargetPatch, this->Red);
    }
  else
    {
    Helpers::BlankAndOutlineImage(this->TargetPatch, this->Red);
    }

  Refresh();
}

void InteractiveBestPatchesWidget::on_chkShowMask_clicked()
{
  Refresh();
}
