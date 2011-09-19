#include "CustomImageStyle.h"

#include "CustomTrackballStyle.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(CustomImageStyle);

void CustomImageStyle::OnLeftButtonDown()
{
  //std::cout << "OnLeftButtonDown()" << std::endl;
  // Behave like the middle button instead
  this->Interactor->SetInteractorStyle(this->OtherStyle);
  this->OtherStyle->OnLeftButtonDown();
}

void CustomImageStyle::SetOtherStyle(CustomTrackballStyle* style)
{
  this->OtherStyle = style;
}