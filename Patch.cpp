#include "Patch.h"

void Patch::DefaultConstructor()
{
  this->TotalAbsoluteScore = 0.0f;
  this->AverageAbsoluteScore = 0.0f;
  this->TotalSquaredScore = 0.0f;
  this->AverageSquaredScore = 0.0f;
  this->Id = 0;
}

Patch::Patch()
{
  DefaultConstructor();
}

Patch::Patch(const itk::ImageRegion<2>& region)
{
  DefaultConstructor();
  this->Region = region;
}
  
bool SortByTotalAbsoluteScore(const Patch& patch1, const Patch& patch2)
{
  return patch1.TotalAbsoluteScore < patch2.TotalAbsoluteScore;
}

bool SortByAverageAbsoluteScore(const Patch& patch1, const Patch& patch2)
{
  return patch1.AverageAbsoluteScore < patch2.AverageAbsoluteScore;
}

bool SortByTotalSquaredScore(const Patch& patch1, const Patch& patch2)
{
  return patch1.TotalSquaredScore < patch2.TotalSquaredScore;
}

bool SortByAverageSquaredScore(const Patch& patch1, const Patch& patch2)
{
  return patch1.AverageSquaredScore < patch2.AverageSquaredScore;
}

