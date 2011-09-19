#include "Patch.h"

Patch::Patch()
{
  this->Score = 0;
  this->Id = 0;
}

Patch::Patch(const itk::ImageRegion<2>& region)
{
  this->Id = 0;
  this->Score = 0;
  this->Region = region;
}
  
bool operator<(Patch patch1, Patch patch2)
{
  return patch1.Score < patch2.Score;
}
