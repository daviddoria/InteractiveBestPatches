#ifndef SwitchBetweenStyle_H
#define SwitchBetweenStyle_H

#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkSmartPointer.h>

#include "CustomImageStyle.h"
#include "CustomTrackballStyle.h"

// To use this, you must do, in this order:
//  this->InteractorStyle->SetCurrentRenderer(this->Renderer);
//  this->Interactor->SetInteractorStyle(this->InteractorStyle);
//  this->InteractorStyle->Init();
    
// Define interaction style
class SwitchBetweenStyle : public vtkInteractorStyleTrackballActor
{
  public:
    
    
    static SwitchBetweenStyle* New();
    vtkTypeMacro(SwitchBetweenStyle,vtkInteractorStyleTrackballActor);

    SwitchBetweenStyle();

    void Init();

    void SetCurrentRenderer(vtkRenderer* renderer);

    void SetImageOrientation(const double*, const double*);
    
        
    vtkSmartPointer<CustomImageStyle> ImageStyle;
    vtkSmartPointer<CustomTrackballStyle> TrackballStyle;
    
  protected:

    

};

#endif
