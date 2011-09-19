#include "SwitchBetweenStyle.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(SwitchBetweenStyle);


SwitchBetweenStyle::SwitchBetweenStyle()
{
  this->ImageStyle = vtkSmartPointer<CustomImageStyle>::New();
  
  this->TrackballStyle = vtkSmartPointer<CustomTrackballStyle>::New();

}

void SwitchBetweenStyle::Init()
{

  this->ImageStyle->SetOtherStyle(this->TrackballStyle);
  this->TrackballStyle->SetOtherStyle(this->ImageStyle);

  this->Interactor->SetInteractorStyle(this->ImageStyle);
}

void SwitchBetweenStyle::SetCurrentRenderer(vtkRenderer* renderer)
{
  this->ImageStyle->SetCurrentRenderer(renderer);
  this->TrackballStyle->SetCurrentRenderer(renderer);
}

void SwitchBetweenStyle::SetImageOrientation(const double* leftToRight, const double* bottomToTop)
{
  this->ImageStyle->SetImageOrientation(leftToRight, bottomToTop);
}
