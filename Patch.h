#ifndef PATCH_H
#define PATCH_H

#include "itkImageRegion.h"

class Patch
{
public:
  Patch();
  
  Patch(const itk::ImageRegion<2>& region);
    
  itk::ImageRegion<2> Region;
  float TotalAbsoluteScore;
  float AverageAbsoluteScore;
  float TotalSquaredScore;
  float AverageSquaredScore;
  unsigned int Id;

  void DefaultConstructor();
};

bool SortByTotalAbsoluteScore(const Patch& patch1, const Patch& patch2);
bool SortByAverageAbsoluteScore(const Patch& patch1, const Patch& patch2);
bool SortByTotalSquaredScore(const Patch& patch1, const Patch& patch2);
bool SortByAverageSquaredScore(const Patch& patch1, const Patch& patch2);

#endif
