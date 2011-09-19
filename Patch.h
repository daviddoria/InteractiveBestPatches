#ifndef PATCH_H
#define PATCH_H

#include "itkImageRegion.h"

class Patch
{
public:
  Patch();
  
  Patch(const itk::ImageRegion<2>& region);
    
  itk::ImageRegion<2> Region;
  float Score;
  unsigned int Id;
};

//inline bool operator<(Patch patch1, Patch patch2); // without inline, there are "multiple definition" errors
bool operator<(Patch patch1, Patch patch2); // without inline, there are "multiple definition" errors

#endif
