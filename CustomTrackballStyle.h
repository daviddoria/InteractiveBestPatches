#ifndef CustomTrackballStyle_H
#define CustomTrackballStyle_H

#include "CustomTrackballStyle.h"

#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballActor.h>

//#include "CustomImageStyle.h"
class CustomImageStyle;

// Define interaction style
class CustomTrackballStyle : public vtkInteractorStyleTrackballActor
{
  public:
    const static unsigned int PatchesMovedEvent = vtkCommand::UserEvent + 1;
    
    static CustomTrackballStyle* New();
    vtkTypeMacro(CustomTrackballStyle,vtkInteractorStyleTrackballActor);

    void OnLeftButtonDown();

    void OnLeftButtonUp();

    void OnMiddleButtonDown();

    void OnRightButtonDown();

    void SetOtherStyle(CustomImageStyle* style);

private:
  CustomImageStyle* OtherStyle;
  
};

#endif
