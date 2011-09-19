#ifndef CustomImageStyle_H
#define CustomImageStyle_H

#include "CustomImageStyle.h"
#include <vtkInteractorStyleImage.h>

//#include "CustomTrackballStyle.h"
class CustomTrackballStyle;

// Define interaction style
class CustomImageStyle : public vtkInteractorStyleImage
{
  public:
    static CustomImageStyle* New();
    vtkTypeMacro(CustomImageStyle,vtkInteractorStyleImage);

    void OnLeftButtonDown();

    void SetOtherStyle(CustomTrackballStyle* style);

  
private:
  CustomTrackballStyle* OtherStyle;
  
};

#endif
